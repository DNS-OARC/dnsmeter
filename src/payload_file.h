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

#include <ppl7.h>
#include <list>

#ifndef __dnsmeter_payload_file_h
#define __dnsmeter_payload_file_h

class PayloadFile {
private:
    ppl7::Mutex                                QueryMutex;
    ppluint64                                  validLinesInQueryFile;
    std::list<ppl7::ByteArray>                 querycache;
    std::list<ppl7::ByteArray>::const_iterator it;
    bool                                       payloadIsPcap;
    bool detectPcap(ppl7::File& ff);
    void loadAndCompile(ppl7::File& ff);
    void loadAndCompilePcapFile(const ppl7::String& Filename);

public:
    PayloadFile();
    void openQueryFile(const ppl7::String& Filename);
    const ppl7::ByteArrayPtr& getQuery();
    bool                      isPcap();
};

#endif
