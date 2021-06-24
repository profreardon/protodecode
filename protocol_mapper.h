#ifndef __PROTODECODE__PROTOCOL_MAPPER__H__
#define __PROTODECODE__PROTOCOL_MAPPER__H__

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

#include "protocol_map.h"
#include "protocol_state.h"

using namespace std;

namespace protodecode {

// TODO: have these bridge packetline

/* TODO: support construct:
u16 somelength
u8 type
if type 1
  blah
  blah
if type 2
  blah
for somelen
  u8 extenion_type
*/
// this will need to disambiguate the extensions into an array

// TODO: warn on namespace clang
// TODO: allow 0xff for expect values
// TODO: allow _ for ignore
// TODO: allow comments in pmap file
// TODO: embedable subheaders

/* TODO: create static map to keep protocols referenced by name */
class ProtocolMapper {
protected:
	class Operator {
	public:
		Operator() {}
		Operator(list<string>* tokens) {
			establish(tokens);
		}
		virtual ~Operator() {}
		virtual void establish(list<string>* tokens) {
			assert(tokens && tokens->size());
			_name = tokens->front();
			tokens->pop_front();
		}

		virtual void process(ProtocolMap* pm, ProtocolState* state) {}

	protected:
		// move the ux to here too, and make into one class
		static size_t read_int(size_t len, ProtocolState* state) {
			size_t ret;
			if (len == 1) {
				ret = *reinterpret_cast<const uint8_t*>(
				    state->data.c_str() + state->data_pos);
			} else if (len == 2) {
				ret = *reinterpret_cast<const uint16_t*>(
				    state->data.c_str() + state->data_pos);
				ret = ntohs(ret);
			} else if (len == 4) {
				ret = *reinterpret_cast<const uint32_t*>(
				    state->data.c_str() + state->data_pos);
				ret = ntohl(ret);
			} else {
				stringstream ss;
				ss << "invalid int length " << len;
				throw logic_error(ss.str());
			}
			state->data_pos += len;
			return ret;
		}

		string _name;
	};

	class ExpectOperator : public Operator {
	public:
		ExpectOperator(list<string>* tokens) : Operator(tokens) {
		}

		void establish(list<string>* tokens) override {
			assert(tokens && tokens->size());
			size_t value = stoi(tokens->front(), nullptr, 0);
			tokens->pop_front();
			assert(tokens->size() > 2);
			_name = *(++(tokens->begin()));
		}

		void process(ProtocolMap* pm, ProtocolState* state) override {
	// TODO
		}
	protected:
		size_t _value;
	};

	class MathOperator : public Operator {
	public:
		MathOperator(const string& type, list<string>* tokens) : _type(type) {
			    establish(tokens);
		}

		void establish(list<string>* tokens) override {
			assert(tokens && tokens->size());
			try {
				_immediate = stoi(tokens->front(), nullptr, 0);
			} catch (const std::invalid_argument& ia) {
				_other_key = tokens->front();
			}
			tokens->pop_front();
			assert(tokens->size());
			_key = tokens->front();
			tokens->pop_front();
			assert(tokens->size());
			_name = tokens->front();
			tokens->pop_front();
		}

		void process(ProtocolMap* pm, ProtocolState* state) override {
			size_t val = pm->get_int(_key);
			if (!_other_key.empty())
			    _immediate = pm->get_int(_other_key);
			if (_type == "mul") val *= _immediate;
			if (_type == "add") val += _immediate;
			if (_type == "sub") val -= _immediate;
			if (_type == "lsh") val <<= _immediate;
			if (_type == "rsh") val >>= _immediate;

			pm->set_int(_name, val);
		}

	protected:
		string _type;
		size_t _immediate;
		string _key;
		string _other_key;
	};

	class StringOperator : public Operator {
	public:
		StringOperator() {}
		StringOperator(list<string>* tokens) {
			establish(tokens);
		}

