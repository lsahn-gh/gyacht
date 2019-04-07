/* gyacht-container-service.c
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

#include "gyacht-container-private.h"
#include "gyacht-container-service.h"
#include "gyacht-debug.h"
#include "gyacht-file-utils.h"
#include "gyacht-path-manager.h"

#include <gio/gio.h>
#include <json-glib/json-glib.h>

struct _GyachtContainerService
{
  GyachtService   parent_instance;

  GSequence       *containers;
  GQueue          *jobs;
};

enum {
  ASYNC_JOB_ERROR = 0,
  ASYNC_JOB_LOAD
};

G_DEFINE_TYPE (GyachtContainerService, gyacht_container_service, GYACHT_TYPE_SERVICE)

/* Forward declarations */
static void internal_load_json_callback (GObject      *source_object,
                                         GAsyncResult *res,
                                         gpointer      user_data);


static GFile *
internal_get_json_path (GyachtService *service)
{
  g_autofree gchar *json_path = NULL;
  g_autofree gchar *file_dir = NULL;
  GyachtRunLevel level;

  level = gyacht_service_get_run_level (service);

  switch (level)
    {
    case RUN_LEVEL_SYS:
      /* TODO System level container dir */
      file_dir = gyacht_dup_user_containers_dir ();
      break;

    case RUN_LEVEL_USER:
    case RUN_LEVEL_NONE:
    default:
      file_dir = gyacht_dup_user_containers_dir ();
    }

  json_path = g_build_filename (file_dir, CONTAINERS_JSON, NULL);
  return g_file_new_for_path (json_path);
}

static void
internal_clear_container_list (GyachtService *service)
{
  GyachtContainerService *self = GYACHT_CONTAINER_SERVICE (service);

  if (self->containers == NULL)
    return;

  g_sequence_free (self->containers);
  self->containers = NULL;
}

static void
internal_execute_next_job (GyachtContainerService *self)
{
  g_queue_pop_head (self->jobs);

  if (!g_queue_is_empty (self->jobs))
    gyacht_service_load_json_async (GYACHT_SERVICE (self),
                                    NULL,
                                    internal_load_json_callback,
                                    NULL);
}

static void
internal_load_json_callback (GObject      *source_object,
                             GAsyncResult *res,
                             gpointer      user_data)
{
  GyachtContainerService *self = GYACHT_CONTAINER_SERVICE (source_object);
  g_autoptr(JsonParser) parser = NULL;
  g_autoptr(GError) error = NULL;
  GSequence *new_containers = NULL;

  parser = gyacht_service_load_json_finish (GYACHT_SERVICE (self),
                                            res,
                                            &error);
  if (error)
    {
      gyacht_warn ("Unable to load json contents from file: %s",
                   error->message);
      goto do_next_job;
    }

  new_containers = gyacht_container_parse_json_contents (parser, &error);
  if (error)
    {
      gyacht_warn ("Unable to parse json contents: %s",
                   error->message);
      goto do_next_job;
    }

  self->containers = new_containers;
  g_signal_emit_by_name (self, "list-updated", 0);

do_next_job:
  internal_execute_next_job (self);
}

static void
internal_load_contents (GyachtContainerService *self)
{
  GYACHT_TRACE_ENTRY;

  g_queue_push_tail (self->jobs, GINT_TO_POINTER (ASYNC_JOB_LOAD));

  /* Run if it has only one job in which is just pushed */
  if (g_queue_get_length (self->jobs) == 1)
    gyacht_service_load_json_async (GYACHT_SERVICE (self),
                                    NULL,
                                    internal_load_json_callback,
                                    NULL);

  GYACHT_TRACE_EXIT;
}

/* --- GObject --- */
static void
gyacht_container_service_finalize (GObject *object)
{
  GyachtContainerService *self = GYACHT_CONTAINER_SERVICE (object);

  GYACHT_TRACE_ENTRY;

  internal_clear_container_list (GYACHT_SERVICE (self));

  g_signal_handlers_disconnect_by_func (self,
                                        G_CALLBACK (internal_load_contents),
                                        NULL);

  g_queue_free (self->jobs);

  GYACHT_TRACE_EXIT;

  G_OBJECT_CLASS (gyacht_container_service_parent_class)->finalize (object);
}

static void
gyacht_container_service_constructed (GObject *object)
{
  GyachtContainerService *self = GYACHT_CONTAINER_SERVICE (object);
  GyachtService *service = GYACHT_SERVICE (object);

  G_OBJECT_CLASS (gyacht_container_service_parent_class)->constructed (object);

  if (!gyacht_service_error_occur (service))
    {
      g_signal_connect (self,
                        "monitor-event-triggered",
                        G_CALLBACK (internal_load_contents),
                        NULL);

      internal_load_contents (self);
    }
}

static void
gyacht_container_service_class_init (GyachtContainerServiceClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GyachtServiceClass *service_class = GYACHT_SERVICE_CLASS (klass);

  object_class->finalize = gyacht_container_service_finalize;
  object_class->constructed = gyacht_container_service_constructed;

  service_class->clear_list = internal_clear_container_list;
  service_class->get_json_path = internal_get_json_path;
}

static void
gyacht_container_service_init (GyachtContainerService *self)
{
  self->containers = NULL;
  self->jobs = g_queue_new ();
}

/* --- Public APIs --- */
GyachtContainerService *
gyacht_container_service_new (GyachtRunLevel level)
{
  return g_object_new (GYACHT_TYPE_CONTAINER_SERVICE,
                       "run-level", level,
                       NULL);
}

GSequence *
gyacht_container_service_get_containers (GyachtContainerService *self)
{
  g_return_val_if_fail (GYACHT_IS_CONTAINER_SERVICE (self), NULL);

  return self->containers;
}
