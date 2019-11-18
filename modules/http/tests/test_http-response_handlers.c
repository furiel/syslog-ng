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

#include "syslog-ng.h"
#include "response-handler.h"
#include <criterion/criterion.h>

Test(response_handlers, test_response_handlers)
{
  HttpResponseHandlers *self = http_response_handlers_new();

  cr_assert_null(http_response_handlers_lookup(self, 404));

  HttpResponseHandler response_handler =  { .status_code = 404 };
  http_response_handlers_insert(self, &response_handler);
  /* While overwriting, g_hash_table frees key and value in strange
  order: it is easy to get use after free. So I add an additional
  insert in unit test. */
  http_response_handlers_insert(self, &response_handler);
  cr_assert(http_response_handlers_lookup(self, 404));

  http_response_handlers_free(self);

}
