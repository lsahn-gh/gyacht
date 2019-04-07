/* gyacht-window.h
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

#include <gtk/gtk.h>

#include "gyacht-application.h"

G_BEGIN_DECLS

#define GYACHT_TYPE_WINDOW (gyacht_window_get_type())

G_DECLARE_FINAL_TYPE (GyachtWindow, gyacht_window, GYACHT, WINDOW, GtkApplicationWindow)

GtkWidget *     gyacht_window_new     (GyachtApplication *application);

G_END_DECLS
