# DNSPecker
DNSPecker is a tool for testing performance of nameserver and/or infrastructure around it.
It generates dns queries and sends them via UDP to a target nameserver and counts the answers.

features:
  - payload can be given as file
  - can automatically run different load steps, which can be given as list or ranges
  - results per load step can be stored in CSV file
  - sender address can be spoofed from a given network
  - answers are counted, even if source address is spoofed, if answers get routed back
    to the load generator
  - roundtrip-times are measured (average, min, mix)
  - amount of DNSSEC queries can be given as percentage of total traffic
  - optimized for high amount of packets. On an Intel(R) Xeon(R) CPU E5-2430 v2 @ 2.50GHz
    it can generate more than 900.000 packets per second
  - runs on Linux (Ubuntu, CentOS) and FreeBSD


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

"dnspecker -h" shows help

**-q HOST | -s NETWORK**

Source IP, hostname or network from which the packets should be send. If you dont't want to spoof,
use -q with a single IP address or hostname. Use -s followed by a network, if you want to spoof
the source address. dnspecker will generated random IP addresses inside this network.
Example: -s 10.0.0.0/8

**-e ETH**

Ignored on Linux, but on FreeBSD you have to enter the name of the network interface on which the
tool should listen for the answers.

**-z HOST:PORT**

Hostname or IP and Port of the target nameserver

**-p FILE**

File with payload in text format. Each line must contain one query with name and record type.
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
on a slow CPU you may need more threads. Dont't use more threads than cores available on your CPU,
minus one!

**[-r #[,#,#]]**

queryrate or load steps. Can be a single value if you want to test a specific queryrate, a comma
separated list or a range with step with. Default: as much as possible.

Examples:
  - Single value: -r 100000
  - a list of queryrates: -r 10000,20000,30000,40000,50000,60000
  - a range with step: -r 10000-200000,10000

**[-d #]**

Amount of DNSSEC queries in percentage between 0 and 100. Default=0

**[-c FILENAME]**

Filname for results in CSV format.

Attention: if file exists, results are appended!


# Example

Lets assume the following scenario:

- load generator runs on FreeBSD
- network interface an which the traffic goes out and comes back is "igb0"
- source ip on the load generator is 192.168.155.20
- target nameserver has ip 192.168.0.1, port 53
- we want to spoof the sender address from the network 10.0.0.0/8
- the payload file is found here: /home/lasttests/payload.txt
- the nameserver is running on CentOS and we need to set a route back to the load generator:
  ip route add 10.0.0.0/8 via 192.168.155.20
- we want to test the following load steps: 30000,40000,45000,50000,100000,150000
- results should be written to results.csv
- DNSSEC rate should be 70%

This makes the following command:

    dnspecker -p /home/lasttests/payload.txt -r 30000,40000,45000,50000,100000,150000 \
    -s 10.0.0.0/8 -z 192.168.0.1:53 -e igb0 -d 70 -c results.csv
  






