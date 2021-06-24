#include "protocol_mapper.h"

#include <string>

using namespace std;
using namespace protodecode;

int main(int argc, char** argv) {
	if (argc == 3) {
		ProtocolMapper pm(argv[1]);
		ifstream fin(argv[2]);
		string data;
		if (fin.good()) {
			stringstream ss;
			ss << fin.rdbuf();
			assert(fin.good());
			data = ss.str();
		}
		ProtocolState state(data);
		ProtocolMap pmap = pm.process(&state);
		pmap.trace();
	} else {
		ProtocolMapper pm;
		pm.set_scheme("u8 first u16 second mul 2 first blah");
		string hex = "012";
		ProtocolMap pmap;
		ProtocolState state(hex);
		pmap = pm.process(&state);
		pmap.trace();
	}
}

