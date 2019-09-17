/*
 * Copyright (c) 2019, OARC, Inc.
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

#include "config.h"

#include "dns_sender.h"

#include <unistd.h>
#include <netinet/in.h>
#include <resolv.h>

int main(int argc, char** argv)
{
    res_init();
    // For unknown reason, res_mkquery is much slower (factor 3) when not
    // setting the following options:
    _res.options |= RES_USE_EDNS0;
    _res.options |= RES_USE_DNSSEC;

    DNSSender Sender;
    return Sender.main(argc, argv);
}
