#ifndef __PROTODECODE__PMAPS__dns__H__
#define __PROTODECODE__PMAPS__dns__H__

#include <cstdint>
#include <string>
#include <vector>

#include "../protocol_library.h"
#include "../protocol_map.h"
using namespace std;
namespace protodecode {
class ProtocolState;
namespace pmaps {

class dns_t {
public:
	dns_t() {
		_init_answer_authenticated = false;
		_init_authoritative = false;
		_init_non_authenticated = false;
		_init_num_additional = false;
		_init_num_answers = false;
		_init_num_authorities = false;
		_init_num_questions = false;
		_init_op = false;
		_init_qr = false;
		_init_ra = false;
		_init_rd = false;
		_init_reply_code = false;
		_init_transaction_id = false;
		_init_truncated = false;
		_init_zero = false;
	}

	dns_t(const ProtocolMap& pmap) { load(pmap); }
	dns_t(const string& data) {
		_state.reset(data);
		process(&_state);
	}

	dns_t(ProtocolState* state) {
		process(state);
	}

	virtual ~dns_t() {}

	static string pmap_name() { return "dns"; }
	void process(ProtocolState* state) {
		state->prepare();
		Operator* op = ProtocolLibrary::_()->load(pmap_name(), pmap_data());
		ProtocolMap pmap;
		op->process(&pmap, state);
		load(pmap);
	}

	virtual void load(const ProtocolMap& pmap) {
		if (pmap.has_int("answer_authenticated")) set_answer_authenticated(pmap.get_int("answer_authenticated"));
		if (pmap.has_int("authoritative")) set_authoritative(pmap.get_int("authoritative"));
		if (pmap.has_int("non_authenticated")) set_non_authenticated(pmap.get_int("non_authenticated"));
		if (pmap.has_int("num_additional")) set_num_additional(pmap.get_int("num_additional"));
		if (pmap.has_int("num_answers")) set_num_answers(pmap.get_int("num_answers"));
		if (pmap.has_int("num_authorities")) set_num_authorities(pmap.get_int("num_authorities"));
		if (pmap.has_int("num_questions")) set_num_questions(pmap.get_int("num_questions"));
		if (pmap.has_int("op")) set_op(pmap.get_int("op"));
		if (pmap.has_int("qr")) set_qr(pmap.get_int("qr"));
		if (pmap.has_int("ra")) set_ra(pmap.get_int("ra"));
		if (pmap.has_int("rd")) set_rd(pmap.get_int("rd"));
		if (pmap.has_int("reply_code")) set_reply_code(pmap.get_int("reply_code"));
		if (pmap.has_int("transaction_id")) set_transaction_id(pmap.get_int("transaction_id"));
		if (pmap.has_int("truncated")) set_truncated(pmap.get_int("truncated"));
		if (pmap.has_int("zero")) set_zero(pmap.get_int("zero"));
		load_additional_array(pmap);
		load_answers_array(pmap);
		load_authorities_array(pmap);
		load_questions_array(pmap);
	}

	void load_additional_array(const ProtocolMap& pmap) {
		size_t i = 0;
		while (true) {
			if (!pmap.has_array("additional", i)) break;
			ProtocolMap element = pmap.get_array("additional", i);
			_additional.push_back(additional_t(element));
			++i;
		}
	}

	class additional_t {
	public:
		additional_t() {
		}

		additional_t(const ProtocolMap& pmap) { load(pmap); }
		additional_t(const string& data) {
			_state.reset(data);
			process(&_state);
		}

		additional_t(ProtocolState* state) {
			process(state);
		}

		virtual ~additional_t() {}

		static string pmap_name() { return "additional"; }
		void process(ProtocolState* state) {
			state->prepare();
			Operator* op = ProtocolLibrary::_()->load(pmap_name(), pmap_data());
			ProtocolMap pmap;
			op->process(&pmap, state);
			load(pmap);
		}

		virtual void load(const ProtocolMap& pmap) {
		}

	protected:
		ProtocolState _state;
	};
	void load_answers_array(const ProtocolMap& pmap) {
		size_t i = 0;
		while (true) {
			if (!pmap.has_array("answers", i)) break;
			ProtocolMap element = pmap.get_array("answers", i);
			_answers.push_back(answers_t(element));
			++i;
		}
	}

	class answers_t {
	public:
		answers_t() {
			_init_dnsclass = false;
			_init_ttl = false;
			_init_type = false;
			_init_ip = false;
			_init_name = false;
		}

		answers_t(const ProtocolMap& pmap) { load(pmap); }
		answers_t(const string& data) {
			_state.reset(data);
			process(&_state);
		}

		answers_t(ProtocolState* state) {
			process(state);
		}

		virtual ~answers_t() {}

