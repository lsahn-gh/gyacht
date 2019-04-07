/* gyacht-debug.h
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

#include <string.h>
#include <glib.h>

G_BEGIN_DECLS

#define NAME_SPACE_LENGTH  7
#define __FILE_NAME__ \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + (1+NAME_SPACE_LENGTH) : __FILE__)

#define GYACHT_TRACE_ENTRY \
  g_debug("ENTRY: [%s:%d] %s()", __FILE_NAME__, __LINE__, G_STRFUNC)

#define GYACHT_TRACE_EXIT \
  g_debug(" EXIT: [%s:%d] %s()", __FILE_NAME__, __LINE__, G_STRFUNC)

#define gyacht_trace(fmt, ...) \
  g_debug("  MSG: [%s:%d] %s() :: " fmt, __FILE_NAME__, __LINE__, G_STRFUNC, ##__VA_ARGS__)

#define gyacht_info(fmt, ...) \
  g_info ("  MSG: [%s:%d] %s() :: " fmt, __FILE_NAME__, __LINE__, G_STRFUNC, ##__VA_ARGS__)

#define gyacht_debug(fmt, ...) \
  g_debug("  MSG: [%s:%d] %s() :: " fmt, __FILE_NAME__, __LINE__, G_STRFUNC, ##__VA_ARGS__)

#define gyacht_error(fmt, ...) \
  g_error("  MSG: [%s:%d] %s() :: " fmt, __FILE_NAME__, __LINE__, G_STRFUNC, ##__VA_ARGS__)

#define gyacht_warn(fmt, ...) \
  g_warning ("  MSG: [%s:%d] %s() :: " fmt, __FILE_NAME__, __LINE__, G_STRFUNC, ##__VA_ARGS__)

#define gyacht_critical(fmt, ...) \
  g_critical("  MSG: [%s:%d] %s() :: " fmt, __FILE_NAME__, __LINE__, G_STRFUNC, ##__VA_ARGS__)

G_END_DECLS
