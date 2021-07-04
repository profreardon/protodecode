#ifndef __PROTODECODE__OPERATORS__H__
#define __PROTODECODE__OPERATORS__H__

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

class ExpectOperator;
class AlignOperator;
class PaddingOperator;
class StringOperator;
class LrstringOperator;
class LpstringOperator;
class MathOperator;
class ProtocolOperator;
class U1Operator;
class U2Operator;
// class U3Operator;
class U4Operator;
class U8Operator;
class U16Operator;
class U32Operator;
class U64Operator;
class ProtocolLibrary;

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

	static Operator* create_operator(list<string>* tokens);

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
		_value = stoi(tokens->front(), nullptr, 0);
		tokens->pop_front();
		assert(tokens->size() > 2);
		_name = *(++(tokens->begin()));
	}

	void process(ProtocolMap* pm, ProtocolState* state) override {
		pm->set_expect(_name, _value);
	}
protected:
	size_t _value;
};

class AlignOperator : public Operator {
public:
	AlignOperator(list<string>* tokens) : Operator(tokens) {
	}

	void establish(list<string>* tokens) override {
		assert(tokens && tokens->size());
		_bytes = stoi(tokens->front(), nullptr, 0);
		_bytes >>= 3;
		tokens->pop_front();
	}

	void process(ProtocolMap* pm, ProtocolState* state) override {
		size_t remaining = _bytes;
		/* if aligned to 0, means rewind the frame to start */
		if (!remaining) {
			state->data_subpos = 0;
			return;
		}
		if (state->data_subpos) {
			state->data_subpos = 0;
			++(state->data_pos);
			--remaining;
		}
		while (remaining) {
			++(state->data_pos);
			--remaining;
		}
	}
protected:
	size_t _bytes;
};

class PaddingOperator : public Operator {
public:
	PaddingOperator(list<string>* tokens) {
		establish(tokens);
	}

	void establish(list<string>* tokens) override {
		assert(tokens && tokens->size());
		_bytes = stoi(tokens->front(), nullptr, 0);
		tokens->pop_front();
	}

	void process(ProtocolMap* pm, ProtocolState* state) override {
		size_t remaining = _bytes;
		assert(remaining);
		if (remaining > state->data.length() - state->data_pos) {
			stringstream ss;
			ss << "not enough packet for buffer "
			   << _bytes << " len is " << state->data.length()
			   << " at pos " << state->data_pos;
		}

		state->data_subpos = 0;
		while (remaining) {
			++(state->data_pos);
			--remaining;
		}
	}
protected:
	size_t _bytes;
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
		if (_type == "rmask") {
			size_t mask = 0;
			for (size_t i = 0; i < _immediate; ++i) {
				mask <<= 1;
				mask += 1;
			}
			val &= mask;
		}
		if (_type == "lmask") {
			size_t mask = 0;
			for (size_t i = 0; i < _immediate; ++i) {
				mask <<= 1;
				mask += 1;
			}
			mask = ~mask;
			val &= mask;
		}
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

class SequenceOperator : public Operator {
public:
	SequenceOperator(list<string>* tokens) {
		establish(tokens);
	}

	virtual void establish(list<string>* tokens) override {
		while (tokens->size()) {
			Operator *op = Operator::create_operator(tokens);
			if (!op) break;
			_operators.emplace_back(op);
		}
	}

	virtual void process(ProtocolMap* pmap, ProtocolState* state) override {
		for (auto &x : _operators) {
			state->trace();
			x->process(pmap, state);
			state->tidy();
			if (!state->ok()) {
				throw runtime_error("buffer overflow");
			}
		}
	}

protected:
	vector<unique_ptr<Operator>> _operators;
};

class CallOperator : public Operator {
public:
	CallOperator(list<string>* tokens) {
		establish(tokens);
	}

	virtual void establish(list<string>* tokens) override {
		assert(tokens && tokens->size());
		_protocol = tokens->front();
		tokens->pop_front();
		assert(tokens->size());
		_prefix = tokens->front();
		tokens->pop_front();
	}

	virtual void process(ProtocolMap* pmap, ProtocolState* state) override;

protected:
	string _protocol;
	string _prefix;
};

class ForOperator : public Operator {
public:
	ForOperator(list<string>* tokens) {
		establish(tokens);
	}

	virtual void establish(list<string>* tokens) override {
		assert(tokens && tokens->size());
		try {
			_immediate = stoi(tokens->front(), nullptr, 0);
		} catch (const std::invalid_argument& ia) {
			_key = tokens->front();
		}
		tokens->pop_front();
		assert(tokens->size());
		_prefix = tokens->front();
		tokens->pop_front();
		assert(tokens->size());
		_condition.reset(create_operator(tokens));
	}

	virtual void process(ProtocolMap* pmap, ProtocolState* state) override {
		if (!_key.empty()) {
			_immediate = pmap->get_int(_key);
		}
		size_t end = state->data_pos + _immediate;
		size_t i = 0;
		while (state->data_pos < end) {
			ProtocolMap submap;
			_condition->process(&submap, state);
			stringstream ss;
			ss << _prefix << "_" << i;
			pmap->merge(ss.str(), submap);
			++i;
		}
	}

protected:
	unique_ptr<Operator> _condition;
	string _key;
	string _prefix;
	size_t _immediate;

};

class ConditionOperator : public Operator {
public:
	ConditionOperator(const string& type, list<string>* tokens) {
		_type = type;
		establish(tokens);
	}

	virtual void establish(list<string>* tokens) override {
		assert(tokens && tokens->size());
		_key = tokens->front();
		tokens->pop_front();
		assert(tokens->size());
		try {
			_immediate = stoi(tokens->front(), nullptr, 0);
		} catch (const std::invalid_argument& ia) {
			_other_key = tokens->front();
		}
		tokens->pop_front();
		assert(tokens->size());
		_condition.reset(create_operator(tokens));
	}

	virtual void process(ProtocolMap* pmap, ProtocolState* state) override {
		size_t val = pmap->get_int(_key);
		if (!_other_key.empty())
		    _immediate = pmap->get_int(_other_key);
		bool result = false;
		if (_type == "eq" && val == _immediate) result = true;
		if (_type == "lt" && val < _immediate) result = true;
		if (_type == "le" && val <= _immediate) result = true;
		if (_type == "gt" && val > _immediate) result = true;
		if (_type == "ge" && val >= _immediate) result = true;
		if (_type == "ne" && val != _immediate) result = true;
		if (result) {
			_condition->process(pmap, state);
		}
	}

protected:
	string _type;
	unique_ptr<Operator> _condition;
	string _other_key;
	string _key;
	size_t _immediate;

};


}  // namespace protodecode

#endif  // __PROTODECODE__OPERATORS__H__