		void establish(list<string>* tokens) override {
			assert(tokens && tokens->size());
			_len = stoi(tokens->front(), nullptr, 0);
			tokens->pop_front();
			Operator::establish(tokens);
		}

		void process(ProtocolMap* pm, ProtocolState* state) override {
			if (state->data_pos + _len > state->data.length()) {
				stringstream ss;
				ss << "at " << state->data_pos
				   << " cannot get string len "
				   << _len << " for " << _name;
				throw logic_error(ss.str());
			}
			string value = string(state->data.substr(
				state->data_pos, _len));
			pm->set_str(_name, value);
			state->data_pos += _len;
		}

	protected:
		size_t _len;
	};

	class LpstringOperator : public StringOperator {
	public:
		LpstringOperator(list<string>* tokens) {
			establish(tokens);
		}

		void establish(list<string>* tokens) override {
			assert(tokens && tokens->size());
			_str_len = stoi(tokens->front(), nullptr, 0);
			tokens->pop_front();
			if (!_str_len || _str_len > 4) {
				stringstream ss;
				ss << "invalid string prefix length "
				   << _str_len << " for " << tokens->front();
				throw logic_error(ss.str());
			}
			Operator::establish(tokens);
		}

		void process(ProtocolMap* pm, ProtocolState* state) override {
			_len = read_int(_str_len, state);
			StringOperator::process(pm, state);
		}

	protected:
		string _ref;
		size_t _str_len;
	};

	class LrstringOperator : public StringOperator {
	public:
		LrstringOperator(list<string>* tokens) {
			establish(tokens);
		}

		void establish(list<string>* tokens) override {
			assert(tokens && tokens->size());
			_ref = stoi(tokens->front(), nullptr, 0);
			tokens->pop_front();
			Operator::establish(tokens);
		}

		void process(ProtocolMap* pm, ProtocolState* state) override {
			_len = pm->get_int(_ref);
			StringOperator::process(pm, state);
		}

	protected:
		string _ref;
	};

	class U1Operator : public Operator {
	public:
		U1Operator(list<string>* tokens) : Operator(tokens) {}

		void process(ProtocolMap* pm, ProtocolState* state) override {
			int mask = 1;
                        mask <<= (7 - state->data_subpos);
                        size_t value = state->data[state->data_pos] & mask;
                        value >>= (7 - state->data_subpos);
                        state->data_subpos += 1;
			pm->set_int(_name, value);
		}
	};

	class U2Operator : public Operator {
	public:
		U2Operator(list<string>* tokens) : Operator(tokens) {}

		void process(ProtocolMap* pm, ProtocolState* state) {
			int mask = 3;
                        mask <<= (6 - state->data_subpos);
                        size_t value = state->data[state->data_pos] & mask;
                        value >>= (6 - state->data_subpos);
                        state->data_subpos += 2;
			pm->set_int(_name, value);
		}
	};

	class U4Operator : public Operator {
	public:
		U4Operator(list<string>* tokens) : Operator(tokens) {}

		void process(ProtocolMap* pm, ProtocolState* state) override {
			int mask = 15;
                        mask <<= (4 - state->data_subpos);
                        size_t value = state->data[state->data_pos] & mask;
                        value >>= (4 - state->data_subpos);
                        state->data_subpos += 4;
			pm->set_int(_name, value);
		}
	};

	class U8Operator : public Operator {
	public:
		U8Operator(list<string>* tokens) : Operator(tokens) {}

		void process(ProtocolMap* pm, ProtocolState* state) override {
			cout << state->data << ' ' << (int)
			    state->data.c_str()[state->data_pos]
			    << " " << state->data_pos << endl;
			size_t value = *reinterpret_cast<const uint8_t*>(
				state->data.c_str() + state->data_pos);
			++(state->data_pos);
			pm->set_int(_name, value);
		}
	};

	class U16Operator : public Operator {
	public:
		U16Operator(list<string>* tokens) : Operator(tokens) {}

