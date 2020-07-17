/*
 * Copyright (c) 2020 Balabit
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

#ifndef EXAMPLE_DESTINATION_WORKER_H_INCLUDED
#define EXAMPLE_DESTINATION_WORKER_H_INCLUDED 1

#include "logthrdest/logthrdestdrv.h"
#include "syslog-ng.h"

#include <stdlib.h>

#ifndef SYSLOG_NG_HAVE_RANDOM_R
struct random_data {};
#endif

typedef struct _ExampleDestinationWorker
{
  LogThreadedDestWorker super;
  struct random_data random_data;
  char random_buf[16];
  FILE *file;
} ExampleDestinationWorker;

LogThreadedDestWorker *example_destination_dw_new(LogThreadedDestDriver *o, gint worker_index);

#endif
