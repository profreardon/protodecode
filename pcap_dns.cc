#include "protocol_library.h"

#include "pmaps/ipv4_header.pmap.h"
#include "pmaps/udp.pmap.h"
#include "pmaps/lcc.pmap.h"
#include "pmaps/dns.pmap.h"
#include "pmaps/pcap.pmap.h"

#include "pcap_parser.h"

#include <iostream>
#include <sstream>
#include <string>
#include <arpa/inet.h>

using namespace std;
using namespace protodecode;
using namespace protodecode::pmaps;

int main(int argc, char** argv) {
	if (argc != 2) {
		cerr << "usage: " << argv[0] << " pcapfile" << endl;
		return -1;
	}
	map<string, string> dns = PcapParser::dns_table(argv[1]);
	for (auto &x : dns)
		cout << x.first << "," << x.second << endl;
}



