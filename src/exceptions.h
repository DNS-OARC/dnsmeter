/*
 * Copyright (c) 2019-2021, OARC, Inc.
 * Copyright (c) 2019, DENIC eG
 * All rights reserved.
 *
 * This file is part of dnsmeter.
 *
 * dnsmeter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * dnsmeter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dnsmeter.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ppl7.h>

#ifndef __dnsmeter_exceptions_h
#define __dnsmeter_exceptions_h

PPL7EXCEPTION(MissingCommandlineParameter, Exception);
PPL7EXCEPTION(InvalidCommandlineParameter, Exception);
PPL7EXCEPTION(InvalidDNSQuery, Exception);
PPL7EXCEPTION(UnknownRRType, Exception);
PPL7EXCEPTION(BufferOverflow, Exception);
PPL7EXCEPTION(UnknownDestination, Exception);
PPL7EXCEPTION(InvalidQueryFile, Exception);
PPL7EXCEPTION(UnsupportedIPFamily, Exception);
PPL7EXCEPTION(FailedToInitializePacketfilter, Exception);
PPL7EXCEPTION(KernelAccessFailed, Exception);
PPL7EXCEPTION(SystemCallFailed, Exception);

#endif
