/* gyacht-path-manager.c
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
#include "gyacht-path-manager.h"

#define USER_OVERLAY_CONTAINERS   "containers/storage/overlay-containers"
#define USER_OVERLAY_IMAGES       "containers/storage/overlay-images"

static gchar *
internal_build_container_filename (void)
{
  return g_build_filename (g_get_home_dir (),
                           ".local",
                           "share",
                           USER_OVERLAY_CONTAINERS,
                           NULL);
}

static gchar *
internal_build_image_filename (void)
{
  return g_build_filename (g_get_home_dir (),
                           ".local",
                           "share",
                           USER_OVERLAY_IMAGES,
                           NULL);
}

/* --- Public APIs --- */
gchar *
gyacht_dup_user_containers_dir (void)
{
  return internal_build_container_filename ();
}

gchar *
gyacht_dup_user_images_dir (void)
{
  return internal_build_image_filename ();
}
