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

#include "operators.h"
#include "protocol_map.h"
#include "protocol_state.h"

using namespace std;

namespace protodecode {

// TODO: have <u8 these bridge packetline

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
// TODO: allow comments in pmap file
// TODO: embedable subheaders
// TODO: lmask, rmask

class ProtocolMapper {
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

	virtual bool set_scheme(const string& scheme) {
		list<string> tokens = tokenize(scheme);
		_operator_seq.set_scheme(&tokens);
				return true;
	}

	virtual ProtocolMap process(const string& data) {
		ProtocolState state(data);
		return process(&state);
	}

	virtual ProtocolMap process(ProtocolState* state) {
		ProtocolMap ret;
		_operator_seq.process(&ret, state);
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
			if (!token.empty()) tokens.emplace_back(token);
		}
		return tokens;
	}

	OperatorSequence _operator_seq;
};

}  // namespace protodecode

#endif  // __PROTODECODE__PROTOCOL_MAPPER__H__
