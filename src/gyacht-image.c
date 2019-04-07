/* gyacht-image.c
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
#include "gyacht-image.h"
#include "gyacht-macros.h"

#include <glib/gi18n.h>

struct _GyachtImage
{
  GObject     parent_instance;

  gchar       *id;
  gchar       *short_id;
  gchar       *digest;
  gchar       *name;
  GPtrArray   *names;
  gchar       *layer;
  gchar       *metadata;
  GDateTime   *created;

#if 0
  /* Should we implement the following data? */
  gpointer    big_data_names;
  gpointer    big_data_sizes;
  gpointer    big_data_digests;
#endif
};

G_DEFINE_TYPE (GyachtImage, gyacht_image, G_TYPE_OBJECT)

enum {
  PROP_ID = 1,
  PROP_DIGEST,
  PROP_NAMES,
  PROP_LAYER,
  PROP_METADATA,
  PROP_CREATED,
  N_PROPERTIES
};

static GParamSpec *properties [N_PROPERTIES] = { NULL };

/* Forward declarations */
static void gyacht_image_set_id       (GyachtImage *, const gchar *);
static void gyacht_image_set_short_id (GyachtImage *, const gchar *);
static void gyacht_image_set_digest   (GyachtImage *, const gchar *);
static void gyacht_image_set_name     (GyachtImage *, const gchar *);
static void gyacht_image_set_names    (GyachtImage *, GPtrArray *);
static void gyacht_image_set_layer    (GyachtImage *, const gchar *);
static void gyacht_image_set_metadata (GyachtImage *, const gchar *);
static void gyacht_image_set_created  (GyachtImage *, GDateTime *);


/* --- GObject --- */
static void
gyacht_image_finalize (GObject *object)
{
  GyachtImage *self = GYACHT_IMAGE (object);

  gyacht_trace ("%s is ready to be finalized", self->name);

  g_free (self->id);
  g_free (self->short_id);
  g_free (self->digest);
  g_free (self->name);
  if (self->names)
    g_ptr_array_unref (self->names);
  g_free (self->layer);
  g_free (self->metadata);
  if (self->created)
    g_date_time_unref (self->created);

  G_OBJECT_CLASS (gyacht_image_parent_class)->finalize (object);
}

