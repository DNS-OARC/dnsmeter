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

#include "dns_receiver_thread.h"
#include "payload_file.h"
#include "system_stat.h"

#include <ppl7.h>

#ifndef __dnsmeter_dns_sender_h
#define __dnsmeter_dns_sender_h

class DNSSender {
public:
    class Results {
    public:
        int       queryrate;
        ppluint64 counter_send;
        ppluint64 counter_received;
        ppluint64 bytes_send;
        ppluint64 bytes_received;
        ppluint64 counter_errors;
        ppluint64 packages_lost;
        ppluint64 counter_0bytes;
        ppluint64 counter_errorcodes[255];
        ppluint64 rcodes[16];
        ppluint64 truncated;
        double    rtt_total;
        double    rtt_avg;
        double    rtt_min;
        double    rtt_max;
        Results();
        void clear();
    };

private:
    ppl7::ThreadPool   threadpool;
    ppl7::IPAddress    TargetIP;
    ppl7::IPAddress    SourceIP;
    ppl7::IPNetwork    SourceNet;
    ppl7::String       CSVFileName;
    ppl7::String       QueryFilename;
    ppl7::File         CSVFile;
    ppl7::Array        rates;
    ppl7::String       InterfaceName;
    PayloadFile        payload;
    DNSReceiverThread* Receiver;
    DNSSender::Results vis_prev_results;
    SystemStat         sys1, sys2;

    int   TargetPort;
    int   Runtime;
    int   Timeout;
    int   ThreadCount;
    int   DnssecRate;
    float Timeslices;
    bool  ignoreResponses;
    bool  spoofingEnabled;
    bool  spoofFromPcap;

    void openCSVFile(const ppl7::String& Filename);
    void run(int queryrate);
    void presentResults(const DNSSender::Results& result);
    void saveResultsToCsv(const DNSSender::Results& result);
    void prepareThreads();
    void getResults(DNSSender::Results& result);
    ppl7::Array getQueryRates(const ppl7::String& QueryRates);
    void readSourceIPList(const ppl7::String& filename);

    void getTarget(int argc, char** argv);
    void getSource(int argc, char** argv);
    int getParameter(int argc, char** argv);
    int  openFiles();
    void calcTimeslice(int queryrate);

    void showCurrentStats(ppl7::ppl_time_t start_time);

public:
    DNSSender();
    ~DNSSender();
    void help();
    int main(int argc, char** argv);
};

DNSSender::Results operator-(const DNSSender::Results& first, const DNSSender::Results& second);

#endif
