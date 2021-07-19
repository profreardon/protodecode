#ifndef __PROTODECODE__PMAPS__lcc__H__
#define __PROTODECODE__PMAPS__lcc__H__

#include <cstdint>
#include <string>
#include <vector>

#include "../protocol_library.h"
#include "../protocol_map.h"
using namespace std;
namespace protodecode {
class ProtocolState;
namespace pmaps {

class lcc_t {
public:
	lcc_t() {
		_init_addr_type = false;
		_init_packet_type = false;
		_init_protocol = false;
		_init_other_mac = false;
		_init_remaining_mac = false;
	}

	lcc_t(const ProtocolMap& pmap) { load(pmap); }
	lcc_t(const string& data) {
		_state.reset(data);
		process(&_state);
	}

	lcc_t(ProtocolState* state) {
		process(state);
	}

	virtual ~lcc_t() {}

	static string pmap_name() { return "lcc"; }
	void process(ProtocolState* state) {
		state->prepare();
		Operator* op = ProtocolLibrary::_()->load(pmap_name(), pmap_data());
		ProtocolMap pmap;
		op->process(&pmap, state);
		load(pmap);
	}

	virtual void load(const ProtocolMap& pmap) {
		if (pmap.has_int("addr_type")) set_addr_type(pmap.get_int("addr_type"));
		if (pmap.has_int("packet_type")) set_packet_type(pmap.get_int("packet_type"));
		if (pmap.has_int("protocol")) set_protocol(pmap.get_int("protocol"));
		if (pmap.has_str("other_mac")) set_other_mac(pmap.get_str("other_mac"));
		if (pmap.has_str("remaining_mac")) set_remaining_mac(pmap.get_str("remaining_mac"));
	}

	static string pmap_data() { return R"(const IPV4 0x800
u16 packet_type
le packet_type 4 {
	u16 addr_type
	padding 10
	u16 protocol
}
gt packet_type 4 {
	string 4 remaining_mac
	string 6 other_mac
	u16 protocol
}
)"; }
	static const size_t IPV4 = 2048;
	void set_addr_type(size_t val) {
		_init_addr_type = true;
		_addr_type = val;
	}
	size_t addr_type() const {
		if (!_init_addr_type) throw runtime_error("addr_type not initialized");
		return _addr_type;
	}
	bool has_addr_type() const { return _init_addr_type; }
	void set_packet_type(size_t val) {
		_init_packet_type = true;
		_packet_type = val;
	}
	size_t packet_type() const {
		if (!_init_packet_type) throw runtime_error("packet_type not initialized");
		return _packet_type;
	}
	bool has_packet_type() const { return _init_packet_type; }
	void set_protocol(size_t val) {
		_init_protocol = true;
		_protocol = val;
	}
	size_t protocol() const {
		if (!_init_protocol) throw runtime_error("protocol not initialized");
		return _protocol;
	}
	bool has_protocol() const { return _init_protocol; }
	void set_other_mac(const string& val) {
		_init_other_mac = true;
		_other_mac = val;
	}
	string other_mac() const {
		if (!_init_other_mac) throw runtime_error("other_mac not initialized");
		return _other_mac;
	}
	bool has_other_mac() const { return _init_other_mac; }
	void set_remaining_mac(const string& val) {
		_init_remaining_mac = true;
		_remaining_mac = val;
	}
	string remaining_mac() const {
		if (!_init_remaining_mac) throw runtime_error("remaining_mac not initialized");
		return _remaining_mac;
	}
	bool has_remaining_mac() const { return _init_remaining_mac; }
protected:
	bool _init_addr_type;
	size_t _addr_type;
	bool _init_packet_type;
	size_t _packet_type;
	bool _init_protocol;
	size_t _protocol;
	bool _init_other_mac;
	string _other_mac;
	bool _init_remaining_mac;
	string _remaining_mac;
	ProtocolState _state;
};

}  // namespace pmaps
}  // namespace protodecode

#endif  // __PROTODECODE__PMAPS__lcc__H__

