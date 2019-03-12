/*
 * sensor.h
 *
 *  Created on: 21.07.2016
 *      Author: patrickf
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include <ppl7.h>
#include <map>

PPL7EXCEPTION(KernelAccessFailed, Exception);
PPL7EXCEPTION(SystemCallFailed, Exception);

class SystemStat
{
	public:
		class Network
		{
				public:
					unsigned long bytes;
					unsigned long packets;
					unsigned long errs;
					unsigned long drop;
					Network() {
						bytes=packets=errs=drop=0;
					}
					Network(unsigned long bytes, unsigned long packets, unsigned long errs, unsigned long drop) {
						this->bytes=bytes;
						this->packets=packets;
						this->errs=errs;
						this->drop=drop;

					}
					void clear() {
						bytes=packets=errs=drop=0;
					}
					void print() {
						printf ("Network bytes: %lu, packets: %lu, errs: %lu, drop: %lu\n",
								bytes, packets, errs, drop);
					}

					static Network getDelta(const Network &sample1, const Network &sample2);
		};

		class Cpu
		{
			public:
				Cpu() {
					user=nice=system=idle=iowait=0;
				}
				int user;
				int nice;
				int system;
				int idle;
				int iowait;

				static double getUsage(const SystemStat::Cpu &sample1,const SystemStat::Cpu &sample2);

		};

		class Sysinfo
		{
			public:
				Sysinfo() {
					uptime=freeswap=totalswap=freeram=bufferram=totalram=sharedram=0;
					procs=0;
				}
				long uptime;
				long freeswap;
				long totalswap;
				long freeram;
				long bufferram;
				long totalram;
				long sharedram;
				int procs;
		};

		class Interface
		{
		public:
			ppl7::String Name;
			Network receive;
			Network transmit;
		};

		double sampleTime;

		Cpu		cpu;
		Sysinfo	sysinfo;
		Interface net_total;
		std::map<ppl7::String, Interface> interfaces;

		void exportToArray(ppl7::AssocArray &data) const;
		void importFromArray(const ppl7::AssocArray &data);
		void print() const;

};

void sampleSensorData(SystemStat &stat);


#endif /* SENSOR_H_ */
