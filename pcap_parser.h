#ifndef __PROTODECODE__PCAP_PARSER__H__
#define __PROTODECODE__PCAP_PARSER__H__

#include "protocol_library.h"

#include "pmaps/ipv4_header.pmap.h"
#include "pmaps/udp.pmap.h"
#include "pmaps/lcc.pmap.h"
#include "pmaps/dns.pmap.h"
#include "pmaps/pcap.pmap.h"

#include <iostream>
#include <sstream>
#include <string>
#include <arpa/inet.h>

using namespace std;

namespace protodecode {

using namespace protodecode::pmaps;

class PcapParser {
public:
	static map<string, string> dns_table(const string& pcap) {
		ifstream fin(pcap);
		map<string, string> retval;
		if (!fin.good()) return retval;
		ProtocolState state(fin);
		pcap_t data(&state);
		for (auto &x: data.entry()) {
			ProtocolState packet(x.payload());
			lcc_t lcc(&packet);
			if (lcc.protocol() != lcc.IPV4) continue;
			ipv4_header_t ip(&packet);
			if (ip.protocol() != ip.UDP) continue;
			udp_t udp(&packet);
			if (udp.sport() != 53) continue;
			char buf[32];
			dns_t dns(&packet);
			for (auto x: dns.answers()) {
				retval[inet_ntop(AF_INET, x.ip().c_str(), buf, 32)] = x.name();
			}
		}
		return retval;
	}
};

}  // namespace protodecode

#endif  // __PROTODECODE__PCAP_PARSER__H__

