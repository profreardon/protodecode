#ifndef __PROTODECODE__PROTOCOL_MAP__H__
#define __PROTODECODE__PROTOCOL_MAP__H__

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

class ProtocolMap {
public:
	void set_int(const string& name, size_t value) {
		if (_expect.count(name) && _expect.at(name) != value) {
			stringstream ss;
			ss << "expectation fail. bad format: "
			   << name << " should be " << _expect.at(name)
			   << " but is " << value;
			throw runtime_error(ss.str());
		}
		_ints[name] = value;
	}

	void set_expect(const string& name, size_t value) {
		_expect[name] = value;
	}

	void set_str(const string& name, const string& value) {
		_strs[name] = value;
	}

	size_t get_int(const string& name) {
		return _ints[name];
	}

	string get_str(const string& name) {
		return _strs[name];
	}

	void trace() {
		for (const auto & x: _ints) {
			cout << x.first << " -> " << x.second << endl;
		}
		for (const auto & x: _strs) {
			cout << x.first << " -> " << x.second << endl;
		}
	}

	void merge(const string& prefix, const ProtocolMap& pmap) {
		for (const auto &x : pmap._ints) {
			_ints[prefix + "_" + x.first] = x.second;
		}
		for (const auto &x : pmap._strs) {
			_strs[prefix + "_" + x.first] = x.second;
		}
	}

protected:
	map<string, size_t> _ints;
	map<string, size_t> _expect;
	map<string, string> _strs;
	size_t _len;

};

}  // namespace protodecode

#endif  // __PROTODECODE__PROTOCOL_MAP__H__
