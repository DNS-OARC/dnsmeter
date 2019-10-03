# dnsmeter

`dnsmeter` is a tool for testing performance of a nameserver and the
infrastructure around it. It generates DNS queries and sends them via UDP
to a target nameserver and counts the answers.

Features:
- payload can be given as a text file or a PCAP file
- can automatically run different load steps, which can be given as a list or ranges
- results per load step can be stored in a CSV file
- sender addresses can be spoofed from a given network or from the addresses found in the PCAP file
- answers are counted, even if source address is spoofed, if answers get routed back to the load generator
- round-trip-times are measured (average, min, mix)
- the amount of DNSSEC queries can be given as percentage of total traffic
- optimized for high amount of packets, on an Intel(R) Xeon(R) CPU E5-2430 v2 @ 2.50GHz it can generate more than 900.000 packets per second
- runs on Linux and FreeBSD

## Dependencies

`dnsmeter` requires a couple of libraries beside a normal C++ compiling
environment with autoconf, automake and libtool.

`dnsmeter` has a non-optional dependency on the PCRE library, `libresolv`
and PCAP library.

`dnsmeter` also includes [pplib](https://github.com/DNS-OARC/pplib),
collection of C++ functions and classes, and it has non-optional dependency
on OpenSSL, bzip2, IDN2 (or IDN1) library and zlib.

To install the dependencies under Debian/Ubuntu:
```
apt-get install -y libssl-dev libbz2-dev libidn2-dev zlib1g-dev libpcap-dev libpcre3-dev
```

NOTE: If your system does not have `libidn2-dev`, please use `libidn11-dev` instead.

To install the dependencies under CentOS (with EPEL enabled):
```
yum install -y openssl-devel bzip2-devel libidn2-devel zlib-devel libpcap-devel pcre-devel
```

NOTE: If your using openSUSE/SLE then bzip2's package is `libbz2-devel`.

To install the dependencies under FreeBSD 10+ using `pkg`:
```
pkg install -y openssl libidn2 libpcap pcre
```

## Building from source tarball

The [source tarball from DNS-OARC](https://www.dns-oarc.net/tools/dnsmeter)
comes prepared with `configure`:

```
tar zxvf dnsmeter-version.tar.gz
cd dnsmeter-version
./configure [options]
make
make install
```

NOTE: If building fails on FreeBSD, try adding these configure
options: `--with-extra-cflags="-I /usr/local/include" --with-extra-ldflags="-L/usr/local/lib"`.

## Building from Git repository

If you are building `dnsmeter` from it's Git repository you will first need
to initiate the Git submodules that exists and later create autoconf/automake
files, this will require a build environment with autoconf, automake and
libtool to be installed.

```
git clone https://github.com/DNS-OARC/dnsmeter.git
cd dnsmeter
git submodule update --init
./autogen.sh
./configure [options]
make
make install
```

## Usage

Once installed please see `man dnsmeter` for usage.

## Example

Lets assume the following scenario:

- load generator runs on FreeBSD
- network interface an which the traffic goes out and comes back is `igb0`
- source IP on the load generator is 192.168.155.20
- target nameserver has IP 192.168.0.1, port 53
- we want to spoof the sender address from the network 10.0.0.0/8
- the payload file is found here: `/home/testdata/payload.txt`
- the nameserver is running on CentOS and we need to set a route back to the load generator: `ip route add 10.0.0.0/8 via 192.168.155.20`
- we want to test the following load steps: 30000,40000,45000,50000,100000,150000
- results should be written to `results.csv`
- DNSSEC rate should be 70%

This makes the following command:

```
dnsmeter -p /home/testdata/payload.txt \
  -r 30000,40000,45000,50000,100000,150000 \
  -s 10.0.0.0/8 \
  -z 192.168.0.1:53 \
  -e igb0 \
  -d 70 \
  -c results.csv
```

In the second example, we want to use a PCAP file as payload and want
to spoof with the addresses from that file:

```
dnsmeter -p /home/testdata/pcap.file1 \
  -r 30000,40000,45000,50000,100000,150000 \
  -s pcap \
  -z 192.168.0.1:53 \
  -e igb0 \
  -c results_pcap.csv
```

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
