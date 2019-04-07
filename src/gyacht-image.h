/* gyacht-image.h
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

#define GYACHT_TYPE_IMAGE (gyacht_image_get_type())

G_DECLARE_FINAL_TYPE (GyachtImage, gyacht_image, GYACHT, IMAGE, GObject)

GyachtImage *     gyacht_image_new                (const gchar     *id,
                                                   const gchar     *digest,
                                                   const GPtrArray *names,
                                                   const gchar     *layer,
                                                   const gchar     *metadata,
                                                   const GDateTime *created);
const gchar *     gyacht_image_get_id             (GyachtImage *self);
const gchar *     gyacht_image_get_short_id       (GyachtImage *self);
const gchar *     gyacht_image_get_digest         (GyachtImage *self);
const gchar *     gyacht_image_get_name           (GyachtImage *self);
const GPtrArray * gyacht_image_get_names          (GyachtImage *self);
const gchar *     gyacht_image_get_layer          (GyachtImage *self);
const gchar *     gyacht_image_get_metadata       (GyachtImage *self);
const GDateTime * gyacht_image_get_created        (GyachtImage *self);
gchar *           gyacht_image_get_calendar_date  (GyachtImage *self);

G_END_DECLS
