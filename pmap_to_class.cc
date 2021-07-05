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
	Operator* op = ProtocolLibrary::load_file(argv[1]);
	ProtocolMap pmap;
	op->get_type(&pmap);
	pmap.trace_type();
	cout << endl;
	ofstream fout(string(argv[1]) + ".h");
	fout << pmap.export_classfile(ProtocolLibrary::protocol_name(argv[1])) << endl;
	cout << pmap.export_classfile(ProtocolLibrary::protocol_name(argv[1])) << endl;
	fout.close();
}
