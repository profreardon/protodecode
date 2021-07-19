#ifndef __PROTODECODE__PMAPS__pcap__H__
#define __PROTODECODE__PMAPS__pcap__H__

#include <cstdint>
#include <string>
#include <vector>

#include "../protocol_library.h"

#include "../tiny_timer.h"
#include "../protocol_map.h"
using namespace std;
using namespace ib;
namespace protodecode {
class ProtocolState;
namespace pmaps {

class pcap_t {
public:
	pcap_t() {
		_init_magic = false;
		_init_major = false;
		_init_minor = false;
		_init_network = false;
		_init_sigfigs = false;
		_init_snaplen = false;
		_init_thiszone = false;
	}

	pcap_t(const ProtocolMap& pmap) { load(pmap); }
	pcap_t(const string& data) {
		_state.reset(data);
		process(&_state);
	}

	pcap_t(ProtocolState* state) {
		process(state);
	}

	virtual ~pcap_t() {}

	static string pmap_name() { return "pcap"; }
	void process(ProtocolState* state) {
		state->prepare();
		Operator* op = ProtocolLibrary::_()->load(pmap_name(), pmap_data());
		ProtocolMap pmap;
		op->process(&pmap, state);
		load(pmap);
	}

	virtual void load(const ProtocolMap& pmap) {
		if (pmap.has_int("magic")) set_magic(pmap.get_int("magic"));
		if (pmap.has_int("major")) set_major(pmap.get_int("major"));
		if (pmap.has_int("minor")) set_minor(pmap.get_int("minor"));
		if (pmap.has_int("network")) set_network(pmap.get_int("network"));
		if (pmap.has_int("sigfigs")) set_sigfigs(pmap.get_int("sigfigs"));
		if (pmap.has_int("snaplen")) set_snaplen(pmap.get_int("snaplen"));
		if (pmap.has_int("thiszone")) set_thiszone(pmap.get_int("thiszone"));
		load_entry_array(pmap);
	}

	void load_entry_array(const ProtocolMap& pmap) {
		size_t i = 0;
		while (true) {
			if (!pmap.has_array("entry", i)) break;
			ProtocolMap element = pmap.get_array("entry", i);
			_entry.push_back(entry_t(element));
			++i;
		}
	}

	class entry_t {
	public:
		entry_t() {
			_init_incl_len = false;
			_init_orig_len = false;
			_init_sec = false;
			_init_usec = false;
			_init_payload = false;
		}

		entry_t(const ProtocolMap& pmap) { load(pmap); }
		entry_t(const string& data) {
			_state.reset(data);
			process(&_state);
		}

		entry_t(ProtocolState* state) {
			process(state);
		}

		virtual ~entry_t() {}

		static string pmap_name() { return "entry"; }
		void process(ProtocolState* state) {
			state->prepare();
			Operator* op = ProtocolLibrary::_()->load(pmap_name(), pmap_data());
			ProtocolMap pmap;
			op->process(&pmap, state);
			load(pmap);
		}

		virtual void load(const ProtocolMap& pmap) {
			if (pmap.has_int("incl_len")) set_incl_len(pmap.get_int("incl_len"));
			if (pmap.has_int("orig_len")) set_orig_len(pmap.get_int("orig_len"));
			if (pmap.has_int("sec")) set_sec(pmap.get_int("sec"));
			if (pmap.has_int("usec")) set_usec(pmap.get_int("usec"));
			if (pmap.has_str("payload")) set_payload(pmap.get_str("payload"));
		}

