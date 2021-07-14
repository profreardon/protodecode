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
using namespace protodecode;
using namespace protodecode::pmaps;

void process_packet(const pcap_t::entry_t & pcap) {
	ProtocolState state(pcap.payload());
	lcc_t lcc(&state);
	if (lcc.protocol() != 2048) return;
	ipv4_header_t ip(&state);
	if (ip.protocol() == ip.UDP) {
		// udp
		udp_t udp(&state);
		if (udp.sport() == 53) {
			char buf[32];
			dns_t dns(&state);
			if (dns.num_answers()) {
				for (auto x: dns.answers()) {
					cout << x.name() << "," <<
					    inet_ntop(AF_INET, x.ip().c_str(), buf, 32) << endl;
				}
			}
		}
	}
}


int main(int argc, char** argv) {
	ProtocolLibrary::_("pmaps");
	ProtocolState state(cin);
	pcap_t data(&state);
	for (auto &x : data.entry()) {
		process_packet(x);
	}
}