		void process(ProtocolMap* pm, ProtocolState* state) override {
			size_t value = *reinterpret_cast<const uint16_t*>(
				state->data.c_str() + state->data_pos);
			state->data_pos += 2;
			value = ntohs(value);
			pm->set_int(_name, value);
		}
	};

	class U32Operator : public Operator {
	public:
		U32Operator(list<string>* tokens) : Operator(tokens) {}

		void process(ProtocolMap* pm, ProtocolState* state) override {
			size_t value = *reinterpret_cast<const uint32_t*>(
				state->data.c_str() + state->data_pos);
			state->data_pos += 4;
			value = ntohl(value);
			pm->set_int(_name, value);
		}
	};

	class U64Operator : public Operator {
	public:
		U64Operator(list<string>* tokens) : Operator(tokens) {}

		void process(ProtocolMap* pm, ProtocolState* state) override {
			throw logic_error("need to implement htonll");
			size_t value = *reinterpret_cast<const uint64_t*>(
				state->data.c_str() + state->data_pos);
			state->data_pos += 8;
			value = ntohs(value);
			pm->set_int(_name, value);
		}
	};

	class OperatorSequence {
	public:
		virtual void process(ProtocolMap* pmap, ProtocolState* state) {
			for (auto &x : _operators) {
				x->process(&ret, state);
			}
		}

	protected:
		vector<unique_ptr<Operator>> _operators;
	};


public:
	ProtocolMapper() {
	}

	ProtocolMapper(const string& filename) {
		load(filename);
	}

	virtual ~ProtocolMapper() = default;

	virtual bool load(const string& filename) {
		string data;
		ifstream fin(filename);
		if (!fin.good()) return false;
		stringstream buffer;
                buffer << fin.rdbuf();
		if (!fin.good()) return false;
		data = buffer.str();
		return set_scheme(data);
	}

	virtual Operator* create_operator(list<string>* tokens) {
		assert(tokens && tokens->size());
		string name = tokens->front();
		tokens->pop_front();
		cout << "name " << name << endl;

		if (name == "expect") return new ExpectOperator(tokens);
//		if (name == "align") return new AlignOperator(tokens);
	//	if (name == "padding") return new PaddingOperator(tokens);
		if (name == "string") return new StringOperator(tokens);
		if (name == "lrstring") return new LrstringOperator(tokens);
		if (name == "lpstring") return new LpstringOperator(tokens);
		if (name == "mul") return new MathOperator("mul", tokens);
		if (name == "add") return new MathOperator("add", tokens);
		if (name == "sub") return new MathOperator("sub", tokens);
		if (name == "lsh") return new MathOperator("lsh", tokens);
		if (name == "rsh") return new MathOperator("rsh", tokens);
		if (name == "u1") return new U1Operator(tokens);
		if (name == "bool") return new U1Operator(tokens);
		if (name == "u2") return new U2Operator(tokens);
		if (name == "u4") return new U4Operator(tokens);
		if (name == "u8") return new U8Operator(tokens);
		if (name == "u16") return new U16Operator(tokens);
		if (name == "u32") return new U32Operator(tokens);
		if (name == "u64") return new U64Operator(tokens);
		throw logic_error("unknown name: " + name);
	}

	virtual bool set_scheme(const string& scheme) {
		list<string> tokens = tokenize(scheme);
		while (tokens.size()) {
			_operators.emplace_back(create_operator(&tokens));
		}
		return true;
	}

	virtual ProtocolMap process(const string& data) {
		ProtocolState state(data);
		return process(&state);
	}

	virtual ProtocolMap process(ProtocolState* state) {
		ProtocolMap ret;
		for (auto &x : _operators) {
			x->process(&ret, state);
		}
		return ret;
	}


