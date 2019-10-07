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
#include "exceptions.h"
#include "dns_sender_thread.h"

#include <signal.h>
#include <string.h>

static const char* rcode_names[] = {
    "OK", "FORMAT", "SRVFAIL", "NAME", "NOTIMPL", "REFUSED",
    "YXDOMAIN", "YXRRSET", "NXRRSET", "NOTAUTH", "NOTZONE",
    "11", "12", "13", "14", "15",
    NULL
};

bool stopFlag = false;

void sighandler(int sig)
{
    stopFlag = true;
    printf("Stopping...\n");
}

void DNSSender::help()
{
    ppl7::String name, underline;
    name.setf("dnsmeter %s", PACKAGE_VERSION);
    underline.repeat("=", name.size());
    name.printnl();
    underline.printnl();
    printf("\nUsage:\n"
           "  -h            shows this help\n"
           "  -q HOST       hostname or IP address of sender if you don't want to spoof\n"
           "                (see -s)\n"
           "  -s NET|pcap   spoof sender address. Use random IP from the given network\n"
           "                (example: 192.168.0.0/16). Only works when running as root!\n"
           "                If payload is a pcap file, you can use \"-s pcap\" to use the\n"
           "                source addresses and ports from the pcap file.\n"
           "  -e ETH        interface on which the packet receiver should listen\n"
           "                (FreeBSD only)\n"
           "  -z HOST:PORT  hostname or IP address and port of the target nameserver\n"
           "  -p FILE       file with queries/payload or pcap file\n"
           "  -l #          runtime in seconds (default=10 seconds)\n"
           "  -t #          timeout in seconds (default=2 seconds)\n"
           "  -n #          number of worker threads (default=1)\n"
           "  -r #          queryrate (Default=as much as possible)\n"
           "                can be a single value, a comma separated list (rate,rate,...)\n"
           "                or a range and a step value (start - end, step)\n"
           "  -d #          amount of queries in percent on which the DNSSEC-flags are set\n"
           "                (default=0)\n"
           "  -c FILE       CSV-file for results\n"
           "  --ignore      answers are ignored and therefor not counted. In this mode\n"
           "                the tool only generates traffic."
           "\n");
}

DNSSender::Results::Results()
{
    queryrate        = 0;
    counter_send     = 0;
    counter_received = 0;
    bytes_send       = 0;
    bytes_received   = 0;
    counter_errors   = 0;
    packages_lost    = 0;
    counter_0bytes   = 0;
    for (int i                = 0; i < 255; i++)
        counter_errorcodes[i] = 0;
    rtt_avg                   = 0.0f;
    rtt_total                 = 0.0f;
    rtt_min                   = 0.0f;
    rtt_max                   = 0.0f;
    for (int i    = 0; i < 16; i++)
        rcodes[i] = 0;
    truncated     = 0;
}

void DNSSender::Results::clear()
{
    queryrate        = 0;
    counter_send     = 0;
    counter_received = 0;
    bytes_send       = 0;
    bytes_received   = 0;
    counter_errors   = 0;
    packages_lost    = 0;
    counter_0bytes   = 0;
    for (int i                = 0; i < 255; i++)
        counter_errorcodes[i] = 0;
    rtt_avg                   = 0.0f;
    rtt_total                 = 0.0f;
    rtt_min                   = 0.0f;
    rtt_max                   = 0.0f;
    for (int i    = 0; i < 16; i++)
        rcodes[i] = 0;
    truncated     = 0;
}

DNSSender::Results operator-(const DNSSender::Results& second, const DNSSender::Results& first)
{
    DNSSender::Results r;
    r.queryrate        = second.queryrate - first.queryrate;
    r.counter_send     = second.counter_send - first.counter_send;
    r.counter_received = second.counter_received - first.counter_received;
    r.bytes_send       = second.bytes_send - first.bytes_send;
    r.bytes_received   = second.bytes_received - first.bytes_received;
    r.counter_errors   = second.counter_errors - first.counter_errors;
    r.packages_lost    = second.packages_lost - first.packages_lost;
    r.counter_0bytes   = second.counter_0bytes - first.counter_0bytes;
    for (int i                  = 0; i < 255; i++)
        r.counter_errorcodes[i] = second.counter_errorcodes[i] - first.counter_errorcodes[i];
    r.rtt_total                 = second.rtt_total - first.rtt_total;
    if (r.counter_received)
        r.rtt_avg = r.rtt_total / r.counter_received;
    else
        r.rtt_avg = 0.0;
    r.rtt_min     = second.rtt_min - first.rtt_min;
    r.rtt_max     = second.rtt_max - first.rtt_max;

    for (int i      = 0; i < 16; i++)
        r.rcodes[i] = second.rcodes[i] - first.rcodes[i];
    r.truncated     = second.truncated - first.truncated;
    return r;
}

