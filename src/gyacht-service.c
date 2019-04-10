/* gyacht-service.c
 *
 * Copyright 2019 Yi-Soo An <yisooan@fedoraproject.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "gyacht-debug.h"
#include "gyacht-file-utils.h"
#include "gyacht-service.h"
#include "gyacht-service-private.h"

typedef struct
{
  GyachtRunLevel  level;
  GFileMonitor    *monitor;
  gboolean        error;
} GyachtServicePrivate;

/* Signals */
enum {
  MONITOR_EVENT_TRIGGERED,
  LIST_UPDATED,
  N_SIGNALS
};

/* Properties */
enum {
  PROP_RUN_LEVEL = 1,
  N_PROPERTIES
};

static guint signals [N_SIGNALS];
static GParamSpec* properties [N_PROPERTIES] = { NULL };

G_DEFINE_TYPE_WITH_PRIVATE (GyachtService, gyacht_service, G_TYPE_OBJECT)


static void
internal_file_monitor_changed_cb (GFileMonitor      *monitor,
                                  GFile             *file,
                                  GFile             *other_file,
                                  GFileMonitorEvent  event_type,
                                  gpointer           user_data)
{
  GyachtService *self = GYACHT_SERVICE (user_data);

  switch (event_type)
    {
    /* When a user executes create/delete commands in container cli tools[1],
     * the following events are triggered in order,
     *
     * 1. G_FILE_MONITOR_EVENT_DELETED
     * 2. G_FILE_MONITOR_EVENT_CREATED
     * 3. G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT
     *
     * But we use the third flag.
     *
     * [1]: Podman, Buildah and so on in which edit containers.json.
     */
    case G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT:
      g_signal_emit (self, signals[MONITOR_EVENT_TRIGGERED], 0);
      break;

    default:
      break;
    }
}

static void
internal_load_json_io_thread (GTask        *task,
                              gpointer      source_object,
                              gpointer      task_data,
                              GCancellable *cancellable)
{
  GyachtService *self = GYACHT_SERVICE (source_object);
  g_autoptr(GFile) location = NULL;
  JsonParser *parser = NULL;
  GError *error = NULL;

  location = GYACHT_SERVICE_GET_CLASS (self)->get_json_path (self);
  if (!gyacht_file_utils_file_exists (location, &error))
    goto out_error;
  else
    {
      g_autofree gchar *path = NULL;

      parser = json_parser_new ();
      path = g_file_get_path (location);

      json_parser_load_from_file (parser, path, &error);
      if (error)
        goto out_error;

      g_task_return_pointer (task, parser, g_object_unref);
      return;
    }

out_error:
  g_object_unref (parser);
  g_task_return_error (task, error);
}

/* --- GObject --- */
static void
gyacht_service_finalize (GObject *object)
{
  GyachtService *self = GYACHT_SERVICE (object);
  GyachtServicePrivate *priv = gyacht_service_get_instance_private (self);

  if (priv->monitor)
    g_signal_handlers_disconnect_by_func (priv->monitor,
                                          G_CALLBACK (internal_file_monitor_changed_cb),
                                          self);
  g_clear_object (&priv->monitor);

  G_OBJECT_CLASS (gyacht_service_parent_class)->finalize (object);
}

static void
gyacht_service_constructed (GObject *object)
{
  GyachtService *self = GYACHT_SERVICE (object);
  GyachtServicePrivate *priv = gyacht_service_get_instance_private (self);
  g_autoptr(GFile) location = NULL;
  g_autoptr(GError) error = NULL;

  location = GYACHT_SERVICE_GET_CLASS (self)->get_json_path (self);
  if (!gyacht_file_utils_file_exists (location, &error))
    {
      gyacht_warn ("Unable to get json contents from file: %s",
                   error->message);
      priv->error = TRUE;
      return;
    }

  priv->monitor = g_file_monitor_file (location, G_FILE_MONITOR_NONE, NULL, NULL);
  g_signal_connect (priv->monitor,
                    "changed",
                    G_CALLBACK (internal_file_monitor_changed_cb),
                    self);
}

