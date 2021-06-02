#!/bin/sh -e
# Copyright (c) 2019-2021, OARC, Inc.
# Copyright (c) 2019, DENIC eG
# All rights reserved.
#
# This file is part of dnsmeter.
#
# dnsmeter is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# dnsmeter is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with dnsmeter.  If not, see <http://www.gnu.org/licenses/>.

m4_files="ax_append_flag.m4 ax_cflags_warn_all.m4 ax_compiler_vendor.m4 \
 ax_prepend_flag.m4 ax_pthread.m4 ax_require_defined.m4"

for ax in $m4_files; do
  rm -f "$ax"
  wget -O "$ax" "http://git.savannah.gnu.org/gitweb/?p=autoconf-archive.git;a=blob_plain;f=m4/$ax"
done
