#!/usr/bin/env python
#############################################################################
# Copyright (c) 2020 Balabit
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# As an additional exemption you are allowed to compile & link against the
# OpenSSL libraries as published by the OpenSSL project. See the file
# COPYING for details.
#
#############################################################################
import subprocess

import pytest

from src.syslog_ng.exceptions import ConfigError


def test_http_ssl_version_invalid_string(config, syslog_ng):
    generator_source = config.create_example_msg_generator_source(num=1)
    http_destination = config.create_http_destination(ssl_version='notexist')
    config.create_logpath(statements=[generator_source, http_destination])

    with pytest.raises(ConfigError):
        syslog_ng.start(config)


def test_http_ssl_version_remote_too_old(config, syslog_ng, some_port):
    generator_source = config.create_example_msg_generator_source(num=1)
    http_destination = config.create_http_destination(url='"https://localhost:{}"'.format(some_port), ssl_version='tlsv1_2', peer_verify='no')
    config.create_logpath(statements=[generator_source, http_destination])

    openssl_args = [
        "openssl", "s_server",
        "-cert", "cert.pem",
        "-key", "key.pem",
        "-port", str(some_port), "-tls1_1",
    ]

    with subprocess.Popen(openssl_args):
        syslog_ng.start(config)
        syslog_ng.wait_for_console_log("Unknown SSL protocol error in connection")
