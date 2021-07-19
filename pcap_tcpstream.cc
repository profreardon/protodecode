#include "protocol_library.h"

#include "pmaps/ipv4_header.pmap.h"
#include "pmaps/tcp.pmap.h"
#include "pmaps/lcc.pmap.h"
#include "pmaps/pcap.pmap.h"

#include <iostream>
#include <sstream>
#include <string>
#include <arpa/inet.h>

using namespace std;
using namespace protodecode;
using namespace protodecode::pmaps;

map<string, stringstream> _data;
map<string, stringstream> _lens;

void process_packet(const pcap_t::entry_t & pcap) {
	ProtocolState state(pcap.payload());
	lcc_t lcc(&state);
	if (lcc.protocol() != 2048) return;
	ipv4_header_t ip(&state);
	if (ip.protocol() == ip.TCP) {
		stringstream ss;
		tcp_t tcp(&state);
		string data = state.remaining();
		if (data.empty()) return;

		ss << tcp.sport() << "-" << tcp.dport();
		_data[ss.str()] << data;
		_lens[ss.str()] << data.length() << endl;
	}
}


int main(int argc, char** argv) {
	ProtocolLibrary::_("pmaps");
	if (argc == 1) {
		cerr << "usage: " << argv[0] << " pcapfile" << endl;
		return -1;
	}
	ifstream fin(argv[1]);
	assert(fin.good());
	ProtocolState state(fin);
	pcap_t data(&state);
	for (auto &x : data.entry()) {
		process_packet(x);
	}
	for (auto &x : _data) {
		cout << x.first << " " << x.second.str().length() << endl;
		string outfile = argv[1];
		if (outfile.substr(outfile.length() - 5) == ".pcap")
			outfile = outfile.substr(0, outfile.length() - 5);
		outfile += "." + x.first + ".tcp";
		cout << outfile << endl << endl;

		ofstream fout(outfile);
		fout << x.second.str();
	}
	for (auto &x : _lens) {
		cout << x.first << " " << x.second.str().length() << endl;
		string outfile = argv[1];
		if (outfile.substr(outfile.length() - 5) == ".pcap")
			outfile = outfile.substr(0, outfile.length() - 5);
		outfile += "." + x.first + ".tcplen";
		cout << outfile << endl << endl;

		ofstream fout(outfile);
		fout << x.second.str();
	}
}



