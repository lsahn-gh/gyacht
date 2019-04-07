/* gyacht-container.c
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

#include "gyacht-container.h"
#include "gyacht-debug.h"
#include "gyacht-macros.h"

#include <glib/gi18n.h>

struct _GyachtContainer
{
  GObject       parent_instance;

  gchar         *id;        /* Container ID */
  gchar         *short_id;  /* It has the first 12 characters of the id */
  gchar         *name;
  GPtrArray     *names;
  gchar         *image;
  gchar         *image_name;
  gchar         *layer;
  gchar         *metadata;
  GDateTime     *created;
  GPtrArray     *uidmaps;
  GPtrArray     *gidmaps;
  GHashTable    *flags;
};

G_DEFINE_TYPE (GyachtContainer, gyacht_container, G_TYPE_OBJECT)

enum {
  PROP_ID = 1,
  PROP_NAMES,
  PROP_IMAGE,
  PROP_LAYER,
  PROP_METADATA,
  PROP_CREATED,
  PROP_UIDMAPS,
  PROP_GIDMAPS,
  PROP_FLAGS,
  N_PROPERTIES
};

static GParamSpec *properties [N_PROPERTIES] = { NULL };

/* Forward declarations */
static void gyacht_container_set_id         (GyachtContainer *, const gchar *);
static void gyacht_container_set_short_id   (GyachtContainer *, const gchar *);
static void gyacht_container_set_name       (GyachtContainer *, const gchar *);
static void gyacht_container_set_names      (GyachtContainer *, GPtrArray *);
static void gyacht_container_set_image      (GyachtContainer *, const gchar *);
static void gyacht_container_set_image_name (GyachtContainer *, const gchar *);
static void gyacht_container_set_layer      (GyachtContainer *, const gchar *);
static void gyacht_container_set_metadata   (GyachtContainer *, const gchar *);
static void gyacht_container_set_created    (GyachtContainer *, GDateTime *);
static void gyacht_container_set_uidmaps    (GyachtContainer *, GPtrArray *);
static void gyacht_container_set_gidmaps    (GyachtContainer *, GPtrArray *);
static void gyacht_container_set_flags      (GyachtContainer *, GHashTable *);


/* --- GObject --- */
static void
gyacht_container_finalize (GObject *object)
{
  GyachtContainer *self = GYACHT_CONTAINER (object);

  gyacht_trace ("%s is ready to be finalized", self->name);

  g_free (self->id);
  g_free (self->short_id);
  g_free (self->name);
  if (self->names)
    g_ptr_array_unref (self->names);
  g_free (self->image);
  g_free (self->image_name);
  g_free (self->layer);
  g_free (self->metadata);
  if (self->created)
    g_date_time_unref (self->created);
  if (self->uidmaps)
    g_ptr_array_unref (self->uidmaps);
  if (self->gidmaps)
    g_ptr_array_unref (self->gidmaps);
  if (self->flags)
    g_hash_table_unref (self->flags);

  G_OBJECT_CLASS (gyacht_container_parent_class)->finalize (object);
}

