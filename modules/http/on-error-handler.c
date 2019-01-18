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

#include "on-error-handler.h"

OnErrorResult
on_error_success(gpointer user_data)
{
  return ON_ERROR_SUCCESS;
}

OnErrorResult
on_error_retry(gpointer user_data)
{
  return ON_ERROR_RETRY;
}

OnErrorResult
on_error_drop(gpointer user_data)
{
  return ON_ERROR_DROP;
}

OnErrorResult
on_error_disconnect(gpointer user_data)
{
  return ON_ERROR_DISCONNECT;
}

static void
_free_page(OnErrorParams *self)
{
  g_free((gchar *)self->match_string);
  g_free(self);
}

gboolean
on_error_handlers_is_used(OnErrorHandlers *self)
{
  return !!g_hash_table_size(self);
}

OnErrorHandlers *
on_error_handlers_new(void)
{
  return g_hash_table_new_full(g_int_hash, g_int_equal, NULL, (GDestroyNotify)_free_page);
}

void
on_error_handlers_free(OnErrorHandlers *self)
{
  g_hash_table_destroy(self);
}

static OnErrorParams *
_clone(OnErrorParams *self)
{
  OnErrorParams *clone = g_new(OnErrorParams, 1);
  *clone = *self;
  clone->match_string = g_strdup(clone->match_string);
  return clone;
}

void
on_error_handlers_insert(OnErrorHandlers *self, OnErrorParams *params)
{
  OnErrorParams *clone = _clone(params);
  g_hash_table_insert(self, &clone->status_code, clone);
}

OnErrorParams *
on_error_handlers_lookup(OnErrorHandlers *self, glong status_code, const gchar *data, gsize len)
{
  OnErrorParams *candidate = g_hash_table_lookup(self, &status_code);
  if (!candidate)
    return NULL;

  const gchar *match_string = candidate->match_string;
  if (match_string && match_string[0])
    {
      if (g_strstr_len(data, len, match_string))
        return candidate;
      else
        return NULL;
    }
  return candidate;
}
