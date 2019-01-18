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

static void
_free_entry(glong *status_code, GSList *table)
{
  g_slist_free_full(table, (GDestroyNotify)_free_page);
}

OnErrorHandlers *
on_error_handlers_new(void)
{
  /* It would be tempting to use g_hash_table_new_full with
  _free_entry. But the thing is, during insert, we append to a list,
  meaning the pointer behind a http-code will change. So I need to
  replace the value. Then _free_page would be called, freeing the rest
  of the list that we still want to use. */
  return g_hash_table_new(g_int_hash, g_int_equal);
}

void
on_error_handlers_free(OnErrorHandlers *self)
{
  g_hash_table_foreach(self, (GHFunc)_free_entry, NULL);
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

  GSList *table = g_hash_table_lookup(self, &clone->status_code);
  GSList *new_table = g_slist_append(table, clone);
  g_hash_table_insert(self, &clone->status_code, new_table);
}

typedef struct
{
  const gchar *data;
  gsize len;
} CString;

static gint
_match(OnErrorParams *self, CString *cstring)
{
  const gchar *match_string = self->match_string;
  if (match_string && match_string[0])
    {
      if (g_strstr_len(cstring->data, cstring->len, match_string))
        return 0;
      else
        return -1;
    }
  return 0;
}

OnErrorParams *
on_error_handlers_lookup(OnErrorHandlers *self, glong status_code, const gchar *data, gsize len)
{
  GSList *table = g_hash_table_lookup(self, &status_code);
  if (!table)
    return NULL;

  CString string_to_match = { .data=data, .len=len };
  GSList *element = g_slist_find_custom(table, &string_to_match, (GCompareFunc)_match);

  if (element)
    return element->data;
  return NULL;
}