DNSSender::DNSSender()
{
    ppl7::InitSockets();
    Runtime         = 10;
    Timeout         = 2;
    ThreadCount     = 1;
    Timeslices      = 1.0f;
    ignoreResponses = false;
    DnssecRate      = 0;
    TargetPort      = 53;
    spoofingEnabled = false;
    Receiver        = NULL;
    spoofFromPcap   = false;
}

DNSSender::~DNSSender()
{
    if (Receiver)
        delete Receiver;
}

ppl7::Array DNSSender::getQueryRates(const ppl7::String& QueryRates)
{
    ppl7::Array rates;
    if (QueryRates.isEmpty()) {
        rates.add("0");
    } else {
        ppl7::Array matches;
        if (QueryRates.pregMatch("/^([0-9]+)-([0-9]+),([0-9]+)$", matches)) {
            for (ppluint64 i = matches[1].toUnsignedInt64(); i <= matches[2].toUnsignedInt64(); i += matches[3].toUnsignedInt64()) {
                rates.addf("%llu", i);
            }
        } else {
            rates.explode(QueryRates, ",");
        }
    }
    return rates;
}

void DNSSender::getTarget(int argc, char** argv)
{
    if (!ppl7::HaveArgv(argc, argv, "-z")) {
        throw MissingCommandlineParameter("target IP/hostname or port missing (-z IP:PORT)");
    }
    ppl7::String Tmp = ppl7::GetArgv(argc, argv, "-z");
    ppl7::Array  Tok(Tmp, ":");
    if (Tok.size() != 2) {
        if (Tok.size() != 1)
            throw InvalidCommandlineParameter("-z IP:PORT");
        TargetPort = 53;
    } else {
        TargetPort = Tok[1].toInt();
    }
    if (TargetPort < 1 || TargetPort > 65535)
        throw InvalidCommandlineParameter("-z IP:PORT, Invalid Port");
    std::list<ppl7::IPAddress> Result;
    size_t                     num = ppl7::GetHostByName(Tok[0], Result, ppl7::af_inet);
    if (!num)
        throw InvalidCommandlineParameter("-z IP:PORT, Invalid IP or could not resolve Hostname");
    TargetIP = Result.front();
    //printf ("num=%d, %s\n",num, (const char*)TargetIP.toString());
}

void DNSSender::getSource(int argc, char** argv)
{
    if (ppl7::HaveArgv(argc, argv, "-s")) {
        ppl7::String Tmp = ppl7::GetArgv(argc, argv, "-s").toLowerCase();
        if (Tmp == "pcap") {
            spoofFromPcap = true;
        } else {
            SourceNet.set(Tmp);
            if (SourceNet.family() != ppl7::IPAddress::IPv4)
                throw UnsupportedIPFamily("only IPv4 works");
        }
        spoofingEnabled = true;
    } else {
        ppl7::String               Tmp = ppl7::GetArgv(argc, argv, "-q");
        std::list<ppl7::IPAddress> Result;
        size_t                     num = ppl7::GetHostByName(Tmp, Result, ppl7::af_inet);
        if (!num)
            throw InvalidCommandlineParameter("-q HOST, Invalid IP or could not resolve Hostname");
        SourceIP = Result.front();
        if (SourceIP.family() != ppl7::IPAddress::IPv4)
            throw UnsupportedIPFamily("only IPv4 works");
        spoofingEnabled = false;
    }
}

