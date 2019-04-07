/* gyacht-container.h
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

#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define GYACHT_TYPE_CONTAINER (gyacht_container_get_type())

G_DECLARE_FINAL_TYPE (GyachtContainer, gyacht_container, GYACHT, CONTAINER, GObject)

typedef struct
{
  gint64  container_id;
  gint64  host_id;
  gint64  size;
} Uidmap;

typedef struct
{
  gint64  container_id;
  gint64  host_id;
  gint64  size;
} Gidmap;

GyachtContainer *   gyacht_container_new                (const gchar      *id,
                                                         const GPtrArray  *names,
                                                         const gchar      *image,
                                                         const gchar      *layer,
                                                         const gchar      *metadata,
                                                         const GDateTime  *created,
                                                         const GPtrArray  *uidmaps,
                                                         const GPtrArray  *gidmaps,
                                                         const GHashTable *flags);
const gchar *       gyacht_container_get_id             (GyachtContainer *self);
const gchar *       gyacht_container_get_short_id       (GyachtContainer *self);
const gchar *       gyacht_container_get_name           (GyachtContainer *self);
const GPtrArray *   gyacht_container_get_names          (GyachtContainer *self);
const gchar *       gyacht_container_get_image          (GyachtContainer *self);
const gchar *       gyacht_container_get_image_name     (GyachtContainer *self);
const gchar *       gyacht_container_get_layer          (GyachtContainer *self);
const gchar *       gyacht_container_get_metadata       (GyachtContainer *self);
const GDateTime *   gyacht_container_get_created        (GyachtContainer *self);
gchar *             gyacht_container_get_calendar_date  (GyachtContainer *self);
const GPtrArray *   gyacht_container_get_uidmaps        (GyachtContainer *self);
const GPtrArray *   gyacht_container_get_gidmaps        (GyachtContainer *self);
const GHashTable *  gyacht_container_get_flags          (GyachtContainer *self);

G_END_DECLS
