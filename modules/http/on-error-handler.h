/*
 * Copyright (c) 2019 Balabit
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */

#ifndef ON_ERROR_HANDLER_H_INCLUDED
#define ON_ERROR_HANDLER_H_INCLUDED 1

#include "syslog-ng.h"

typedef enum
{
  ON_ERROR_SUCCESS,
  ON_ERROR_RETRY,
  ON_ERROR_DROP,
  ON_ERROR_DISCONNECT,
  ON_ERROR_MAX
} OnErrorResult;

typedef OnErrorResult (*OnErrorAction)(gpointer user_data);
OnErrorResult on_error_success(gpointer user_data);
OnErrorResult on_error_retry(gpointer user_data);
OnErrorResult on_error_drop(gpointer user_data);
OnErrorResult on_error_disconnect(gpointer user_data);

typedef struct
{
  gint status_code;
  gchar *match_string;
  OnErrorAction action;
  gpointer user_data;
} OnErrorParams;

typedef GHashTable OnErrorHandlers;

OnErrorHandlers *on_error_handlers_new(void);
void on_error_handlers_free(OnErrorHandlers *self);
void on_error_handlers_insert(OnErrorHandlers *self, OnErrorParams *params);
OnErrorParams *on_error_handlers_lookup(OnErrorHandlers *self, glong status_code, const gchar *data, gsize len);
gboolean on_error_handlers_is_used(OnErrorHandlers *self);

#endif