		static string pmap_name() { return "answers"; }
		void process(ProtocolState* state) {
			state->prepare();
			Operator* op = ProtocolLibrary::_()->load(pmap_name(), pmap_data());
			ProtocolMap pmap;
			op->process(&pmap, state);
			load(pmap);
		}

		virtual void load(const ProtocolMap& pmap) {
			if (pmap.has_int("dnsclass")) set_dnsclass(pmap.get_int("dnsclass"));
			if (pmap.has_int("ttl")) set_ttl(pmap.get_int("ttl"));
			if (pmap.has_int("type")) set_type(pmap.get_int("type"));
			if (pmap.has_str("ip")) set_ip(pmap.get_str("ip"));
			if (pmap.has_str("name")) set_name(pmap.get_str("name"));
		}

		void set_dnsclass(size_t val) {
			_init_dnsclass = true;
			_dnsclass = val;
		}
		size_t dnsclass() const {
			if (!_init_dnsclass) throw runtime_error("dnsclass not initialized");
			return _dnsclass;
		}
		bool has_dnsclass() const { return _init_dnsclass; }
		void set_ttl(size_t val) {
			_init_ttl = true;
			_ttl = val;
		}
		size_t ttl() const {
			if (!_init_ttl) throw runtime_error("ttl not initialized");
			return _ttl;
		}
		bool has_ttl() const { return _init_ttl; }
		void set_type(size_t val) {
			_init_type = true;
			_type = val;
		}
		size_t type() const {
			if (!_init_type) throw runtime_error("type not initialized");
			return _type;
		}
		bool has_type() const { return _init_type; }
		void set_ip(const string& val) {
			_init_ip = true;
			_ip = val;
		}
		string ip() const {
			if (!_init_ip) throw runtime_error("ip not initialized");
			return _ip;
		}
		bool has_ip() const { return _init_ip; }
		void set_name(const string& val) {
			_init_name = true;
			_name = val;
		}
		string name() const {
			if (!_init_name) throw runtime_error("name not initialized");
			return _name;
		}
		bool has_name() const { return _init_name; }
	protected:
		bool _init_dnsclass;
		size_t _dnsclass;
		bool _init_ttl;
		size_t _ttl;
		bool _init_type;
		size_t _type;
		bool _init_ip;
		string _ip;
		bool _init_name;
		string _name;
		ProtocolState _state;
	};
	void load_authorities_array(const ProtocolMap& pmap) {
		size_t i = 0;
		while (true) {
			if (!pmap.has_array("authorities", i)) break;
			ProtocolMap element = pmap.get_array("authorities", i);
			_authorities.push_back(authorities_t(element));
			++i;
		}
	}

	class authorities_t {
	public:
		authorities_t() {
			_init_dnsclass = false;
			_init_ttl = false;
			_init_type = false;
			_init_name = false;
			_init_nameserver = false;
		}

		authorities_t(const ProtocolMap& pmap) { load(pmap); }
		authorities_t(const string& data) {
			_state.reset(data);
			process(&_state);
		}

		authorities_t(ProtocolState* state) {
			process(state);
		}

		virtual ~authorities_t() {}

		static string pmap_name() { return "authorities"; }
		void process(ProtocolState* state) {
			state->prepare();
			Operator* op = ProtocolLibrary::_()->load(pmap_name(), pmap_data());
			ProtocolMap pmap;
			op->process(&pmap, state);
			load(pmap);
		}

		virtual void load(const ProtocolMap& pmap) {
			if (pmap.has_int("dnsclass")) set_dnsclass(pmap.get_int("dnsclass"));
			if (pmap.has_int("ttl")) set_ttl(pmap.get_int("ttl"));
			if (pmap.has_int("type")) set_type(pmap.get_int("type"));
			if (pmap.has_str("name")) set_name(pmap.get_str("name"));
			if (pmap.has_str("nameserver")) set_nameserver(pmap.get_str("nameserver"));
		}

		void set_dnsclass(size_t val) {
			_init_dnsclass = true;
			_dnsclass = val;
		}
		size_t dnsclass() const {
			if (!_init_dnsclass) throw runtime_error("dnsclass not initialized");
			return _dnsclass;
		}
		bool has_dnsclass() const { return _init_dnsclass; }
		void set_ttl(size_t val) {
			_init_ttl = true;
			_ttl = val;
		}
		size_t ttl() const {
			if (!_init_ttl) throw runtime_error("ttl not initialized");
			return _ttl;
		}
		bool has_ttl() const { return _init_ttl; }
		void set_type(size_t val) {
			_init_type = true;
			_type = val;
		}
		size_t type() const {
			if (!_init_type) throw runtime_error("type not initialized");
			return _type;
		}
		bool has_type() const { return _init_type; }
		void set_name(const string& val) {
			_init_name = true;
			_name = val;
		}
		string name() const {
			if (!_init_name) throw runtime_error("name not initialized");
			return _name;
		}
		bool has_name() const { return _init_name; }
		void set_nameserver(const string& val) {
			_init_nameserver = true;
			_nameserver = val;
		}
		string nameserver() const {
			if (!_init_nameserver) throw runtime_error("nameserver not initialized");
			return _nameserver;
		}
		bool has_nameserver() const { return _init_nameserver; }
	protected:
		bool _init_dnsclass;
		size_t _dnsclass;
		bool _init_ttl;
		size_t _ttl;
		bool _init_type;
		size_t _type;
		bool _init_name;
		string _name;
		bool _init_nameserver;
		string _nameserver;
		ProtocolState _state;
	};
	void load_questions_array(const ProtocolMap& pmap) {
		size_t i = 0;
		while (true) {
			if (!pmap.has_array("questions", i)) break;
			ProtocolMap element = pmap.get_array("questions", i);
			_questions.push_back(questions_t(element));
			++i;
		}
	}

