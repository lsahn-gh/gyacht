/* gyacht-container-json.c
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
#include "gyacht-debug.h"

#define GYACHT_CONTAINER_JSON_ERROR (gyacht_container_json_error_quark())

static GQuark
gyacht_container_json_error_quark (void)
{
  return g_quark_from_static_string ("gyacht-container-json-error-quark");
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

static GPtrArray *
internal_uidmap_array_new (JsonArray *jarray)
{
  GPtrArray *new_array;
  guint length;
  guint i;

  length = json_array_get_length (jarray);
  new_array = g_ptr_array_new_full (length, g_free);

  for (i = 0; i < length; i++)
    {
      JsonObject *item = json_array_get_object_element (jarray, i);
      Uidmap *new_uidmap = g_new0 (Uidmap, 1);

      new_uidmap->container_id = json_object_get_int_member (item, "container_id");
      new_uidmap->host_id = json_object_get_int_member (item, "host_id");
      new_uidmap->size = json_object_get_int_member (item, "size");

      g_ptr_array_insert (new_array, i, new_uidmap);
    }

  return new_array;
}

static GPtrArray *
internal_gidmap_array_new (JsonArray *jarray)
{
  return internal_uidmap_array_new (jarray);
}

static GHashTable *
internal_flags_new (JsonObject *jobj)
{
  GHashTable *new_ht;
  JsonObjectIter iter;
  const gchar *member_name;
  JsonNode *member_node;

  new_ht = g_hash_table_new_full (g_str_hash,
                                  g_str_equal,
                                  g_free,
                                  g_free);

  json_object_iter_init (&iter, jobj);
  while (json_object_iter_next (&iter, &member_name, &member_node))
    {
      gchar *key = g_strdup (member_name);
      gchar *value = json_node_dup_string (member_node);

      g_hash_table_insert (new_ht, key, value);
    }

  return new_ht;
}

static void
internal_container_json_foreach_cb (JsonArray *array,
                                    guint      index_,
                                    JsonNode  *element_node,
                                    gpointer   user_data)
{
  GSequence *seq;
  JsonObject *elem;
  GyachtContainer *new_container;
  /* container data */
  const gchar *id = NULL;
  GPtrArray *names = NULL;
  const gchar *image = NULL;
  const gchar *layer = NULL;
  const gchar *metadata = NULL;
  GDateTime *created = NULL;
  GPtrArray *uidmap = NULL;
  GPtrArray *gidmap = NULL;
  GHashTable *flags = NULL;

  seq = (GSequence *)user_data;
  elem = json_node_get_object (element_node);

  if (json_object_has_member (elem, "id"))
    {
      id = json_object_get_string_member (elem, "id");
    }
  if (json_object_has_member (elem, "names"))
    {
      JsonArray *member = json_object_get_array_member (elem, "names");
      names = internal_names_new (member);
    }
  if (json_object_has_member (elem, "image"))
    {
      image = json_object_get_string_member (elem, "image");
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
  if (json_object_has_member (elem, "uidmap"))
    {
      JsonArray *member = json_object_get_array_member (elem, "uidmap");
      uidmap = internal_uidmap_array_new (member);
    }
  if (json_object_has_member (elem, "gidmap"))
    {
      JsonArray *member = json_object_get_array_member (elem, "gidmap");
      gidmap = internal_gidmap_array_new (member);
    }
  if (json_object_has_member (elem, "flags"))
    {
      JsonObject *member = json_object_get_object_member (elem, "flags");
      flags = internal_flags_new (member);
    }

  new_container = gyacht_container_new (id, names, image,
                                        layer, metadata, created,
                                        uidmap, gidmap, flags);
  g_sequence_append (seq, new_container);
}

/**
 * gyacht_container_parse_json_contents:
 * @parser: #JsonParser in which has json contents.
 * @error: (nullable): A #GError.
 *
 * Return value: (transfer full): Null if it is on failure and error is set,
 *    otherwise returns #GSequence and error is NULL.
 */
GSequence *
gyacht_container_parse_json_contents (JsonParser  *parser,
                                      GError     **error)
{
  JsonNode *root;
  JsonArray *array;
  GSequence *seq;

  GYACHT_TRACE_ENTRY;

  g_return_val_if_fail (JSON_IS_PARSER (parser), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  /* json-glib tutorial reference,
   * https://kt48.wordpress.com/2015/03/03/basic-tutorial-on-json-glib-for-tizen/
   */
  root = json_parser_get_root (parser);
  if (root == NULL)
    {
      if (error != NULL)
        g_set_error (error, GYACHT_CONTAINER_JSON_ERROR, 0,
                     "Failed to get a root from the parser");
      return NULL;
    }

  array = json_node_get_array (root);
  seq = g_sequence_new (g_object_unref);
  json_array_foreach_element (array, internal_container_json_foreach_cb, seq);
  /* Should we sort the sequence? */

  GYACHT_TRACE_EXIT;

  return seq;
}
