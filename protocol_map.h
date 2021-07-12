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
	ProtocolMap() : _parent(nullptr) {}
	ProtocolMap(ProtocolMap* parent) : _parent(parent) {}

	void set_const_int(const string& name, size_t value) {
		_const_ints[name] = value;
	}

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
		if (_parent && !has_int(name)) return _parent->get_int(name);
		if (!has_int(name)) throw runtime_error("no key: " + name);
		return _ints.at(name);
	}

	string get_str(const string& name) const {
		if (_parent && !has_str(name)) return _parent->get_str(name);
		if (!has_str(name)) throw runtime_error("no key: " + name);
		return _strs.at(name);
	}

	bool can_int(const string& name) const {
		return (_parent && _parent->has_int(name)) || _ints.count(name);
	}

	bool can_str(const string& name) const {
		return (_parent && _parent->has_str(name)) || _strs.count(name);
	}

	bool has_int(const string& name) const {
		return _ints.count(name);
	}

	bool has_str(const string& name) const {
		return _strs.count(name);
	}

	static string prefix_array(const string& name, size_t i) {
		stringstream prefix_ss;
		prefix_ss << name << "_" << i << "_";
		return prefix_ss.str();
	}

	bool has_array(const string& name, size_t i) const {
		string prefix = prefix_array(name, i);
		const auto str_it = _strs.lower_bound(prefix);
		const auto int_it = _ints.lower_bound(prefix);
		if (str_it != _strs.end() &&
		    str_it->first.substr(0, prefix.length()) == prefix)
			return true;
		if (int_it != _ints.end() &&
		    int_it->first.substr(0, prefix.length()) == prefix)
			return true;
		return false;
	}

	ProtocolMap get_array(const string& name, size_t i) const {
		ProtocolMap sub;
		string prefix = prefix_array(name, i);
		auto str_it = _strs.lower_bound(prefix);
		auto int_it = _ints.lower_bound(prefix);
		while (str_it != _strs.end()) {
			if (str_it->first.substr(0, prefix.length()) == prefix) {
				sub.set_str(str_it->first.substr(prefix.length()),
					    str_it->second);
			}
			++str_it;
		}
		while (int_it != _ints.end()) {
			if (int_it->first.substr(0, prefix.length()) == prefix) {
				sub.set_int(int_it->first.substr(prefix.length()),
					    int_it->second);
			}
			++int_it;
		}
		return sub;
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
		ss << "#include <cstdint>" << endl;
		ss << "#include <string>" << endl;
		ss << "#include <vector>" << endl << endl;
		ss << "#include \"../protocol_library.h\"" << endl;
		ss << "#include \"../protocol_map.h\"" << endl;
		ss << "using namespace std;" << endl;
		ss << "namespace protodecode {" << endl;
		ss << "class ProtocolState;" << endl;
		ss << "namespace pmaps {" << endl << endl;
		ss << export_class(name, 0) << endl;
		ss << "}  // namespace pmaps" << endl;
		ss << "}  // namespace protodecode" << endl << endl;
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
		ss << name << "_t(const ProtocolMap& pmap) { load(pmap); }" << endl;
		tab(ss, tab_depth - 1);
		ss << name << "_t(const string& data) {" << endl;
		tab(ss, tab_depth);
		ss << "_state.reset(data);" << endl;
		tab(ss, tab_depth);
		ss << "process(&_state);" << endl;
		tab(ss, tab_depth - 1);
		ss << "}" << endl << endl;

		tab(ss, tab_depth - 1);
		ss << name << "_t(ProtocolState* state) {" << endl;
		tab(ss, tab_depth);
		ss << "process(state);" << endl;
		tab(ss, tab_depth - 1);
		ss << "}" << endl << endl;

		tab(ss, tab_depth - 1);
		ss << "virtual ~" << name << "_t() {}" << endl;

		return ss.str();
	}

	string export_array_loaders(const string& name, size_t tab_depth) {
		stringstream ss;

		tab(ss, tab_depth);
		ss << "void load_" << name
		   << "_array(const ProtocolMap& pmap) {" << endl;
		++tab_depth;
		tab(ss, tab_depth);
		ss << "size_t i = 0;" << endl;
		tab(ss, tab_depth);
		ss << "while (true) {" << endl;
		++tab_depth;
		tab(ss, tab_depth);
		ss << "if (!pmap.has_array(\"" << name << "\", i)) break;" << endl;
		tab(ss, tab_depth);
		ss << "ProtocolMap element = pmap.get_array(\"" << name
		   << "\", i);" << endl;
		tab(ss, tab_depth);
		ss << "_" + name + ".push_back(" << name << "_t(element));"
		   << endl;
		tab(ss, tab_depth);
		ss << "++i;" << endl;
		tab(ss, tab_depth - 1);
		ss << "}" << endl;
		tab(ss, tab_depth - 2);
		ss << "}" << endl;

		return ss.str();
	}

	string export_functions(const string& name, size_t tab_depth) {
		stringstream ss;

		tab(ss, tab_depth);
		ss << "string pmap_name() const { return \"" << name << "\"; }" << endl;
		tab(ss, tab_depth);
		ss << "void process(ProtocolState* state) {" << endl;
		tab(ss, tab_depth + 1);
		ss << "state->prepare();" << endl;
		tab(ss, tab_depth + 1);
		ss << "Operator* op = ProtocolLibrary::_()->get(pmap_name());" << endl;
		tab(ss, tab_depth + 1);
		ss << "ProtocolMap pmap;" << endl;
		tab(ss, tab_depth + 1);
		ss << "op->process(&pmap, state);" << endl;
		tab(ss, tab_depth + 1);
		ss << "load(pmap);" << endl;
		tab(ss, tab_depth);
		ss << "}" << endl << endl;
		tab(ss, tab_depth);
		ss << "virtual void load(const ProtocolMap& pmap) {" << endl;
		++tab_depth;
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
		for (auto& x : _arrays) {
			tab(ss, tab_depth);
			ss << "load_" << x.first << "_array(pmap);" << endl;
		}
		--tab_depth;
		tab(ss, tab_depth);
		ss << "}" << endl;

		return ss.str();
	}


	string export_class(const string& name, size_t tab_depth) {
		stringstream ss;

		tab(ss, tab_depth);
		ss << "class " << name << "_t {" << endl;
		tab(ss, tab_depth);
		ss << "public:" << endl;
		++tab_depth;
		ss << export_constructor(name, tab_depth) << endl;
		ss << export_functions(name, tab_depth) << endl;
		for (auto &x : _arrays) {
			ss << export_array_loaders(x.first, tab_depth) << endl;
			ss << x.second.export_class(x.first, tab_depth);
		}
		for (auto& x : _const_ints) {
			tab(ss, tab_depth);
			ss << "static const size_t " << x.first << " = "
			   << x.second << ";" << endl;
		}
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
		for (auto& x : _arrays) {
			tab(ss, tab_depth);
			ss << "void add_" << x.first << "(const " << x.first
			   << "_t& val) { _" << x.first
			   << ".emplace_back(val); }" << endl;
			tab(ss, tab_depth);
			ss << "const vector<" << x.first << "_t>& " << x.first
			   << "() const { return _" << x.first << "; }"
			   << endl;
		}
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

		for (auto& x : _arrays) {
			tab(ss, tab_depth);
			ss << "vector<" << x.first << "_t> _" << x.first << ";" << endl;
		}
		tab(ss, tab_depth);
		ss << "ProtocolState _state;" << endl;

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

	ProtocolMap* _parent;

	map<string, size_t> _const_ints;
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