int DNSSender::getParameter(int argc, char** argv)
{
    if (ppl7::HaveArgv(argc, argv, "-q") && ppl7::HaveArgv(argc, argv, "-s")) {
        printf("ERROR: could not use parameters -q and -s together\n\n");
        help();
        return 1;
    }
    if ((!ppl7::HaveArgv(argc, argv, "-q")) && (!ppl7::HaveArgv(argc, argv, "-s"))) {
        printf("ERROR: source IP/hostname or network for source address spoofing missing (-q IP | -s NETWORK)\n\n");
        help();
        return 1;
    }
    ignoreResponses = ppl7::HaveArgv(argc, argv, "--ignore");

    if (ppl7::HaveArgv(argc, argv, "-e")) {
        InterfaceName = ppl7::GetArgv(argc, argv, "-e");
    }

    try {
        getTarget(argc, argv);
        getSource(argc, argv);
    } catch (const ppl7::Exception& e) {
        printf("ERROR: missing or invalid parameter\n");
        e.print();
        printf("\n");
        help();
        return 1;
    }

    Runtime                 = ppl7::GetArgv(argc, argv, "-l").toInt();
    Timeout                 = ppl7::GetArgv(argc, argv, "-t").toInt();
    ThreadCount             = ppl7::GetArgv(argc, argv, "-n").toInt();
    ppl7::String QueryRates = ppl7::GetArgv(argc, argv, "-r");
    CSVFileName             = ppl7::GetArgv(argc, argv, "-c");
    QueryFilename           = ppl7::GetArgv(argc, argv, "-p");
    if (ppl7::HaveArgv(argc, argv, "-d")) {
        DnssecRate = ppl7::GetArgv(argc, argv, "-d").toInt();
        if (DnssecRate < 0 || DnssecRate > 100) {
            printf("ERROR: DNSSEC-Rate must be an integer between 0 and 100 (-d #)\n\n");
            help();
            return 1;
        }
    }
    if (!ThreadCount)
        ThreadCount = 1;
    if (!Runtime)
        Runtime = 10;
    if (!Timeout)
        Timeout = 2;
    if (QueryFilename.isEmpty()) {
        printf("ERROR: Payload-File is missing (-p FILENAME)\n\n");
        help();
        return 1;
    }
    rates = getQueryRates(QueryRates);
    return 0;
}

int DNSSender::openFiles()
{
    if (CSVFileName.notEmpty()) {
        try {
            openCSVFile(CSVFileName);
        } catch (const ppl7::Exception& e) {
            printf("ERROR: could not open CSV-file for writing\n");
            e.print();
            return 1;
        }
    }
    try {
        payload.openQueryFile(QueryFilename);
    } catch (const ppl7::Exception& e) {
        printf("ERROR: could not open payload file or it does not contain any queries\n");
        e.print();
        return 1;
    }
    return 0;
}

int DNSSender::main(int argc, char** argv)
{
    if (ppl7::HaveArgv(argc, argv, "-h") || ppl7::HaveArgv(argc, argv, "--help") || argc < 2) {
        help();
        return 0;
    }
    if (getParameter(argc, argv) != 0)
        return 1;
    if (openFiles() != 0)
        return 1;

    signal(SIGINT, sighandler);
    signal(SIGKILL, sighandler);

    DNSSender::Results results;
    try {
        if (!ignoreResponses) {
            Receiver = new DNSReceiverThread();
            Receiver->setSource(TargetIP, TargetPort);
            try {
                Receiver->setInterface(InterfaceName);
            } catch (const ppl7::Exception& e) {
                printf("ERROR: could not bind on device [%s]\n", (const char*)InterfaceName);
                e.print();
                printf("\n");
                help();
                return 1;
            }
        }
        prepareThreads();
        for (size_t i = 0; i < rates.size(); i++) {
            results.queryrate = rates[i].toInt();
            run(rates[i].toInt());
            getResults(results);
            presentResults(results);
            saveResultsToCsv(results);
        }
        threadpool.destroyAllThreads();
    } catch (const ppl7::OperationInterruptedException&) {
        getResults(results);
        presentResults(results);
        saveResultsToCsv(results);
    } catch (const ppl7::Exception& e) {
        e.print();
        return 1;
    }
    return 0;
}

void DNSSender::prepareThreads()
{
    for (int i = 0; i < ThreadCount; i++) {
        DNSSenderThread* thread = new DNSSenderThread();
        thread->setDestination(TargetIP, TargetPort);
        thread->setRuntime(Runtime);
        thread->setTimeout(Timeout);
        thread->setTimeslice(Timeslices);
        thread->setDNSSECRate(DnssecRate);
        thread->setVerbose(false);
        thread->setPayload(payload);
        if (spoofingEnabled) {
            if (spoofFromPcap)
                thread->setSourcePcap();
            else
                thread->setSourceNet(SourceNet);
        } else {
            thread->setSourceIP(SourceIP);
        }
        threadpool.addThread(thread);
    }
}

