/* gyacht-service-private.h
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

#include "gyacht-service.h"

G_BEGIN_DECLS

GyachtRunLevel  gyacht_service_get_run_level    (GyachtService *self);
gboolean        gyacht_service_error_occur      (GyachtService *self);
void            gyacht_service_load_json_async  (GyachtService       *self,
                                                 GCancellable        *cancellable,
                                                 GAsyncReadyCallback  callback,
                                                 gpointer             user_data);
JsonParser *    gyacht_service_load_json_finish (GyachtService  *self,
                                                 GAsyncResult   *res,
                                                 GError        **error);

G_END_DECLS