static void
gyacht_image_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  GyachtImage *self = GYACHT_IMAGE (object);

  switch (prop_id)
    {
    case PROP_ID:
      gyacht_image_set_id (self, g_value_get_string (value));
      break;

    case PROP_DIGEST:
      gyacht_image_set_digest (self, g_value_get_string (value));
      break;

    case PROP_NAMES:
      gyacht_image_set_names (self, g_value_get_pointer (value));
      break;

    case PROP_LAYER:
      gyacht_image_set_layer (self, g_value_get_string (value));
      break;

    case PROP_METADATA:
      gyacht_image_set_metadata (self, g_value_get_string (value));
      break;

    case PROP_CREATED:
      gyacht_image_set_created (self, g_value_get_pointer (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gyacht_image_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  switch (prop_id)
    {
    case PROP_ID:
    case PROP_DIGEST:
    case PROP_NAMES:
    case PROP_LAYER:
    case PROP_METADATA:
    case PROP_CREATED:
      gyacht_warn ("Use getters instead of g_object_get_property()");
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gyacht_image_class_init (GyachtImageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gyacht_image_finalize;
  object_class->set_property = gyacht_image_set_property;
  object_class->get_property = gyacht_image_get_property;

  properties [PROP_ID] =
    g_param_spec_string ("id",
                         "Id",
                         "Image id information",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  properties [PROP_DIGEST] =
    g_param_spec_string ("digest",
                         "Digest",
                         "Image digest information",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  properties [PROP_NAMES] =
    g_param_spec_pointer ("names",
                          "Names",
                          "Image names information",
                          (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  properties [PROP_LAYER] =
    g_param_spec_string ("layer",
                         "Layer",
                         "Image layer information",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  properties [PROP_METADATA] =
    g_param_spec_string ("metadata",
                         "Metadata",
                         "Image metadata information",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  properties [PROP_CREATED] =
    g_param_spec_pointer ("created",
                          "Created",
                          "Image created date information",
                          (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_properties (object_class, N_PROPERTIES, properties);
}

static void
gyacht_image_init (GyachtImage *self)
{

}

/* --- Setters --- */
static void
gyacht_image_set_id (GyachtImage *self,
                     const gchar *new_id)
{
  g_return_if_fail (GYACHT_IS_IMAGE (self));

  if (new_id)
    {
      g_free (self->id);
      self->id = g_strdup (new_id);
      gyacht_image_set_short_id (self, new_id);
    }
}

static void
gyacht_image_set_short_id (GyachtImage *self,
                           const gchar *new_short_id)
{
  g_return_if_fail (GYACHT_IS_IMAGE (self));

  if (new_short_id)
    {
      g_free (self->short_id);
      self->short_id = g_strndup (new_short_id, 12);
    }
}

static void
gyacht_image_set_digest (GyachtImage *self,
                         const gchar *new_digest)
{
  g_return_if_fail (GYACHT_IS_IMAGE (self));

  if (new_digest)
    {
      g_free (self->digest);
      self->digest = g_strdup (new_digest);
    }
}

static void
gyacht_image_set_name (GyachtImage *self,
                       const gchar *new_name)
{
  g_return_if_fail (GYACHT_IS_IMAGE (self));

  if (new_name)
    {
      g_free (self->name);
      self->name = g_strdup (new_name);
    }
}

static void
gyacht_image_set_names (GyachtImage *self,
                        GPtrArray   *new_names)
{
  g_return_if_fail (GYACHT_IS_IMAGE (self));

  if (new_names)
    {
      if (self->names)
        g_ptr_array_unref (self->names);
      self->names = new_names;
      gyacht_image_set_name (self, g_ptr_array_index (new_names, 0));
    }
}

static void
gyacht_image_set_layer (GyachtImage *self,
                        const gchar *new_layer)
{
  g_return_if_fail (GYACHT_IS_IMAGE (self));

  if (new_layer)
    {
      g_free (self->layer);
      self->layer = g_strdup (new_layer);
    }
}

static void
gyacht_image_set_metadata (GyachtImage *self,
                           const gchar *new_metadata)
{
  g_return_if_fail (GYACHT_IS_IMAGE (self));

  if (new_metadata)
    {
      g_free (self->metadata);
      self->metadata = g_strdup (new_metadata);
    }
}

static void
gyacht_image_set_created (GyachtImage *self,
                          GDateTime   *new_created)
{
  g_return_if_fail (GYACHT_IS_IMAGE (self));

  if (new_created)
    {
      if (self->created)
        g_date_time_unref (self->created);
      self->created = new_created;
    }
}

/* --- Public APIs --- */
GyachtImage *
gyacht_image_new (const gchar     *id,
                  const gchar     *digest,
                  const GPtrArray *names,
                  const gchar     *layer,
                  const gchar     *metadata,
                  const GDateTime *created)
{
  g_return_val_if_fail (id != NULL, NULL);

  return g_object_new (GYACHT_TYPE_IMAGE,
                       "id", id,
                       "digest", digest,
                       "names", names,
                       "layer", layer,
                       "metadata", metadata,
                       "created", created,
                       NULL);
}

/* --- Getters --- */
const gchar *
gyacht_image_get_id (GyachtImage *self)
{
  g_return_val_if_fail (GYACHT_IS_IMAGE (self), NULL);

  return self->id;
}

const gchar *
gyacht_image_get_short_id (GyachtImage *self)
{
  g_return_val_if_fail (GYACHT_IS_IMAGE (self), NULL);

  return self->short_id;
}

const gchar *
gyacht_image_get_digest (GyachtImage *self)
{
  g_return_val_if_fail (GYACHT_IS_IMAGE (self), NULL);

  return self->digest;
}

const gchar *
gyacht_image_get_name (GyachtImage *self)
{
  g_return_val_if_fail (GYACHT_IS_IMAGE (self), NULL);

  return self->name;
}

const GPtrArray *
gyacht_image_get_names (GyachtImage *self)
{
  g_return_val_if_fail (GYACHT_IS_IMAGE (self), NULL);

  return self->names;
}

const gchar *
gyacht_image_get_layer (GyachtImage *self)
{
  g_return_val_if_fail (GYACHT_IS_IMAGE (self), NULL);

  return self->layer;
}

const gchar *
gyacht_image_get_metadata (GyachtImage *self)
{
  g_return_val_if_fail (GYACHT_IS_IMAGE (self), NULL);

  return self->metadata;
}

const GDateTime *
gyacht_image_get_created (GyachtImage *self)
{
  g_return_val_if_fail (GYACHT_IS_IMAGE (self), NULL);

  return self->created;
}

gchar *
gyacht_image_get_calendar_date (GyachtImage *self)
{
  GDateTime *date = NULL;
  gchar *formatted_date = NULL;

  g_return_val_if_fail (GYACHT_IS_IMAGE (self), NULL);

  date = self->created;

  if (date)
    formatted_date = g_date_time_format (date, _("%B %e, %Y"));

  return formatted_date;
}
