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

	size_t get_int(const string& name) const {
		return _ints.at(name);
	}

	string get_str(const string& name) const {
		return _strs.at(name);
	}

	bool has_int(const string& name) const {
		return _ints.count(name);
	}

	bool has_str(const string& name) const {
		return _strs.count(name);
	}

	void trace() const {
		for (const auto & x: _ints) {
			cout << x.first << " -> " << x.second << endl;
		}
		for (const auto & x: _strs) {
			cout << x.first << " -> " << x.second << endl;
		}
	}

	void trace_type() const {
		for (const auto &x : _ints) {
			cout << "size_t " << x.first << endl;
		}
		for (const auto &x : _strs) {
			cout << "string " << x.first << endl;
		}
		for (const auto &x : _arrays) {
			cout << "array " << x.first << endl;
			x.second.trace_type();
			cout << "end array" << endl;
		}
		for (const auto &x : _classes) {
			cout << x.second << " " << x.first << endl;
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

	void set_class(const string& prefix, const string& name) {
		_classes[prefix] = name;
	}

	void set_array(const string& prefix, const ProtocolMap& pmap) {
		_arrays[prefix] = pmap;
	}

	string export_classfile(const string& name) {
		stringstream ss;
		ss << "#ifndef __PROTODECODE__PMAPS__" << name << "__H__" << endl;
		ss << "#define __PROTODECODE__PMAPS__" << name << "__H__" << endl;
		ss << endl;
		ss << "using namespace std;" << endl;
		ss << "namespace protodecode {" << endl;
		ss << "namespace pmaps {" << endl << endl;
		ss << export_class(name, 0);
		ss << "}  // namespace pmaps" << endl;
		ss << "}  // namespace protodecode" << endl;
		ss << "#endif  // __PROTODECODE__PMAPS__" << name << "__H__" << endl;

		return ss.str();
	}

	string export_constructor(const string& name, size_t tab_depth) {
		stringstream ss;

		tab(ss, tab_depth);
		ss << name << "_t() {" << endl;
		++tab_depth;
		for (auto& x : _ints) {
			tab(ss, tab_depth);
			ss << "_init_" << x.first << " = false;" << endl;
		}
		for (auto& x : _strs) {
			tab(ss, tab_depth);
			ss << "_init_" << x.first << " = false;" << endl;
		}
		tab(ss, tab_depth - 1);
		ss << "}" << endl << endl;
		tab(ss, tab_depth - 1);
		ss << name << "_t(const ProtocolMap& pmap) {" << endl;
		for (auto& x : _ints) {
                        tab(ss, tab_depth);
			ss << "if (pmap.has_int(\"" << x.first << "\")) set_"
			   << x.first << "(pmap.get_int(\"" << x.first << "\"));"
			   << endl;
                }
                for (auto& x : _strs) {
                        tab(ss, tab_depth);
			ss << "if (pmap.has_str(\"" << x.first << "\")) set_"
			   << x.first << "(pmap.get_str(\"" << x.first << "\"));"
			   << endl;
                }
		tab(ss, tab_depth - 1);
		ss << "}" << endl << endl;

		return ss.str();
	}

	string export_class(const string& name, size_t tab_depth) {
		stringstream ss;

		tab(ss, tab_depth);
		ss << "class " << name << "_t {" << endl;
		tab(ss, tab_depth);
		ss << "public:" << endl;
		++tab_depth;
		ss << export_constructor(name, tab_depth);
		for (auto& x : _ints) {
			tab(ss, tab_depth);
			ss << "void set_" << x.first << "(size_t val) {" << endl;
			tab(ss, tab_depth + 1);
			ss << "_init_" << x.first << " = true;" << endl;
			tab(ss, tab_depth + 1);
			ss << "_" << x.first << " = val;" << endl;
			tab(ss, tab_depth);
			ss << "}" << endl;
			tab(ss, tab_depth);
			ss << "size_t " << x.first << "() const {" << endl;
			tab(ss, tab_depth + 1);
			ss << "if (!_init_" << x.first
			   << ") throw runtime_error(\"" << x.first
			   << " not initialized\");" << endl;
			tab(ss, tab_depth + 1);
			ss << "return _" << x.first << ";" << endl;
			tab(ss, tab_depth);
			ss << "}" << endl;
			tab(ss, tab_depth);
			ss << "bool has_" << x.first << "() const { return _init_"
			   << x.first << "; }" << endl;
		}
		for (auto& x : _strs) {
			tab(ss, tab_depth);
			ss << "void set_" << x.first << "(const string& val) {" << endl;
			tab(ss, tab_depth + 1);
			ss << "_init_" << x.first << " = true;" << endl;
			tab(ss, tab_depth + 1);
			ss << "_" << x.first << " = val;" << endl;
			tab(ss, tab_depth);
			ss << "}" << endl;
			tab(ss, tab_depth);
			ss << "string " << x.first << "() const {" << endl;
			tab(ss, tab_depth + 1);
			ss << "if (!_init_" << x.first
			   << ") throw runtime_error(\"" << x.first
			   << " not initialized\");" << endl;
			tab(ss, tab_depth + 1);
			ss << "return _" << x.first << ";" << endl;
			tab(ss, tab_depth);
			ss << "}" << endl;
			tab(ss, tab_depth);
			ss << "bool has_" << x.first << "() const { return _init_"
			   << x.first << "; }" << endl;
		}
		// strings
		// etal
		tab(ss, tab_depth - 1);
		ss << "protected:" << endl;
		for (auto& x : _ints) {
			tab(ss, tab_depth);
			ss << "bool _init_" << x.first << ";" << endl;
			tab(ss, tab_depth);
			ss << "size_t _" << x.first << ";" << endl;
		}

		for (auto& x : _strs) {
			tab(ss, tab_depth);
			ss << "bool _init_" << x.first << ";" << endl;
			tab(ss, tab_depth);
			ss << "string _" << x.first << ";" << endl;
		}

		tab(ss, tab_depth - 1);
		ss << "};" << endl;
		return ss.str();
	}

protected:
	static void tab(stringstream& ss, size_t tab_depth) {
		for (size_t i = 0; i < tab_depth; ++i) {
			ss << "\t";
		}
		return;
	}

	map<string, size_t> _ints;
	map<string, size_t> _expect;
	map<string, string> _strs;
	size_t _len;

	// extra fields for type scaffolding
	map<string, string> _classes;
	map<string, ProtocolMap> _arrays;
};

}  // namespace protodecode

#endif  // __PROTODECODE__PROTOCOL_MAP__H__
