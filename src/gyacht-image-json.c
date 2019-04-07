/* gyacht-image-json.c
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
#include "gyacht-image-private.h"

#define GYACHT_IMAGE_JSON_ERROR (gyacht_image_json_error_quark())

static GQuark
gyacht_image_json_error_quark (void)
{
  return g_quark_from_static_string ("gyacht-image-json-error-quark");
}

static GPtrArray *
internal_names_new (JsonArray *jarray)
{
  GPtrArray *new_array;
  guint length;
  guint i;

  length = json_array_get_length (jarray);
  new_array = g_ptr_array_new_full (length, g_free);

  for (i = 0; i < length; i++)
    {
      const gchar *name = json_array_get_string_element (jarray, i);
      g_ptr_array_insert (new_array, i, g_strdup (name));
    }

  return new_array;
}

static void
internal_image_json_foreach_cb (JsonArray *array,
                                guint      index_,
                                JsonNode  *element_node,
                                gpointer   user_data)
{
  GSequence *seq;
  JsonObject *elem;
  GyachtImage *new_image;
  /* Image data */
  const gchar *id = NULL;
  const gchar *digest = NULL;
  GPtrArray *names = NULL;
  const gchar *layer = NULL;
  const gchar *metadata = NULL;
  GDateTime *created = NULL;

  seq = (GSequence *)user_data;
  elem = json_node_get_object (element_node);

  if (json_object_has_member (elem, "id"))
    {
      id = json_object_get_string_member (elem, "id");
    }
  if (json_object_has_member (elem, "digest"))
    {
      digest = json_object_get_string_member (elem, "digest");
    }
  if (json_object_has_member (elem, "names"))
    {
      JsonArray *member = json_object_get_array_member (elem, "names");
      names = internal_names_new (member);
    }
  if (json_object_has_member (elem, "layer"))
    {
      layer = json_object_get_string_member (elem, "layer");
    }
  if (json_object_has_member (elem, "metadata"))
    {
      metadata = json_object_get_string_member (elem, "metadata");
    }
  if (json_object_has_member (elem, "created"))
    {
      g_autoptr(GTimeZone) time_zone = g_time_zone_new_local ();
      const gchar *member = json_object_get_string_member (elem, "created");
      created = g_date_time_new_from_iso8601 (member, time_zone);
    }

  new_image = gyacht_image_new (id, digest, names,
                                layer, metadata, created);
  g_sequence_append (seq, new_image);
}

/**
 * gyacht_image_parse_json_contents:
 * @parser: #JsonParser in which has json contents.
 * @error: (nullable): A #GError.
 *
 * Return value: (transfer full): Null if it is on failure and error is set,
 *    otherwise returns #GSequence and error is NULL.
 */
GSequence *
gyacht_image_parse_json_contents (JsonParser  *parser,
                                  GError     **error)
{
  JsonNode *root;
  JsonArray *array;
  GSequence *seq;

  GYACHT_TRACE_ENTRY;

  g_return_val_if_fail (JSON_IS_PARSER (parser), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  root = json_parser_get_root (parser);
  if (root == NULL)
    {
      if (error != NULL)
        g_set_error (error, GYACHT_IMAGE_JSON_ERROR, 0,
                     "Failed to get a root from the parser");
      return NULL;
    }

  array = json_node_get_array (root);
  seq = g_sequence_new (g_object_unref);
  json_array_foreach_element (array, internal_image_json_foreach_cb, seq);
  /* Should we sort the sequence? */

  GYACHT_TRACE_EXIT;

  return seq;
}