void DNSSender::openCSVFile(const ppl7::String& Filename)
{
    CSVFile.open(Filename, ppl7::File::APPEND);
    if (CSVFile.size() == 0) {
        CSVFile.putsf("#QPS Send; QPS Received; QPS Errors; Lostrate; "
                      "rtt_avg; rtt_min; rtt_max;"
                      "\n");
        CSVFile.flush();
    }
}

void DNSSender::showCurrentStats(ppl7::ppl_time_t start_time)
{
    DNSSender::Results result, diff;
    ppl7::ppl_time_t   runtime = ppl7::GetTime() - start_time;
    getResults(result);
    diff             = result - vis_prev_results;
    vis_prev_results = result;

    int h = (int)(runtime / 3600);
    runtime -= h * 3600;
    int m = (int)(runtime / 60);
    int s = runtime - (m * 60);

    printf("%02d:%02d:%02d Queries send: %7llu, rcv: %7llu, ", h, m, s,
        diff.counter_send, diff.counter_received);
    printf("Data send: %6llu KB, rcv: %6llu KB", diff.bytes_send / 1024, diff.bytes_received / 1024);
    printf("\n");
}

void DNSSender::calcTimeslice(int queryrate)
{
    Timeslices = (1000.0f / queryrate) * ThreadCount;
    //if (Zeitscheibe<1.0f) Zeitscheibe=1.0f;
    if (Timeslices < 0.1f)
        Timeslices = 0.1f;
}

void DNSSender::run(int queryrate)
{
    printf("###############################################################################\n");
    if (queryrate) {
        calcTimeslice(queryrate);
        printf("# Start Session with Threads: %d, Queryrate: %d, Timeslot: %0.6f ms\n",
            ThreadCount, queryrate, Timeslices);
    } else {
        printf("# Start Session with Threads: %d, Queryrate: unlimited\n",
            ThreadCount);
    }

    ppl7::ThreadPool::iterator it;
    for (it = threadpool.begin(); it != threadpool.end(); ++it) {
        ((DNSSenderThread*)(*it))->setQueryRate(queryrate / ThreadCount);
        ((DNSSenderThread*)(*it))->setTimeslice(Timeslices);
    }
    vis_prev_results.clear();
    sampleSensorData(sys1);
    if (Receiver)
        Receiver->threadStart();
    threadpool.startThreads();
    ppl7::ppl_time_t start  = ppl7::GetTime();
    ppl7::ppl_time_t report = start + 1;
    ppl7::MSleep(500);
    while (threadpool.running() == true && stopFlag == false) {
        ppl7::MSleep(100);
        ppl7::ppl_time_t now = ppl7::GetTime();
        if (now >= report) {
            report = now + 1;
            showCurrentStats(start);
        }
    }
    if (Receiver)
        Receiver->threadStop();
    sampleSensorData(sys2);
    if (stopFlag == true) {
        threadpool.stopThreads();
        throw ppl7::OperationInterruptedException("test aborted");
    }
}

void DNSSender::getResults(DNSSender::Results& result)
{
    ppl7::ThreadPool::iterator it;
    result.clear();

    for (it = threadpool.begin(); it != threadpool.end(); ++it) {
        result.counter_send += ((DNSSenderThread*)(*it))->getPacketsSend();
        result.bytes_send += ((DNSSenderThread*)(*it))->getBytesSend();
        result.counter_errors += ((DNSSenderThread*)(*it))->getErrors();
        result.counter_0bytes += ((DNSSenderThread*)(*it))->getCounter0Bytes();
        for (int i = 0; i < 255; i++)
            result.counter_errorcodes[i] += ((DNSSenderThread*)(*it))->getCounterErrorCode(i);
    }
    if (Receiver) {
        const RawSocketReceiver::Counter& counter = Receiver->getCounter();
        result.counter_received                   = counter.num_pkgs;
        result.bytes_received                     = counter.bytes_rcv;
        result.rtt_total                          = counter.rtt_total;
        if (counter.num_pkgs)
            result.rtt_avg = counter.rtt_total / counter.num_pkgs;
        else
            result.rtt_avg = 0.0;
        result.rtt_min     = counter.rtt_min;
        result.rtt_max     = counter.rtt_max;
        for (int i           = 0; i < 16; i++)
            result.rcodes[i] = counter.rcodes[i];
        result.truncated     = counter.truncated;
    }

    result.packages_lost = result.counter_send - result.counter_received;
    if (result.counter_received > result.counter_send)
        result.packages_lost = 0;
}

