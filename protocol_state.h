#ifndef __PROTODECODE__PROTOCOL_STATE__H__
#define __PROTODECODE__PROTOCOL_STATE__H__

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

class ProtocolState {
public:
	ProtocolState() : data_pos(0), data_subpos(0) {}
	ProtocolState(istream& in) {
		stringstream ss;
		ss << in.rdbuf();
		reset(ss.str());
	}

	ProtocolState(const string& init_data) {
		reset(init_data);
	}

	virtual void reset(const string& init_data) {
		data = init_data;
		data_pos = 0;
		data_subpos = 0;
	}

	virtual void prepare() {
		assert(!data_subpos);
		data_startoff = data_pos;
	}

	virtual size_t peek_int() const {
		size_t ret = *reinterpret_cast<const uint32_t*>(data.c_str() + data_pos);
		ret = ntohl(ret);
		return ret;
	}

	virtual size_t read_int() {
		size_t ret = peek_int();
		data_pos += 4;
		return ret;
	}

	virtual size_t peek_short() const {
		return peek_short(data_pos);
	}

	virtual size_t peek_short(size_t pos) const {
		size_t ret = *reinterpret_cast<const uint16_t*>(data.c_str() + pos);
		ret = ntohs(ret);
		return ret;
	}

	virtual size_t read_short() {
		size_t ret = peek_short();
		data_pos += 2;
		return ret;
	}

	virtual size_t peek_byte() const {
		return peek_byte(data_pos);
	}

	virtual size_t peek_byte(size_t pos) const {
		return *reinterpret_cast<const uint8_t*>(data.c_str() + pos);
	}

	virtual size_t read_byte() {
		size_t ret = peek_byte();
		++data_pos;
		return ret;
	}

	virtual size_t relative_offset(size_t where) {
		return data_startoff + where;
	}

	virtual string remaining() {
		size_t where = data_pos;
		data_pos = data.length();
		return data.substr(where);
	}


	string data;
	size_t data_pos;
	size_t data_subpos;
	size_t data_startoff;

	virtual void tidy() {
		if (data_subpos == 8) {
			data_subpos = 0;
			++data_pos;
		}
	}

	virtual bool has_more() {
		assert (data_subpos < 8);
		if (data_pos >= data.length()) return false;
		return true;
	}

	virtual bool ok() {
		assert (data_subpos < 8);
		if (data_pos > data.length()) return false;
		return true;
	}

	virtual void trace() {
		cout << "protocol state: pos="
		     << data_pos << " subpos=" << data_subpos << endl;
	}
};

}  // namespace protodecode

#endif  // __PROTODECODE__PROTOCOL_STATE__H__