	/*

		size_t pos = 0;
		while (pos < _tokens.size()) {
			_tokens.push_back(create_token(&pos));
			if (name == "expect") pos += 2;
			else if (name == "align") pos += 2;
			else if (name == "padding") pos += 2;
			else if (name == "string") pos += 3;
			else if (name == "lrstring") pos += 2;
			else if (name == "lpstring") pos += 3;
			else if (name == "mul") pos += 4;
			else if (name == "add") pos += 4;
			else if (name == "sub") pos += 4;
			else if (name == "u1") pos += 2;
			else if (name == "u2") pos += 2;
			else if (name == "u4") pos += 2;
			else if (name == "u8") pos += 2;
			else if (name == "u16") pos += 2;
			else if (name == "u32") pos += 2;
			else if (name == "u64") pos += 2;
			else return false;
		}
		if (pos != _tokens.size()) return false;
		return true;
	}

	virtual size_t process(const string& data,
			       ProtocolMap* pmap) {
		return process(data, 0, pmap);
	}

	virtual size_t process(const string& data,
			       size_t startpos,
			       ProtocolMap* pmap) {
		map<string, uint64_t>* ints = &(pmap->ints);
		map<string, string>* strs = &(pmap->strs);
		assert(ints);
		assert(strs);
		size_t data_pos = startpos;
		size_t data_subpos = 0;
		size_t pos = 0;
		while (pos < _tokens.size()) {
			string name = _tokens[pos];
			if (name == "expect") {
				++pos;
				(*ints)[_tokens[pos + 2]] =
					stoi(_tokens[pos]);
				++pos;
				continue;
			}
			else if (name == "align") {
				++pos;
				size_t bytes = stoi(_tokens[pos]);
				bytes >>= 3;
				if (data_subpos) {
					data_subpos = 0;
					++data_pos;
					--bytes;
				}
				while (bytes) {
					--bytes;
					++data_pos;
				}
				++pos;
				continue;
			}
			else if (name == "padding") {
				++pos;
				size_t bytes = stoi(_tokens[pos]);
				data_subpos = 0;
				while (bytes) {
					--bytes;
					++data_pos;
				}
				++pos;
				continue;

			}
			else if (name == "string") {
				++pos;
				size_t len = stoi(_tokens[pos]);
				++pos;
				string key = _tokens[pos];
				if (data_pos + len > data.length()) {
					throw logic_error(Logger::stringify(
						"at % cannot get string len % data %",
						data_pos, len, data));
				}
				string value = string(data.substr(data_pos,
								  len));
				(*strs)[key] = value;
				data_pos += len;
				++pos;
				continue;
			}
			else if (name == "lrstring") {
				++pos;
				string ref = _tokens[pos];
				if (!ints->count(ref))
					throw logic_error(Logger::stringify(
						"no length reference % %", ref,
						data));
				size_t len = (*ints)[ref];
				++pos;
				string key = _tokens[pos];
				if (data_pos + len > data.length()) {
					throw logic_error(Logger::stringify(
						"at % cannot get string len % data %",
						data_pos, len, data.length()));
				}
				string value = string(data.substr(data_pos,
								  len));
				(*strs)[key] = value;
				data_pos += len;
				++pos;
				continue;
			}
			else if (name == "lpstring") {
				++pos;
				size_t len_len = stoi(_tokens[pos]);
				++pos;
				string key = _tokens[pos];
				size_t len;
				if (len_len == 1) {
					len = *reinterpret_cast<const uint8_t*>(
						data.c_str() + data_pos);
				} else if (len_len == 2) {
					len = *reinterpret_cast<const uint16_t*>(
						data.c_str() + data_pos);
					len = ntohs(len);
				} else if (len_len == 4) {
					len = *reinterpret_cast<const uint32_t*>(
						data.c_str() + data_pos);
					len = ntohl(len);
				} else {
					throw logic_error(Logger::stringify(
						"not implemented lpstring len %",
						len_len));
				}
				data_pos += len_len;
				if (data_pos + len > data.length()) {
					throw logic_error(Logger::stringify(
						"at % cannot get string len % data %",
						pos, len, data));
				}
				cout << len << endl;
				string value = string(data.substr(data_pos,
								  len));
				(*strs)[key] = value;
				data_pos += len;
				++pos;
				continue;
			} else if (name == "mul") {
				++pos;
				string old = _tokens[pos];
				++pos;
				size_t scale = stoi(_tokens[pos]);
				++pos;
				string key = _tokens[pos];
				if (!ints->count(old))
					throw logic_error(
						Logger::stringify("no key % for %",
								  old, key));
				(*ints)[key] = scale * (*ints)[old];
				++pos;
				continue;
			} else if (name == "add") {
				++pos;
				string old = _tokens[pos];
				++pos;
				size_t amount = stoi(_tokens[pos]);
				++pos;
				string key = _tokens[pos];
				if (!ints->count(old))
					throw logic_error(
						Logger::stringify("no key % for %",
								  old, key));
				(*ints)[key] = amount + (*ints)[old];
				++pos;
				continue;
			} else if (name == "sub") {
				++pos;
				string old = _tokens[pos];
				++pos;
				size_t amount = stoi(_tokens[pos]);
				++pos;
				string key = _tokens[pos];
				if (!ints->count(old))
					throw logic_error(
						Logger::stringify("no key % for %",
								  old, key));
				(*ints)[key] = (*ints)[old] - amount;
				++pos;
				continue;
			}


			// remaining types are for ints only.
			size_t value;
			++pos;
			string key = _tokens[pos];
			if (name == "u1") {
				int mask = 1;
				mask <<= (7 - data_subpos);
				value = data[data_pos] & mask;
				value >>= (7 - data_subpos);
				cout << mask << " " << value << " "
				     << data_subpos <<endl;
				++data_subpos;
			}
			else if (name == "u2") {
				int mask = 3;
				mask <<= (6 - data_subpos);
				value = data[data_pos] & mask;
				value >>= (6 - data_subpos);
				data_subpos += 2;

			}
			else if (name == "u4") {
				int mask = 15;
				mask <<= (4 - data_subpos);
				value = data[data_pos] & mask;
				value >>= (4 - data_subpos);
				data_subpos += 4;
			}
			else if (name == "u8") {
				assert(!data_subpos);
				value = *reinterpret_cast<const uint8_t*>(
					data.c_str() + data_pos);
				++data_pos;
			}
			else if (name == "u16") {
				assert(!data_subpos);
				value = *reinterpret_cast<const uint16_t*>(
					data.c_str() + data_pos);
				value = ntohs(value);
				data_pos += 2;
			}
			else if (name == "u32") {
				assert(!data_subpos);
				value = *reinterpret_cast<const uint32_t*>(
					data.c_str() + data_pos);
				value = ntohl(value);
				data_pos += 4;
			}
			else if (name == "u64") {
				assert(!data_subpos);
				throw logic_error("implement ntohll");
				value = *reinterpret_cast<const uint64_t*>(
					data.c_str() + data_pos);
				//value = ntohll(value);
				data_pos += 8;

			} else {
				throw logic_error("bad name " + name);
			}
			++pos;
			if (data_subpos == 8) {
				data_subpos = 0;
				++data_pos;
			}
			if (data_subpos > 8) {
				throw logic_error("subbits cross byte barrier");
			}
			if (key != "_" && ints->count(key)) {
				if ((*ints)[key] != value) {
					Logger::error("not valid payload % % %",
						      key, value,
						      (*ints)[key]);
					break;
				}
			}
			(*ints)[key] = value;
		}
		pmap->len = data_pos - startpos;
		return data_pos;
	}*/

protected:
	list<string> tokenize(const string& s) {
		list<string> tokens;
		stringstream ss;
		ss << s;
		while (ss.good()) {
			string token;
			ss >> token;
			tokens.emplace_back(token);
		}
		return tokens;
	}

	vector<unique_ptr<Operator>> _operators;
};

}  // namespace protodecode

#endif  // __PROTODECODE__PROTOCOL_MAPPER__H__