void DNSSender::saveResultsToCsv(const DNSSender::Results& result)
{

    if (CSVFile.isOpen()) {
        CSVFile.putsf("%llu;%llu;%llu;%0.3f;%0.4f;%0.4f;%0.4f;\n",
            (ppluint64)((double)result.counter_send / (double)Runtime),
            (ppluint64)((double)result.counter_received / (double)Runtime),
            (ppluint64)((double)result.counter_errors / (double)Runtime),
            (double)result.packages_lost * 100.0 / (double)result.counter_send,
            result.rtt_avg * 1000.0,
            result.rtt_min * 1000.0,
            result.rtt_max * 1000.0);
        CSVFile.flush();
    }
}

void DNSSender::presentResults(const DNSSender::Results& result)
{
    printf("===============================================================================\n");
    const SystemStat::Interface& net1     = sys1.interfaces[InterfaceName];
    const SystemStat::Interface& net2     = sys2.interfaces[InterfaceName];
    SystemStat::Network          transmit = SystemStat::Network::getDelta(net1.transmit, net2.transmit);
    SystemStat::Network          received = SystemStat::Network::getDelta(net1.receive, net2.receive);
    printf("network if %s Pkt send: %lu, rcv: %lu, Data send: %lu KB, rcv: %lu KB\n",
        (const char*)InterfaceName,
        transmit.packets, received.packets, transmit.bytes / 1024, received.bytes / 1024);

    ppluint64 qps_send     = (ppluint64)((double)result.counter_send / (double)Runtime);
    ppluint64 bps_send     = (ppluint64)((double)result.bytes_send / (double)Runtime);
    ppluint64 qps_received = (ppluint64)((double)result.counter_received / (double)Runtime);
    ppluint64 bps_received = (ppluint64)((double)result.bytes_received / (double)Runtime);

    printf("DNS Queries send: %10llu, Qps: %7llu, Data send: %7llu KB = %6llu MBit\n",
        result.counter_send, qps_send, result.bytes_send / 1024, bps_send / (1024 * 1024));

    printf("DNS Queries rcv:  %10llu, Qps: %7llu, Data rcv:  %7llu KB = %6llu MBit\n",
        result.counter_received, qps_received, result.bytes_received / 1024, bps_received / (1024 * 1024));

    printf("DNS Queries lost: %10llu = %0.3f %%\n", result.packages_lost,
        (double)result.packages_lost * 100.0 / (double)result.counter_send);

    printf("DNS rtt average: %0.4f ms, "
           "min: %0.4f ms, "
           "max: %0.4f ms\n",
        result.rtt_avg * 1000.0,
        result.rtt_min * 1000.0,
        result.rtt_max * 1000.0);
    printf("DNS truncated: %llu\nDNS RCODES: ", result.truncated);
    for (int i = 0; i < 15; i++) {
        if (result.rcodes[i]) {
            printf("%s: %llu, ", rcode_names[i], result.rcodes[i]);
        }
    }
    printf("\n");

    if (result.counter_errors) {
        printf("Errors:           %10llu, Qps: %10llu\n", result.counter_errors,
            (ppluint64)((double)result.counter_errors / (double)Runtime));
    }
    if (result.counter_0bytes) {
        printf("Errors 0Byte:     %10llu, Qps: %10llu\n", result.counter_0bytes,
            (ppluint64)((double)result.counter_0bytes / (double)Runtime));
    }
    for (int i = 0; i < 255; i++) {
        if (result.counter_errorcodes[i] > 0) {
            printf("Errors %3d:       %10llu, Qps: %10llu [%s]\n", i, result.counter_errorcodes[i],
                (ppluint64)((double)result.counter_errorcodes[i] / (double)Runtime),
                strerror(i));
        }
    }
}