static void
gyacht_container_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  GyachtContainer *self = GYACHT_CONTAINER (object);

  switch (prop_id)
    {
    case PROP_ID:
      gyacht_container_set_id (self, g_value_get_string (value));
      break;

    case PROP_NAMES:
      gyacht_container_set_names (self, g_value_get_pointer (value));
      break;

    case PROP_IMAGE:
      gyacht_container_set_image (self, g_value_get_string (value));
      break;

    case PROP_LAYER:
      gyacht_container_set_layer (self, g_value_get_string (value));
      break;

    case PROP_METADATA:
      gyacht_container_set_metadata (self, g_value_get_string (value));
      break;

    case PROP_CREATED:
      gyacht_container_set_created (self, g_value_get_pointer (value));
      break;

    case PROP_UIDMAPS:
      gyacht_container_set_uidmaps (self, g_value_get_pointer (value));
      break;

    case PROP_GIDMAPS:
      gyacht_container_set_gidmaps (self, g_value_get_pointer (value));
      break;

    case PROP_FLAGS:
      gyacht_container_set_flags (self, g_value_get_pointer (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gyacht_container_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  switch (prop_id)
    {
    case PROP_ID:
    case PROP_NAMES:
    case PROP_IMAGE:
    case PROP_LAYER:
    case PROP_METADATA:
    case PROP_CREATED:
    case PROP_UIDMAPS:
    case PROP_GIDMAPS:
    case PROP_FLAGS:
      gyacht_warn ("Use getters instead of g_object_get_property()");
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gyacht_container_class_init (GyachtContainerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gyacht_container_finalize;
  object_class->set_property = gyacht_container_set_property;
  object_class->get_property = gyacht_container_get_property;

  properties [PROP_ID] =
    g_param_spec_string ("id",
                         "Id",
                         "Container id information",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  properties [PROP_NAMES] =
    g_param_spec_pointer ("names",
                          "Names",
                          "Container names information",
                          (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  properties [PROP_IMAGE] =
    g_param_spec_string ("image",
                         "Image",
                         "Container image information",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  properties [PROP_LAYER] =
    g_param_spec_string ("layer",
                         "Layer",
                         "Container layer information",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  properties [PROP_METADATA] =
    g_param_spec_string ("metadata",
                         "Metadata",
                         "Container metadata information",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  properties [PROP_CREATED] =
    g_param_spec_pointer ("created",
                          "Created",
                          "Container created time information",
                          (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  properties [PROP_UIDMAPS] =
    g_param_spec_pointer ("uidmaps",
                          "Uidmaps",
                          "Container uidmap information",
                          (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  properties [PROP_GIDMAPS] =
    g_param_spec_pointer ("gidmaps",
                          "Gidmaps",
                          "Container gidmap information",
                          (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  properties [PROP_FLAGS] =
    g_param_spec_pointer ("flags",
                          "Flags",
                          "Container flag information",
                          (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}

static void
gyacht_container_init (GyachtContainer *self)
{

}

/* --- Setters --- */
static void
gyacht_container_set_id (GyachtContainer *self,
                         const gchar     *new_id)
{
  g_return_if_fail (GYACHT_IS_CONTAINER (self));

  if (new_id)
    {
      g_free (self->id);
      self->id = g_strdup (new_id);
      gyacht_container_set_short_id (self, new_id);
    }
}

static void
gyacht_container_set_short_id (GyachtContainer *self,
                               const gchar     *new_short_id)
{
  g_return_if_fail (GYACHT_IS_CONTAINER (self));

  if (new_short_id)
    {
      g_free (self->short_id);
      self->short_id = g_strndup (new_short_id, 12);
    }
}

static void
gyacht_container_set_name (GyachtContainer *self,
                           const gchar     *new_name)
{
  g_return_if_fail (GYACHT_IS_CONTAINER (self));

  if (new_name)
    {
      g_free (self->name);
      self->name = g_strdup (new_name);
    }
}

static void
gyacht_container_set_names (GyachtContainer *self,
                            GPtrArray       *new_names)
{
  g_return_if_fail (GYACHT_IS_CONTAINER (self));

  if (new_names)
    {
      if (self->names)
        g_ptr_array_unref (self->names);
      self->names = new_names;
      gyacht_container_set_name (self, g_ptr_array_index (new_names, 0));
    }
}

static void
gyacht_container_set_image (GyachtContainer *self,
                            const gchar     *new_image)
{
  g_return_if_fail (GYACHT_IS_CONTAINER (self));

  if (new_image)
    {
      g_free (self->image);
      self->image = g_strdup (new_image);
    }
}

static void
gyacht_container_set_image_name (GyachtContainer *self,
                                 const gchar     *metadata)
{
  g_return_if_fail (GYACHT_IS_CONTAINER (self));

  if (metadata)
    {
      char *cur = strstr (metadata, "image-name");
      if (cur)
        {
          gchar *new_image_name;
          gchar *start, *end;
          ptrdiff_t diff = 0;

          while (*cur != ':') ++cur;
          while (*cur != '"') ++cur;

          start = ++cur;

          while (*cur != '"') ++cur;

          end = cur;

          diff = end - start;

          new_image_name = g_strndup (start, diff);

          g_free (self->image_name);
          self->image_name = new_image_name;
        }
    }
}

static void
gyacht_container_set_layer (GyachtContainer *self,
                            const gchar     *new_layer)
{
  g_return_if_fail (GYACHT_IS_CONTAINER (self));

  if (new_layer)
    {
      g_free (self->layer);
      self->layer = g_strdup (new_layer);
    }
}


static void
gyacht_container_set_metadata (GyachtContainer *self,
                               const gchar     *new_metadata)
{
  g_return_if_fail (GYACHT_IS_CONTAINER (self));

  if (new_metadata)
    {
      g_free (self->metadata);
      self->metadata = g_strdup (new_metadata);
      gyacht_container_set_image_name (self, new_metadata);
    }
}

static void
gyacht_container_set_created (GyachtContainer *self,
                              GDateTime       *new_created)
{
  g_return_if_fail (GYACHT_IS_CONTAINER (self));

  if (new_created)
    {
      if (self->created)
        g_date_time_unref (self->created);
      self->created = new_created;
    }
}

static void
gyacht_container_set_uidmaps (GyachtContainer *self,
                              GPtrArray       *new_uidmaps)
{
  g_return_if_fail (GYACHT_IS_CONTAINER (self));

  if (new_uidmaps)
    {
      if (self->uidmaps)
        g_ptr_array_unref (self->uidmaps);
      self->uidmaps = new_uidmaps;
    }
}

static void
gyacht_container_set_gidmaps (GyachtContainer *self,
                              GPtrArray       *new_gidmaps)
{
  g_return_if_fail (GYACHT_IS_CONTAINER (self));

  if (new_gidmaps)
    {
      if (self->gidmaps)
        g_ptr_array_unref (self->gidmaps);
      self->gidmaps = new_gidmaps;
    }
}

static void
gyacht_container_set_flags (GyachtContainer *self,
                            GHashTable      *new_flags)
{
  g_return_if_fail (GYACHT_IS_CONTAINER (self));

  if (new_flags)
    {
      if (self->flags)
        g_hash_table_unref (self->flags);
      self->flags = new_flags;
    }
}

/* --- Public APIs --- */
GyachtContainer *
gyacht_container_new (const gchar      *id,
                      const GPtrArray  *names,
                      const gchar      *image,
                      const gchar      *layer,
                      const gchar      *metadata,
                      const GDateTime  *created,
                      const GPtrArray  *uidmaps,
                      const GPtrArray  *gidmaps,
                      const GHashTable *flags)
{
  g_return_val_if_fail (id != NULL, NULL);

  return g_object_new (GYACHT_TYPE_CONTAINER,
                       "id", id,
                       "names", names,
                       "image", image,
                       "layer", layer,
                       "metadata", metadata,
                       "created", created,
                       "uidmaps", uidmaps,
                       "gidmaps", gidmaps,
                       "flags", flags,
                       NULL);
}

/* --- Getters --- */
const gchar *
gyacht_container_get_id (GyachtContainer *self)
{
  g_return_val_if_fail (GYACHT_IS_CONTAINER (self), NULL);

  return self->id;
}

const gchar *
gyacht_container_get_short_id (GyachtContainer *self)
{
  g_return_val_if_fail (GYACHT_IS_CONTAINER (self), NULL);

  return self->short_id;
}

const gchar *
gyacht_container_get_name (GyachtContainer *self)
{
  g_return_val_if_fail (GYACHT_IS_CONTAINER (self), NULL);

  return self->name;
}

const GPtrArray *
gyacht_container_get_names (GyachtContainer *self)
{
  g_return_val_if_fail (GYACHT_IS_CONTAINER (self), NULL);

  return self->names;
}

const gchar *
gyacht_container_get_image (GyachtContainer *self)
{
  g_return_val_if_fail (GYACHT_IS_CONTAINER (self), NULL);

  return self->image;
}

const gchar *
gyacht_container_get_image_name (GyachtContainer *self)
{
  g_return_val_if_fail (GYACHT_IS_CONTAINER (self), NULL);

  return self->image_name;
}

const gchar *
gyacht_container_get_layer (GyachtContainer *self)
{
  g_return_val_if_fail (GYACHT_IS_CONTAINER (self), NULL);

  return self->layer;
}

const gchar *
gyacht_container_get_metadata (GyachtContainer *self)
{
  g_return_val_if_fail (GYACHT_IS_CONTAINER (self), NULL);

  return self->metadata;
}

const GDateTime *
gyacht_container_get_created (GyachtContainer *self)
{
  g_return_val_if_fail (GYACHT_IS_CONTAINER (self), NULL);

  return self->created;
}

gchar *
gyacht_container_get_calendar_date (GyachtContainer *self)
{
  GDateTime *date = NULL;
  gchar *formatted_date = NULL;

  g_return_val_if_fail (GYACHT_IS_CONTAINER (self), NULL);

  date = self->created;

  if (date)
    formatted_date = g_date_time_format (date, _("%B %e, %Y"));

  return formatted_date;
}

const GPtrArray *
gyacht_container_get_uidmaps (GyachtContainer *self)
{
  g_return_val_if_fail (GYACHT_IS_CONTAINER (self), NULL);

  return self->uidmaps;
}

const GPtrArray *
gyacht_container_get_gidmaps (GyachtContainer *self)
{
  g_return_val_if_fail (GYACHT_IS_CONTAINER (self), NULL);

  return self->gidmaps;
}

const GHashTable *
gyacht_container_get_flags (GyachtContainer *self)
{
  g_return_val_if_fail (GYACHT_IS_CONTAINER (self), NULL);

  return self->flags;
}
