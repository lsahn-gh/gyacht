/* main.c
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

#include <glib/gi18n.h>
#define HANDY_USE_UNSTABLE_API
#include <handy.h>

int
main (int   argc,
      char *argv[])
{
  g_autoptr(GyachtApplication) app = NULL;
  int ret;

  /* Set up gettext translations */
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  hdy_init (&argc, &argv);

  app = gyacht_application_new ();
  g_application_set_default (G_APPLICATION (app));

  ret = g_application_run (G_APPLICATION (app), argc, argv);

  return ret;
}
