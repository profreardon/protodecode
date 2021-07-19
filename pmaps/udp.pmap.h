#ifndef __PROTODECODE__PMAPS__udp__H__
#define __PROTODECODE__PMAPS__udp__H__

#include <cstdint>
#include <string>
#include <vector>

#include "../protocol_library.h"
#include "../protocol_map.h"
using namespace std;
namespace protodecode {
class ProtocolState;
namespace pmaps {

class udp_t {
public:
	udp_t() {
		_init_chksum = false;
		_init_dport = false;
		_init_len = false;
		_init_sport = false;
	}

	udp_t(const ProtocolMap& pmap) { load(pmap); }
	udp_t(const string& data) {
		_state.reset(data);
		process(&_state);
	}

	udp_t(ProtocolState* state) {
		process(state);
	}

	virtual ~udp_t() {}

	static string pmap_name() { return "udp"; }
	void process(ProtocolState* state) {
		state->prepare();
		Operator* op = ProtocolLibrary::_()->load(pmap_name(), pmap_data());
		ProtocolMap pmap;
		op->process(&pmap, state);
		load(pmap);
	}

	virtual void load(const ProtocolMap& pmap) {
		if (pmap.has_int("chksum")) set_chksum(pmap.get_int("chksum"));
		if (pmap.has_int("dport")) set_dport(pmap.get_int("dport"));
		if (pmap.has_int("len")) set_len(pmap.get_int("len"));
		if (pmap.has_int("sport")) set_sport(pmap.get_int("sport"));
	}

	static string pmap_data() { return R"(u16 sport
u16 dport
u16 len
u16 chksum
)"; }
	void set_chksum(size_t val) {
		_init_chksum = true;
		_chksum = val;
	}
	size_t chksum() const {
		if (!_init_chksum) throw runtime_error("chksum not initialized");
		return _chksum;
	}
	bool has_chksum() const { return _init_chksum; }
	void set_dport(size_t val) {
		_init_dport = true;
		_dport = val;
	}
	size_t dport() const {
		if (!_init_dport) throw runtime_error("dport not initialized");
		return _dport;
	}
	bool has_dport() const { return _init_dport; }
	void set_len(size_t val) {
		_init_len = true;
		_len = val;
	}
	size_t len() const {
		if (!_init_len) throw runtime_error("len not initialized");
		return _len;
	}
	bool has_len() const { return _init_len; }
	void set_sport(size_t val) {
		_init_sport = true;
		_sport = val;
	}
	size_t sport() const {
		if (!_init_sport) throw runtime_error("sport not initialized");
		return _sport;
	}
	bool has_sport() const { return _init_sport; }
protected:
	bool _init_chksum;
	size_t _chksum;
	bool _init_dport;
	size_t _dport;
	bool _init_len;
	size_t _len;
	bool _init_sport;
	size_t _sport;
	ProtocolState _state;
};

}  // namespace pmaps
}  // namespace protodecode

#endif  // __PROTODECODE__PMAPS__udp__H__

