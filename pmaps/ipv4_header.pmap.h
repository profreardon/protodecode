#ifndef __PROTODECODE__PMAPS__ipv4_header__H__
#define __PROTODECODE__PMAPS__ipv4_header__H__

#include <cstdint>
#include <string>
#include <vector>

#include "../protocol_library.h"
#include "../protocol_map.h"
using namespace std;
namespace protodecode {
class ProtocolState;
namespace pmaps {

class ipv4_header_t {
public:
	ipv4_header_t() {
		_init_checksum = false;
		_init_df = false;
		_init_dscp_ecn = false;
		_init_dst_ip = false;
		_init_fragment = false;
		_init_id = false;
		_init_ihl = false;
		_init_ihl_bytes = false;
		_init_mf = false;
		_init_options_len = false;
		_init_protocol = false;
		_init_src_ip = false;
		_init_tot_len = false;
		_init_ttl = false;
		_init_version = false;
		_init_zero = false;
		_init_options = false;
	}

	ipv4_header_t(const ProtocolMap& pmap) { load(pmap); }
	ipv4_header_t(const string& data) {
		_state.reset(data);
		process(&_state);
	}

	ipv4_header_t(ProtocolState* state) {
		process(state);
	}

	virtual ~ipv4_header_t() {}

	static string pmap_name() { return "ipv4_header"; }
	void process(ProtocolState* state) {
		state->prepare();
		Operator* op = ProtocolLibrary::_()->load(pmap_name(), pmap_data());
		ProtocolMap pmap;
		op->process(&pmap, state);
		load(pmap);
	}

	virtual void load(const ProtocolMap& pmap) {
		if (pmap.has_int("checksum")) set_checksum(pmap.get_int("checksum"));
		if (pmap.has_int("df")) set_df(pmap.get_int("df"));
		if (pmap.has_int("dscp_ecn")) set_dscp_ecn(pmap.get_int("dscp_ecn"));
		if (pmap.has_int("dst_ip")) set_dst_ip(pmap.get_int("dst_ip"));
		if (pmap.has_int("fragment")) set_fragment(pmap.get_int("fragment"));
		if (pmap.has_int("id")) set_id(pmap.get_int("id"));
		if (pmap.has_int("ihl")) set_ihl(pmap.get_int("ihl"));
		if (pmap.has_int("ihl_bytes")) set_ihl_bytes(pmap.get_int("ihl_bytes"));
		if (pmap.has_int("mf")) set_mf(pmap.get_int("mf"));
		if (pmap.has_int("options_len")) set_options_len(pmap.get_int("options_len"));
		if (pmap.has_int("protocol")) set_protocol(pmap.get_int("protocol"));
		if (pmap.has_int("src_ip")) set_src_ip(pmap.get_int("src_ip"));
		if (pmap.has_int("tot_len")) set_tot_len(pmap.get_int("tot_len"));
		if (pmap.has_int("ttl")) set_ttl(pmap.get_int("ttl"));
		if (pmap.has_int("version")) set_version(pmap.get_int("version"));
		if (pmap.has_int("zero")) set_zero(pmap.get_int("zero"));
		if (pmap.has_str("options")) set_options(pmap.get_str("options"));
	}

