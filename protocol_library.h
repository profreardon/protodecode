#ifndef __PROTODECODE__PROTOCOL_LIBRARY__H__
#define __PROTODECODE__PROTOCOL_LIBRARY__H__

#include <arpa/inet.h>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace protodecode {

class ProtocolLibrary {
public:
	static ProtocolLibrary* _() {
		static ProtocolLibrary pl;
		return &pl;
	}

	virtual ProtocolMapper* get(const string& name) {
		if (!_protocols.count(name)) load(name);
		if (!_protocols.count(name)) return nullptr;
		return _protocols.at(name).get();
	}

	virtual void load(const string& name) {
		ifstream fin(_dir + "/" + name + ".pmap");
		if (fin.good()) {
			_protocols[name].reset(new ProtocolMapper(
				_dir + "/" + name + ".pmap"));
		}
	}

protected:
	map<string, unique_ptr<ProtocolMapper>> _protocols;
	string _dir;
};

}  // namespace protodecode

#endif  // __PROTODECODE__PROTOCOL_LIBRARY__H__