		void set_incl_len(size_t val) {
			_init_incl_len = true;
			_incl_len = val;
		}
		size_t incl_len() const {
			if (!_init_incl_len) throw runtime_error("incl_len not initialized");
			return _incl_len;
		}
		bool has_incl_len() const { return _init_incl_len; }
		void set_orig_len(size_t val) {
			_init_orig_len = true;
			_orig_len = val;
		}
		size_t orig_len() const {
			if (!_init_orig_len) throw runtime_error("orig_len not initialized");
			return _orig_len;
		}
		bool has_orig_len() const { return _init_orig_len; }
		void set_sec(size_t val) {
			_init_sec = true;
			_sec = val;
		}
		size_t sec() const {
			if (!_init_sec) throw runtime_error("sec not initialized");
			return _sec;
		}
		bool has_sec() const { return _init_sec; }
		void set_usec(size_t val) {
			_init_usec = true;
			_usec = val;
		}
		size_t usec() const {
			if (!_init_usec) throw runtime_error("usec not initialized");
			return _usec;
		}
		bool has_usec() const { return _init_usec; }
		void set_payload(const string& val) {
			_init_payload = true;
			_payload = val;
		}
		string payload() const {
			if (!_init_payload) throw runtime_error("payload not initialized");
			return _payload;
		}
		bool has_payload() const { return _init_payload; }
	protected:
		bool _init_incl_len;
		size_t _incl_len;
		bool _init_orig_len;
		size_t _orig_len;
		bool _init_sec;
		size_t _sec;
		bool _init_usec;
		size_t _usec;
		bool _init_payload;
		string _payload;
		ProtocolState _state;
	};
	static string pmap_data() { return R"(u32 magic
u16 major
u16 minor
u32 thiszone
u32 sigfigs
u32 snaplen
u32 network
eq magic 3569595041 {
	htons major major
	htons minor minor
	htonl thiszone thiszone
	htonl sigfigs sigfigs
	htonl snaplen snaplen
	htonl network network
}
while entry {
	u32 sec
	u32 usec
	u32 incl_len
	u32 orig_len
	eq magic 3569595041 {
		htonl sec sec
		htonl usec usec
		htonl incl_len incl_len
		htonl orig_len orig_len
	}
	lrstring incl_len payload
}
)"; }
	void set_magic(size_t val) {
		_init_magic = true;
		_magic = val;
	}
	size_t magic() const {
		if (!_init_magic) throw runtime_error("magic not initialized");
		return _magic;
	}
	bool has_magic() const { return _init_magic; }
	void set_major(size_t val) {
		_init_major = true;
		_major = val;
	}
	size_t major() const {
		if (!_init_major) throw runtime_error("major not initialized");
		return _major;
	}
	bool has_major() const { return _init_major; }
	void set_minor(size_t val) {
		_init_minor = true;
		_minor = val;
	}
	size_t minor() const {
		if (!_init_minor) throw runtime_error("minor not initialized");
		return _minor;
	}
	bool has_minor() const { return _init_minor; }
	void set_network(size_t val) {
		_init_network = true;
		_network = val;
	}
	size_t network() const {
		if (!_init_network) throw runtime_error("network not initialized");
		return _network;
	}
	bool has_network() const { return _init_network; }
	void set_sigfigs(size_t val) {
		_init_sigfigs = true;
		_sigfigs = val;
	}
	size_t sigfigs() const {
		if (!_init_sigfigs) throw runtime_error("sigfigs not initialized");
		return _sigfigs;
	}
	bool has_sigfigs() const { return _init_sigfigs; }
	void set_snaplen(size_t val) {
		_init_snaplen = true;
		_snaplen = val;
	}
	size_t snaplen() const {
		if (!_init_snaplen) throw runtime_error("snaplen not initialized");
		return _snaplen;
	}
	bool has_snaplen() const { return _init_snaplen; }
	void set_thiszone(size_t val) {
		_init_thiszone = true;
		_thiszone = val;
	}
	size_t thiszone() const {
		if (!_init_thiszone) throw runtime_error("thiszone not initialized");
		return _thiszone;
	}
	bool has_thiszone() const { return _init_thiszone; }
	void add_entry(const entry_t& val) { _entry.emplace_back(val); }
	const vector<entry_t>& entry() const { return _entry; }
protected:
	bool _init_magic;
	size_t _magic;
	bool _init_major;
	size_t _major;
	bool _init_minor;
	size_t _minor;
	bool _init_network;
	size_t _network;
	bool _init_sigfigs;
	size_t _sigfigs;
	bool _init_snaplen;
	size_t _snaplen;
	bool _init_thiszone;
	size_t _thiszone;
	vector<entry_t> _entry;
	ProtocolState _state;
};

}  // namespace pmaps
}  // namespace protodecode

#endif  // __PROTODECODE__PMAPS__pcap__H__