	static string pmap_data() { return R"(// default types for protocol
const ICMP 1
const TCP 6
const UDP 17
u4 version
u4 ihl
/* compute the options len */
mul ihl 4 ihl_bytes
sub ihl_bytes 20 options_len
u8 dscp_ecn
u16 tot_len
u16 id
u1 zero
u1 df
u1 mf
align 8
u8 fragment
u8 ttl
u8 protocol
u16 checksum
u32 src_ip
u32 dst_ip
lrstring options_len options
)"; }
	static const size_t ICMP = 1;
	static const size_t TCP = 6;
	static const size_t UDP = 17;
	void set_checksum(size_t val) {
		_init_checksum = true;
		_checksum = val;
	}
	size_t checksum() const {
		if (!_init_checksum) throw runtime_error("checksum not initialized");
		return _checksum;
	}
	bool has_checksum() const { return _init_checksum; }
	void set_df(size_t val) {
		_init_df = true;
		_df = val;
	}
	size_t df() const {
		if (!_init_df) throw runtime_error("df not initialized");
		return _df;
	}
	bool has_df() const { return _init_df; }
	void set_dscp_ecn(size_t val) {
		_init_dscp_ecn = true;
		_dscp_ecn = val;
	}
	size_t dscp_ecn() const {
		if (!_init_dscp_ecn) throw runtime_error("dscp_ecn not initialized");
		return _dscp_ecn;
	}
	bool has_dscp_ecn() const { return _init_dscp_ecn; }
	void set_dst_ip(size_t val) {
		_init_dst_ip = true;
		_dst_ip = val;
	}
	size_t dst_ip() const {
		if (!_init_dst_ip) throw runtime_error("dst_ip not initialized");
		return _dst_ip;
	}
	bool has_dst_ip() const { return _init_dst_ip; }
	void set_fragment(size_t val) {
		_init_fragment = true;
		_fragment = val;
	}
	size_t fragment() const {
		if (!_init_fragment) throw runtime_error("fragment not initialized");
		return _fragment;
	}
	bool has_fragment() const { return _init_fragment; }
	void set_id(size_t val) {
		_init_id = true;
		_id = val;
	}
	size_t id() const {
		if (!_init_id) throw runtime_error("id not initialized");
		return _id;
	}
	bool has_id() const { return _init_id; }
	void set_ihl(size_t val) {
		_init_ihl = true;
		_ihl = val;
	}
	size_t ihl() const {
		if (!_init_ihl) throw runtime_error("ihl not initialized");
		return _ihl;
	}
	bool has_ihl() const { return _init_ihl; }
	void set_ihl_bytes(size_t val) {
		_init_ihl_bytes = true;
		_ihl_bytes = val;
	}
	size_t ihl_bytes() const {
		if (!_init_ihl_bytes) throw runtime_error("ihl_bytes not initialized");
		return _ihl_bytes;
	}
	bool has_ihl_bytes() const { return _init_ihl_bytes; }
	void set_mf(size_t val) {
		_init_mf = true;
		_mf = val;
	}
	size_t mf() const {
		if (!_init_mf) throw runtime_error("mf not initialized");
		return _mf;
	}
	bool has_mf() const { return _init_mf; }
	void set_options_len(size_t val) {
		_init_options_len = true;
		_options_len = val;
	}
	size_t options_len() const {
		if (!_init_options_len) throw runtime_error("options_len not initialized");
		return _options_len;
	}
	bool has_options_len() const { return _init_options_len; }
	void set_protocol(size_t val) {
		_init_protocol = true;
		_protocol = val;
	}
	size_t protocol() const {
		if (!_init_protocol) throw runtime_error("protocol not initialized");
		return _protocol;
	}
	bool has_protocol() const { return _init_protocol; }
	void set_src_ip(size_t val) {
		_init_src_ip = true;
		_src_ip = val;
	}
	size_t src_ip() const {
		if (!_init_src_ip) throw runtime_error("src_ip not initialized");
		return _src_ip;
	}
	bool has_src_ip() const { return _init_src_ip; }
	void set_tot_len(size_t val) {
		_init_tot_len = true;
		_tot_len = val;
	}
	size_t tot_len() const {
		if (!_init_tot_len) throw runtime_error("tot_len not initialized");
		return _tot_len;
	}
	bool has_tot_len() const { return _init_tot_len; }
	void set_ttl(size_t val) {
		_init_ttl = true;
		_ttl = val;
	}
	size_t ttl() const {
		if (!_init_ttl) throw runtime_error("ttl not initialized");
		return _ttl;
	}
	bool has_ttl() const { return _init_ttl; }
	void set_version(size_t val) {
		_init_version = true;
		_version = val;
	}
	size_t version() const {
		if (!_init_version) throw runtime_error("version not initialized");
		return _version;
	}
	bool has_version() const { return _init_version; }
	void set_zero(size_t val) {
		_init_zero = true;
		_zero = val;
	}
	size_t zero() const {
		if (!_init_zero) throw runtime_error("zero not initialized");
		return _zero;
	}
	bool has_zero() const { return _init_zero; }
	void set_options(const string& val) {
		_init_options = true;
		_options = val;
	}
	string options() const {
		if (!_init_options) throw runtime_error("options not initialized");
		return _options;
	}
	bool has_options() const { return _init_options; }
protected:
	bool _init_checksum;
	size_t _checksum;
	bool _init_df;
	size_t _df;
	bool _init_dscp_ecn;
	size_t _dscp_ecn;
	bool _init_dst_ip;
	size_t _dst_ip;
	bool _init_fragment;
	size_t _fragment;
	bool _init_id;
	size_t _id;
	bool _init_ihl;
	size_t _ihl;
	bool _init_ihl_bytes;
	size_t _ihl_bytes;
	bool _init_mf;
	size_t _mf;
	bool _init_options_len;
	size_t _options_len;
	bool _init_protocol;
	size_t _protocol;
	bool _init_src_ip;
	size_t _src_ip;
	bool _init_tot_len;
	size_t _tot_len;
	bool _init_ttl;
	size_t _ttl;
	bool _init_version;
	size_t _version;
	bool _init_zero;
	size_t _zero;
	bool _init_options;
	string _options;
	ProtocolState _state;
};

}  // namespace pmaps
}  // namespace protodecode

#endif  // __PROTODECODE__PMAPS__ipv4_header__H__

