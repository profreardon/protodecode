#include <iostream>
#include <string>

#include "protocol_library.h"
#include "protocol_map.h"

using namespace std;
using namespace protodecode;

int main(int argc, char** argv) {
	if (argc != 2) {
		cerr << "usage: " << argv[0] << " file.pmap" << endl;
		return -1;
	}
	string data;
	{
	ifstream fin(argv[1]);
	if (fin.good()) {
		stringstream buffer;
		buffer << fin.rdbuf();
		if (fin.good()) data = buffer.str();
	}
	}

	Operator* op = ProtocolLibrary::load_file(argv[1]);
	ProtocolMap pmap;
	op->get_type(&pmap);
	pmap.trace_type();
	ofstream fout(string(argv[1]) + ".h");
	fout << pmap.export_classfile(ProtocolLibrary::protocol_name(argv[1]),
				      data) << endl;
	fout.close();
}
