/* gyacht-file-utils.c
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

#include "gyacht-file-utils.h"

#define GYACHT_FILE_UTILS_ERROR (gyacht_file_utils_error_quark())

static GQuark
gyacht_file_utils_error_quark (void)
{
  return g_quark_from_static_string ("gyacht-container-list-error-quark");
}

gboolean
gyacht_file_utils_file_exists (GFile   *file,
                               GError **error)
{
  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (!g_file_query_exists (file, NULL))
    {
      if (error != NULL)
        g_set_error (error, GYACHT_FILE_UTILS_ERROR, 0,
                     "%s: No such file or directory", g_file_get_path (file));
      return FALSE;
    }

  return TRUE;
}