static void
gyacht_service_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  GyachtService *self = GYACHT_SERVICE (object);
  GyachtServicePrivate *priv = gyacht_service_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_RUN_LEVEL:
      g_value_set_uint (value, priv->level);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gyacht_service_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  GyachtService *self = GYACHT_SERVICE (object);
  GyachtServicePrivate *priv = gyacht_service_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_RUN_LEVEL:
        {
          GyachtRunLevel level = g_value_get_uint (value);
          switch (level)
            {
            case RUN_LEVEL_SYS:
            case RUN_LEVEL_USER:
              break;

            default:
              gyacht_warn ("Run level is set to RUN_LEVEL_USER");
              level = RUN_LEVEL_USER;
            }
          priv->level = level;
        }
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

/* --- GyachtService class definitions --- */
static void
gyacht_service_clear_list (GyachtService *self)
{
  /* Prevent developers forget to implement children' clear_sequence() */
  g_assert_not_reached ();
}

static GFile *
gyacht_service_get_json_path (GyachtService *self)
{
  /* Prevent developers forget to implement children' get_json_path() */
  g_assert_not_reached ();

  return NULL;
}

static void
gyacht_service_class_init (GyachtServiceClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gyacht_service_finalize;
  object_class->constructed = gyacht_service_constructed;
  object_class->get_property = gyacht_service_get_property;
  object_class->set_property = gyacht_service_set_property;

  klass->clear_list = gyacht_service_clear_list;
  klass->get_json_path = gyacht_service_get_json_path;

  properties [PROP_RUN_LEVEL] =
    g_param_spec_uint ("run-level",
                       "Run Level",
                       "Whether it is system level",
                       RUN_LEVEL_NONE,
                       N_RUN_LEVELS,
                       RUN_LEVEL_NONE,
                       (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);

  signals [MONITOR_EVENT_TRIGGERED] =
    g_signal_new ("monitor-event-triggered",
                  G_TYPE_FROM_CLASS (object_class),
                  G_SIGNAL_RUN_LAST,
                  0, NULL, NULL, NULL,
                  G_TYPE_NONE, 0);

  signals [LIST_UPDATED] =
    g_signal_new ("list-updated",
                  G_TYPE_FROM_CLASS (object_class),
                  G_SIGNAL_RUN_LAST,
                  0, NULL, NULL, NULL,
                  G_TYPE_NONE, 0);
}

static void
gyacht_service_init (GyachtService *self)
{
  GyachtServicePrivate *priv = gyacht_service_get_instance_private (self);

  priv->error = FALSE;
}

/* --- Private APIs --- */
GyachtRunLevel
gyacht_service_get_run_level (GyachtService *self)
{
  GyachtServicePrivate *priv;

  g_return_val_if_fail (GYACHT_IS_SERVICE (self), RUN_LEVEL_NONE);

  priv = gyacht_service_get_instance_private (self);

  return priv->level;
}

gboolean
gyacht_service_error_occur (GyachtService *self)
{
  GyachtServicePrivate *priv;

  g_return_val_if_fail (GYACHT_IS_SERVICE (self), TRUE);

  priv = gyacht_service_get_instance_private (self);

  return priv->error;
}

void
gyacht_service_load_json_async (GyachtService       *self,
                                GCancellable        *cancellable,
                                GAsyncReadyCallback  callback,
                                gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;

  GYACHT_TRACE_ENTRY;

  g_return_if_fail (GYACHT_IS_SERVICE (self));

  GYACHT_SERVICE_GET_CLASS (self)->clear_list (self);

  task = g_task_new (G_OBJECT (self),
                     cancellable,
                     callback,
                     user_data);
  g_task_run_in_thread (task, internal_load_json_io_thread);

  GYACHT_TRACE_EXIT;
}

JsonParser *
gyacht_service_load_json_finish (GyachtService  *self,
                                 GAsyncResult   *res,
                                 GError        **error)
{
  gpointer ret = NULL;

  GYACHT_TRACE_ENTRY;

  g_return_val_if_fail (GYACHT_IS_SERVICE (self), NULL);
  g_return_val_if_fail (g_task_is_valid (res,
                                         G_OBJECT (self)), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  ret = g_task_propagate_pointer (G_TASK (res), error);

  GYACHT_TRACE_EXIT;

  return ret;
}
