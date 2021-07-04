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

#include "operators.h"

using namespace std;

namespace protodecode {

// TODO: have <u8 these bridge packetline

/* TODO: support construct:
u16 somelength
for somelen
  u8 extenion_type
*/
// this will need to disambiguate the extensions into an array

// TODO: warn on namespace clang
// TODO: allow comments in pmap file

class ProtocolLibrary {
public:
	static ProtocolLibrary* _(const string& dir = ".") {
		static ProtocolLibrary pl(dir);
		return &pl;
	}

	ProtocolLibrary(const string& dir) {
		set_dir(dir);
	}

	virtual void set_dir(const string& dir) {
		_dir = dir;
	}

	virtual Operator* get(const string& name) {
		if (!_protocols.count(name)) load(name);
		if (!_protocols.count(name)) return nullptr;
		return _protocols.at(name).get();
	}

	static Operator* load_file(const string& name) {
		ifstream fin(name);
		if (fin.good()) {
			stringstream buffer;
			buffer << fin.rdbuf();
			if (!fin.good()) return nullptr;
			return parse(buffer.str());
		}
		return nullptr;
	}

	virtual Operator* load(const string& name) {
		ifstream fin(_dir + "/" + name + ".pmap");
		if (fin.good()) {
			stringstream buffer;
			buffer << fin.rdbuf();
			if (!fin.good()) return nullptr;
			return load(name, buffer.str());
		}
		return nullptr;
	}

	virtual Operator* load(const string& name,
			       const string& data) {
		_protocols[name].reset(parse(data));
		return _protocols[name].get();
	}

	static Operator* parse(const string& data) {
		list<string> tokens = tokenize(data);
		return new SequenceOperator(&tokens);
	}


protected:
	static list<string> tokenize(const string& s) {
		list<string> tokens;
		stringstream ss;
		for (const char &c : s) {
			if (c == '{' || c == '}') ss << " " << c << " ";
			else ss << c;
		}
		while (ss.good()) {
			string token;
			ss >> token;
			if (!token.empty()) tokens.emplace_back(token);
		}
		return tokens;
	}

	map<string, unique_ptr<Operator>> _protocols;
	string _dir;
};

}  // namespace protodecode

#endif  // __PROTODECODE__PROTOCOL_LIBRARY__H__
