# DNSMeter
DNSMeter is a tool for testing performance of nameserver and/or infrastructure around it.
It generates dns queries and sends them via UDP to a target nameserver and counts the answers.

features:
  - payload can be given as text file or PCAP file
  - can automatically run different load steps, which can be given as list or ranges
  - results per load step can be stored in CSV file
  - sender address can be spoofed from a given network or from PCAP file, if payload
    is a PCAP file
  - answers are counted, even if source address is spoofed, if answers get routed back
    to the load generator
  - roundtrip-times are measured (average, min, mix)
  - amount of DNSSEC queries can be given as percentage of total traffic
  - optimized for high amount of packets. On an Intel(R) Xeon(R) CPU E5-2430 v2 @ 2.50GHz
    it can generate more than 900.000 packets per second
  - runs on Linux (Ubuntu, CentOS) and FreeBSD


# Requirements
- c++ compiler and libraries (tested with gcc and clang)
- pcre library
- pthreads
- libresolv, which contains the function res_mkquery or libbind
- libpcap (optional, only required if you want to use PCAP files a payload)

# Build and install
    ./configure
    make
    make install

By default the binaries will be installed in /usr/local/bin. If you want them
somewhere else (e.g. /usr/bin), please use ./configure --prefix:

    ./configure --prefix=/usr

# Uninstall
make uninstall

# Usage

"dnsmeter -h" shows help

**-q HOST | -s NETWORK | -s pcap**

Source IP, hostname or network from which the packets should be send. If you dont't want to spoof,
use -q with a single IP address or hostname. Use -s followed by a network, if you want to spoof
the source address. dnsmeter will generated random IP addresses inside this network.
Example: -s 10.0.0.0/8

If payload is a PCAP file, you can use the source addresses and ports from the PCAP file, if
you use "-s pcap"

**-e ETH**

Ignored on Linux, but on FreeBSD you have to enter the name of the network interface on which the
tool should listen for the answers.

**-z HOST:PORT**

Hostname or IP and Port of the target nameserver

**-p FILE**

File with payload in text format or PCAP file. When using a text format each line must
contain one query with name and record type.
Example:

    www.denic.de A
    denic.de NS
    ...

Attention: the file should not be too big, because it is completely loaded into memory and
precompiled to DNS query packets. In my tests I used a 180 MB file with over 8000000 queries.

**[-l #]**

runtime for each load step, default=10 seconds

**[-t #]**

time to wait for answers after each load step. default=2 seconds

**[-n #]**

number of worker threads, default=1
Recommendation:
  - less than 200000 packets per second: 1 Thread
  - 200000 - 500000 packets per second: 2 Threads
  - more than 500000 packets per second: 4 Threads
Attention: this is CPU dependent! If you have a fast CPU, you may need lesser threads,
on a slow CPU you may need more threads. Don't use more threads than cores available on your CPU,
minus one!

**[-r #[,#,#]]**

query rate or load steps. Can be a single value if you want to test a specific query rate, a comma
separated list or a range with step with. Default: as much as possible.

Examples:
  - Single value: -r 100000
  - a list of query rates: -r 10000,20000,30000,40000,50000,60000
  - a range with step: -r 10000-200000,10000

**[-d #]**

Amount of DNSSEC queries in percentage between 0 and 100. Default=0.
Is ignored, if using PCAP file as payload.

**[-c FILENAME]**

Filename for results in CSV format.

Attention: if file exists, results are appended!


# Example

Lets assume the following scenario:

- load generator runs on FreeBSD
- network interface an which the traffic goes out and comes back is "igb0"
- source ip on the load generator is 192.168.155.20
- target nameserver has ip 192.168.0.1, port 53
- we want to spoof the sender address from the network 10.0.0.0/8
- the payload file is found here: /home/testdata/payload.txt
- the nameserver is running on CentOS and we need to set a route back to the load generator:
  ip route add 10.0.0.0/8 via 192.168.155.20
- we want to test the following load steps: 30000,40000,45000,50000,100000,150000
- results should be written to results.csv
- DNSSEC rate should be 70%

This makes the following command:

    dnsmeter -p /home/testdata/payload.txt -r 30000,40000,45000,50000,100000,150000 \
    -s 10.0.0.0/8 -z 192.168.0.1:53 -e igb0 -d 70 -c results.csv

In the second example, we want to use a PCAP file as payload and want to spoof with the
addresses from that file:

    dnsmeter -p /home/testdata/pcap.file1 -r 30000,40000,45000,50000,100000,150000 \
    -s pcap -z 192.168.0.1:53 -e igb0 -c results_pcap.csv



## Author(s)

- Patrick Fedick [@pfedick](https://github.com/pfedick)

## Contributor(s)

- Jerry Lundström [@jelu](https://github.com/jelu)

## Copyright

Copyright (c) 2019, OARC, Inc.

Copyright (c) 2019, DENIC eG

All rights reserved.

```
This file is part of dnsmeter.

dnsmeter is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

dnsmeter is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with dnsmeter.  If not, see <http://www.gnu.org/licenses/>.
```
