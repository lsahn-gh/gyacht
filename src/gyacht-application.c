/* gyacht-application.c
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

#include "gyacht-application.h"
#include "gyacht-debug.h"
#include "gyacht-macros.h"
#include "gyacht-window.h"

#include <glib/gi18n.h>

struct _GyachtApplication
{
  GtkApplication  parent_instance;

  GtkWidget       *window;
};

G_DEFINE_TYPE (GyachtApplication, gyacht_application, GTK_TYPE_APPLICATION)

/* Forward declarations */
static void
internal_application_show_about (GSimpleAction *, GVariant *, gpointer);

static const GActionEntry gyacht_application_entries[] = {
    { "about", internal_application_show_about }
};


static void
internal_application_show_about (GSimpleAction *simple,
                                 GVariant      *parameter,
                                 gpointer       user_data)
{
  GyachtApplication *self = GYACHT_APPLICATION (user_data);

  static const gchar *authors[] = {
    "Yi-Soo An <yisooan@fedoraproject.org>",
    NULL
  };

  gtk_show_about_dialog (GTK_WINDOW (self->window),
                         "program-name", "Gyacht",
                         "version", PACKAGE_VERSION,
                         "copyright", _("Copyright \xC2\xA9 2019 Yi-Soo An"),
                         "license-type", GTK_LICENSE_GPL_3_0,
                         "authors", authors,
                         "artists", NULL,
                         "logo-icon-name", GYACHT_APPLICATION_ID,
                         "translator-credits", _("translator-credits"),
                         NULL);
}

/* --- GObject --- */
static void
gyacht_application_startup (GApplication *application)
{
  GyachtApplication *self = GYACHT_APPLICATION (application);

  /* Actions */
  g_action_map_add_action_entries (G_ACTION_MAP (self),
                                   gyacht_application_entries,
                                   G_N_ELEMENTS (gyacht_application_entries),
                                   self);

  G_APPLICATION_CLASS (gyacht_application_parent_class)->startup (application);

  self->window = gyacht_window_new (self);
}

static void
gyacht_application_activate (GApplication *application)
{
  GyachtApplication *self = GYACHT_APPLICATION (application);

  G_APPLICATION_CLASS (gyacht_application_parent_class)->activate (application);

  gtk_window_present (GTK_WINDOW (self->window));
}

static void
gyacht_application_class_init (GyachtApplicationClass *klass)
{
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

  application_class->startup = gyacht_application_startup;
  application_class->activate = gyacht_application_activate;
}

static void
gyacht_application_init (GyachtApplication *self)
{
}

GyachtApplication *
gyacht_application_new (void)
{
  return g_object_new (GYACHT_TYPE_APPLICATION,
                       "application-id", GYACHT_APPLICATION_ID,
                       "flags", G_APPLICATION_FLAGS_NONE,
                       NULL);
}
