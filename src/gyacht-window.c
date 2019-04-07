/* gyacht-window.c
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

#include "gyacht-config.h"
#include "gyacht-container-list-view.h"
#include "gyacht-debug.h"
#include "gyacht-image-list-view.h"
#include "gyacht-window.h"
#include "gyacht-macros.h"

struct _GyachtWindow
{
  GtkApplicationWindow    parent_instance;
};

G_DEFINE_TYPE (GyachtWindow, gyacht_window, GTK_TYPE_APPLICATION_WINDOW)


/* --- GObject --- */
static void
gyacht_window_class_init (GyachtWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  /* Must calls before binding templates */
  g_type_ensure (GYACHT_TYPE_CONTAINER_LIST_VIEW);
  g_type_ensure (GYACHT_TYPE_IMAGE_LIST_VIEW);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               GYACHT_UI_PREFIX "gyacht-window.ui");
}

static void
gyacht_window_init (GyachtWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkWidget *
gyacht_window_new (GyachtApplication *application)
{
  return g_object_new (GYACHT_TYPE_WINDOW,
                       "application", application,
                       NULL);
}