	class questions_t {
	public:
		questions_t() {
			_init_dnsclass = false;
			_init_type = false;
			_init_name = false;
		}

		questions_t(const ProtocolMap& pmap) { load(pmap); }
		questions_t(const string& data) {
			_state.reset(data);
			process(&_state);
		}

		questions_t(ProtocolState* state) {
			process(state);
		}

		virtual ~questions_t() {}

		static string pmap_name() { return "questions"; }
		void process(ProtocolState* state) {
			state->prepare();
			Operator* op = ProtocolLibrary::_()->load(pmap_name(), pmap_data());
			ProtocolMap pmap;
			op->process(&pmap, state);
			load(pmap);
		}

		virtual void load(const ProtocolMap& pmap) {
			if (pmap.has_int("dnsclass")) set_dnsclass(pmap.get_int("dnsclass"));
			if (pmap.has_int("type")) set_type(pmap.get_int("type"));
			if (pmap.has_str("name")) set_name(pmap.get_str("name"));
		}

		void set_dnsclass(size_t val) {
			_init_dnsclass = true;
			_dnsclass = val;
		}
		size_t dnsclass() const {
			if (!_init_dnsclass) throw runtime_error("dnsclass not initialized");
			return _dnsclass;
		}
		bool has_dnsclass() const { return _init_dnsclass; }
		void set_type(size_t val) {
			_init_type = true;
			_type = val;
		}
		size_t type() const {
			if (!_init_type) throw runtime_error("type not initialized");
			return _type;
		}
		bool has_type() const { return _init_type; }
		void set_name(const string& val) {
			_init_name = true;
			_name = val;
		}
		string name() const {
			if (!_init_name) throw runtime_error("name not initialized");
			return _name;
		}
		bool has_name() const { return _init_name; }
	protected:
		bool _init_dnsclass;
		size_t _dnsclass;
		bool _init_type;
		size_t _type;
		bool _init_name;
		string _name;
		ProtocolState _state;
	};
	static string pmap_data() { return R"(u16 transaction_id
u1 qr
u4 op
u1 authoritative
u1 truncated
u1 rd
u1 ra
u1 zero
u1 answer_authenticated
u1 non_authenticated
u4 reply_code
u16 num_questions
u16 num_answers
u16 num_authorities
u16 num_additional
for num_questions questions {
	dnsstring name
	u16 type
	u16 dnsclass
}
for num_answers answers {
	dnsstring name
	u16 type
	u16 dnsclass
	u32 ttl
	lpstring 2 ip
}
for num_authorities authorities {
	dnsstring name
	u16 type
        u16 dnsclass
        u32 ttl
	dnsstring nameserver
}
for num_additional additional {
}

)"; }
	void set_answer_authenticated(size_t val) {
		_init_answer_authenticated = true;
		_answer_authenticated = val;
	}
	size_t answer_authenticated() const {
		if (!_init_answer_authenticated) throw runtime_error("answer_authenticated not initialized");
		return _answer_authenticated;
	}
	bool has_answer_authenticated() const { return _init_answer_authenticated; }
	void set_authoritative(size_t val) {
		_init_authoritative = true;
		_authoritative = val;
	}
	size_t authoritative() const {
		if (!_init_authoritative) throw runtime_error("authoritative not initialized");
		return _authoritative;
	}
	bool has_authoritative() const { return _init_authoritative; }
	void set_non_authenticated(size_t val) {
		_init_non_authenticated = true;
		_non_authenticated = val;
	}
	size_t non_authenticated() const {
		if (!_init_non_authenticated) throw runtime_error("non_authenticated not initialized");
		return _non_authenticated;
	}
	bool has_non_authenticated() const { return _init_non_authenticated; }
	void set_num_additional(size_t val) {
		_init_num_additional = true;
		_num_additional = val;
	}
	size_t num_additional() const {
		if (!_init_num_additional) throw runtime_error("num_additional not initialized");
		return _num_additional;
	}
	bool has_num_additional() const { return _init_num_additional; }
	void set_num_answers(size_t val) {
		_init_num_answers = true;
		_num_answers = val;
	}
	size_t num_answers() const {
		if (!_init_num_answers) throw runtime_error("num_answers not initialized");
		return _num_answers;
	}
	bool has_num_answers() const { return _init_num_answers; }
	void set_num_authorities(size_t val) {
		_init_num_authorities = true;
		_num_authorities = val;
	}
	size_t num_authorities() const {
		if (!_init_num_authorities) throw runtime_error("num_authorities not initialized");
		return _num_authorities;
	}
	bool has_num_authorities() const { return _init_num_authorities; }
	void set_num_questions(size_t val) {
		_init_num_questions = true;
		_num_questions = val;
	}
	size_t num_questions() const {
		if (!_init_num_questions) throw runtime_error("num_questions not initialized");
		return _num_questions;
	}
	bool has_num_questions() const { return _init_num_questions; }
	void set_op(size_t val) {
		_init_op = true;
		_op = val;
	}
	size_t op() const {
		if (!_init_op) throw runtime_error("op not initialized");
		return _op;
	}
	bool has_op() const { return _init_op; }
	void set_qr(size_t val) {
		_init_qr = true;
		_qr = val;
	}
	size_t qr() const {
		if (!_init_qr) throw runtime_error("qr not initialized");
		return _qr;
	}
	bool has_qr() const { return _init_qr; }
	void set_ra(size_t val) {
		_init_ra = true;
		_ra = val;
	}
	size_t ra() const {
		if (!_init_ra) throw runtime_error("ra not initialized");
		return _ra;
	}
	bool has_ra() const { return _init_ra; }
	void set_rd(size_t val) {
		_init_rd = true;
		_rd = val;
	}
	size_t rd() const {
		if (!_init_rd) throw runtime_error("rd not initialized");
		return _rd;
	}
	bool has_rd() const { return _init_rd; }
	void set_reply_code(size_t val) {
		_init_reply_code = true;
		_reply_code = val;
	}
	size_t reply_code() const {
		if (!_init_reply_code) throw runtime_error("reply_code not initialized");
		return _reply_code;
	}
	bool has_reply_code() const { return _init_reply_code; }
	void set_transaction_id(size_t val) {
		_init_transaction_id = true;
		_transaction_id = val;
	}
	size_t transaction_id() const {
		if (!_init_transaction_id) throw runtime_error("transaction_id not initialized");
		return _transaction_id;
	}
	bool has_transaction_id() const { return _init_transaction_id; }
	void set_truncated(size_t val) {
		_init_truncated = true;
		_truncated = val;
	}
	size_t truncated() const {
		if (!_init_truncated) throw runtime_error("truncated not initialized");
		return _truncated;
	}
	bool has_truncated() const { return _init_truncated; }
	void set_zero(size_t val) {
		_init_zero = true;
		_zero = val;
	}
	size_t zero() const {
		if (!_init_zero) throw runtime_error("zero not initialized");
		return _zero;
	}
	bool has_zero() const { return _init_zero; }
	void add_additional(const additional_t& val) { _additional.emplace_back(val); }
	const vector<additional_t>& additional() const { return _additional; }
	void add_answers(const answers_t& val) { _answers.emplace_back(val); }
	const vector<answers_t>& answers() const { return _answers; }
	void add_authorities(const authorities_t& val) { _authorities.emplace_back(val); }
	const vector<authorities_t>& authorities() const { return _authorities; }
	void add_questions(const questions_t& val) { _questions.emplace_back(val); }
	const vector<questions_t>& questions() const { return _questions; }
protected:
	bool _init_answer_authenticated;
	size_t _answer_authenticated;
	bool _init_authoritative;
	size_t _authoritative;
	bool _init_non_authenticated;
	size_t _non_authenticated;
	bool _init_num_additional;
	size_t _num_additional;
	bool _init_num_answers;
	size_t _num_answers;
	bool _init_num_authorities;
	size_t _num_authorities;
	bool _init_num_questions;
	size_t _num_questions;
	bool _init_op;
	size_t _op;
	bool _init_qr;
	size_t _qr;
	bool _init_ra;
	size_t _ra;
	bool _init_rd;
	size_t _rd;
	bool _init_reply_code;
	size_t _reply_code;
	bool _init_transaction_id;
	size_t _transaction_id;
	bool _init_truncated;
	size_t _truncated;
	bool _init_zero;
	size_t _zero;
	vector<additional_t> _additional;
	vector<answers_t> _answers;
	vector<authorities_t> _authorities;
	vector<questions_t> _questions;
	ProtocolState _state;
};

}  // namespace pmaps
}  // namespace protodecode

#endif  // __PROTODECODE__PMAPS__dns__H__

