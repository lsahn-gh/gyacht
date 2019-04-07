/* gyacht-image-service.h
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

#include "gyacht-macros.h"
#include "gyacht-service-private.h"

G_BEGIN_DECLS

#define GYACHT_TYPE_IMAGE_SERVICE (gyacht_image_service_get_type())

G_DECLARE_FINAL_TYPE (GyachtImageService, gyacht_image_service, GYACHT, IMAGE_SERVICE, GyachtService)

GyachtImageService *  gyacht_image_service_new        (GyachtRunLevel level);
GSequence *           gyacht_image_service_get_images (GyachtImageService *self);

G_END_DECLS
