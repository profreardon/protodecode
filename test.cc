#include "protocol_library.h"

#include <string>

#include "pmaps/tcp_header.pmap.h"
using namespace std;
using namespace protodecode;

int main(int argc, char** argv) {
	if (argc == 3) {
		ProtocolLibrary::_("example");
		Operator* op = ProtocolLibrary::load_file(argv[1]);
		ifstream fin(argv[2]);

		string data;
		if (fin.good()) {
			stringstream ss;
			ss << fin.rdbuf();
			assert(fin.good());
			data = ss.str();
		}
		ProtocolState state(data);
		ProtocolMap pmap;
		op->process(&pmap, &state);
		pmap.trace();
	} else {
		try {
			Operator* op = ProtocolLibrary::parse("u8 first u16 second mul 2 first blah");
			string hex = "012";
			ProtocolMap pmap;
			ProtocolState state(hex);
			op->process(&pmap, &state);
			pmap.trace();
		} catch (char const* c) {
			cerr << "error: " << c << endl;
		}
	}
}

