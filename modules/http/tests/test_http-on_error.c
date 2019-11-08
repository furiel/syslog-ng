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
#include "on-error-handler.h"
#include <criterion/criterion.h>

Test(on_error, test_on_error)
{
  OnErrorHandlers *self = on_error_handlers_new();

  cr_assert_null(on_error_handlers_lookup(self, 404, NULL, 0));

  on_error_handlers_insert(self, &(OnErrorParams)
  {
    .status_code = 404
  });
  cr_assert(on_error_handlers_lookup(self, 404, NULL, 0));

  on_error_handlers_free(self);

}

static OnErrorResult
first_action(gpointer user_data)
{
  return ON_ERROR_SUCCESS;
}

static OnErrorResult
second_action(gpointer user_data)
{
  return ON_ERROR_SUCCESS;
}

Test(on_error, test_on_error_multiple_entries)
{
  OnErrorHandlers *self = on_error_handlers_new();

  on_error_handlers_insert(self, &(OnErrorParams)
  {
    .status_code = 404,
    .match_string = "first_action",
    .action = first_action
  });

  on_error_handlers_insert(self, &(OnErrorParams)
  {
    .status_code = 404,
    .match_string = "second_action",
    .action = second_action
  });

  cr_assert_eq(on_error_handlers_lookup(self, 404, "first_action", sizeof("first_action"))->action, first_action);
  cr_assert_eq(on_error_handlers_lookup(self, 404, "second_action", sizeof("second_action"))->action, second_action);

  /* Matching is based on insert order */
  cr_assert_eq(on_error_handlers_lookup(self, 404, "second_action first_action",
                                        sizeof("second_action first_action"))->action, first_action);

  /* No matching */
  cr_assert_null(on_error_handlers_lookup(self, 42, "first_action", sizeof("first_action")));
  cr_assert_null(on_error_handlers_lookup(self, 404, "do-not-match", sizeof("do-not-match")));

  on_error_handlers_free(self);
}

static OnErrorResult
default_action(gpointer user_data)
{
  return ON_ERROR_SUCCESS;
}

Test(on_error, test_nomatchstring_is_the_default_order_does_not_matter_default_first)
{
  OnErrorHandlers *self = on_error_handlers_new();

  on_error_handlers_insert(self, &(OnErrorParams)
  {
    .status_code = 404,
    .action = default_action
  });

  on_error_handlers_insert(self, &(OnErrorParams)
  {
    .status_code = 404,
    .match_string = "first_action",
    .action = first_action
  });

  cr_assert_eq(on_error_handlers_lookup(self, 404, "first_action", sizeof("first_action"))->action, first_action);
  cr_assert_eq(on_error_handlers_lookup(self, 404, "anything", sizeof("anything"))->action, default_action);

  on_error_handlers_free(self);
}

Test(on_error, test_nomatchstring_is_the_default_order_does_not_matter_default_last)
{
  OnErrorHandlers *self = on_error_handlers_new();

  on_error_handlers_insert(self, &(OnErrorParams)
  {
    .status_code = 404,
    .match_string = "first_action",
    .action = first_action
  });

  on_error_handlers_insert(self, &(OnErrorParams)
  {
    .status_code = 404,
    .action = default_action
  });

  cr_assert_eq(on_error_handlers_lookup(self, 404, "first_action", sizeof("first_action"))->action, first_action);
  cr_assert_eq(on_error_handlers_lookup(self, 404, "anything", sizeof("anything"))->action, default_action);

  on_error_handlers_free(self);
}


Test(on_error, test_nomatchstring_evaluation_happens_in_order)
{
  OnErrorHandlers *self = on_error_handlers_new();

  on_error_handlers_insert(self, &(OnErrorParams)
  {
    .status_code = 404,
    .match_string = "foo",
    .action = first_action
  });

  on_error_handlers_insert(self, &(OnErrorParams)
  {
    .status_code = 404,
    .match_string = "foo_that_should_not_be_reached",
    .action = second_action
  });

  cr_assert_eq(on_error_handlers_lookup(self, 404, "foo_that_should_not_be_reached",
                                        sizeof("foo_that_should_not_be_reached"))->action, first_action);

  on_error_handlers_free(self);
}
