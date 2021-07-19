#ifndef __PROTODECODE__PMAPS__tcp__H__
#define __PROTODECODE__PMAPS__tcp__H__

#include <cstdint>
#include <string>
#include <vector>

#include "../protocol_library.h"
#include "../protocol_map.h"
using namespace std;
namespace protodecode {
class ProtocolState;
namespace pmaps {

class tcp_t {
public:
	tcp_t() {
		_init_ack = false;
		_init_acknum = false;
		_init_checksum = false;
		_init_cwr = false;
		_init_dport = false;
		_init_ece = false;
		_init_fin = false;
		_init_ns = false;
		_init_offset = false;
		_init_offset_bytes = false;
		_init_option_len = false;
		_init_psh = false;
		_init_rst = false;
		_init_seqnum = false;
		_init_sport = false;
		_init_syn = false;
		_init_urg = false;
		_init_windowsize = false;
		_init_zero = false;
		_init_options = false;
	}

	tcp_t(const ProtocolMap& pmap) { load(pmap); }
	tcp_t(const string& data) {
		_state.reset(data);
		process(&_state);
	}

	tcp_t(ProtocolState* state) {
		process(state);
	}

	virtual ~tcp_t() {}

	static string pmap_name() { return "tcp"; }
	void process(ProtocolState* state) {
		state->prepare();
		Operator* op = ProtocolLibrary::_()->load(pmap_name(), pmap_data());
		ProtocolMap pmap;
		op->process(&pmap, state);
		load(pmap);
	}

	virtual void load(const ProtocolMap& pmap) {
		if (pmap.has_int("ack")) set_ack(pmap.get_int("ack"));
		if (pmap.has_int("acknum")) set_acknum(pmap.get_int("acknum"));
		if (pmap.has_int("checksum")) set_checksum(pmap.get_int("checksum"));
		if (pmap.has_int("cwr")) set_cwr(pmap.get_int("cwr"));
		if (pmap.has_int("dport")) set_dport(pmap.get_int("dport"));
		if (pmap.has_int("ece")) set_ece(pmap.get_int("ece"));
		if (pmap.has_int("fin")) set_fin(pmap.get_int("fin"));
		if (pmap.has_int("ns")) set_ns(pmap.get_int("ns"));
		if (pmap.has_int("offset")) set_offset(pmap.get_int("offset"));
		if (pmap.has_int("offset_bytes")) set_offset_bytes(pmap.get_int("offset_bytes"));
		if (pmap.has_int("option_len")) set_option_len(pmap.get_int("option_len"));
		if (pmap.has_int("psh")) set_psh(pmap.get_int("psh"));
		if (pmap.has_int("rst")) set_rst(pmap.get_int("rst"));
		if (pmap.has_int("seqnum")) set_seqnum(pmap.get_int("seqnum"));
		if (pmap.has_int("sport")) set_sport(pmap.get_int("sport"));
		if (pmap.has_int("syn")) set_syn(pmap.get_int("syn"));
		if (pmap.has_int("urg")) set_urg(pmap.get_int("urg"));
		if (pmap.has_int("windowsize")) set_windowsize(pmap.get_int("windowsize"));
		if (pmap.has_int("zero")) set_zero(pmap.get_int("zero"));
		if (pmap.has_str("options")) set_options(pmap.get_str("options"));
	}

	static string pmap_data() { return R"(u16 sport
u16 dport
u32 seqnum
u32 acknum
u4 offset
mul offset 4 offset_bytes
sub offset_bytes 20 option_len
u1 zero
u1 zero
u1 zero
u1 ns
u1 cwr
u1 ece
u1 urg
u1 ack
u1 psh
u1 rst
u1 syn
u1 fin
u16 windowsize
u16 checksum
u16 urg
lrstring option_len options
)"; }
	void set_ack(size_t val) {
		_init_ack = true;
		_ack = val;
	}
	size_t ack() const {
		if (!_init_ack) throw runtime_error("ack not initialized");
		return _ack;
	}
	bool has_ack() const { return _init_ack; }
	void set_acknum(size_t val) {
		_init_acknum = true;
		_acknum = val;
	}
	size_t acknum() const {
		if (!_init_acknum) throw runtime_error("acknum not initialized");
		return _acknum;
	}
	bool has_acknum() const { return _init_acknum; }
	void set_checksum(size_t val) {
		_init_checksum = true;
		_checksum = val;
	}
	size_t checksum() const {
		if (!_init_checksum) throw runtime_error("checksum not initialized");
		return _checksum;
	}
	bool has_checksum() const { return _init_checksum; }
	void set_cwr(size_t val) {
		_init_cwr = true;
		_cwr = val;
	}
	size_t cwr() const {
		if (!_init_cwr) throw runtime_error("cwr not initialized");
		return _cwr;
	}
	bool has_cwr() const { return _init_cwr; }
	void set_dport(size_t val) {
		_init_dport = true;
		_dport = val;
	}
	size_t dport() const {
		if (!_init_dport) throw runtime_error("dport not initialized");
		return _dport;
	}
	bool has_dport() const { return _init_dport; }
	void set_ece(size_t val) {
		_init_ece = true;
		_ece = val;
	}
	size_t ece() const {
		if (!_init_ece) throw runtime_error("ece not initialized");
		return _ece;
	}
	bool has_ece() const { return _init_ece; }
	void set_fin(size_t val) {
		_init_fin = true;
		_fin = val;
	}
	size_t fin() const {
		if (!_init_fin) throw runtime_error("fin not initialized");
		return _fin;
	}
	bool has_fin() const { return _init_fin; }
	void set_ns(size_t val) {
		_init_ns = true;
		_ns = val;
	}
	size_t ns() const {
		if (!_init_ns) throw runtime_error("ns not initialized");
		return _ns;
	}
	bool has_ns() const { return _init_ns; }
	void set_offset(size_t val) {
		_init_offset = true;
		_offset = val;
	}
	size_t offset() const {
		if (!_init_offset) throw runtime_error("offset not initialized");
		return _offset;
	}
	bool has_offset() const { return _init_offset; }
	void set_offset_bytes(size_t val) {
		_init_offset_bytes = true;
		_offset_bytes = val;
	}
	size_t offset_bytes() const {
		if (!_init_offset_bytes) throw runtime_error("offset_bytes not initialized");
		return _offset_bytes;
	}
	bool has_offset_bytes() const { return _init_offset_bytes; }
	void set_option_len(size_t val) {
		_init_option_len = true;
		_option_len = val;
	}
	size_t option_len() const {
		if (!_init_option_len) throw runtime_error("option_len not initialized");
		return _option_len;
	}
	bool has_option_len() const { return _init_option_len; }
	void set_psh(size_t val) {
		_init_psh = true;
		_psh = val;
	}
	size_t psh() const {
		if (!_init_psh) throw runtime_error("psh not initialized");
		return _psh;
	}
	bool has_psh() const { return _init_psh; }
	void set_rst(size_t val) {
		_init_rst = true;
		_rst = val;
	}
	size_t rst() const {
		if (!_init_rst) throw runtime_error("rst not initialized");
		return _rst;
	}
	bool has_rst() const { return _init_rst; }
	void set_seqnum(size_t val) {
		_init_seqnum = true;
		_seqnum = val;
	}
	size_t seqnum() const {
		if (!_init_seqnum) throw runtime_error("seqnum not initialized");
		return _seqnum;
	}
	bool has_seqnum() const { return _init_seqnum; }
	void set_sport(size_t val) {
		_init_sport = true;
		_sport = val;
	}
	size_t sport() const {
		if (!_init_sport) throw runtime_error("sport not initialized");
		return _sport;
	}
	bool has_sport() const { return _init_sport; }
	void set_syn(size_t val) {
		_init_syn = true;
		_syn = val;
	}
	size_t syn() const {
		if (!_init_syn) throw runtime_error("syn not initialized");
		return _syn;
	}
	bool has_syn() const { return _init_syn; }
	void set_urg(size_t val) {
		_init_urg = true;
		_urg = val;
	}
	size_t urg() const {
		if (!_init_urg) throw runtime_error("urg not initialized");
		return _urg;
	}
	bool has_urg() const { return _init_urg; }
	void set_windowsize(size_t val) {
		_init_windowsize = true;
		_windowsize = val;
	}
	size_t windowsize() const {
		if (!_init_windowsize) throw runtime_error("windowsize not initialized");
		return _windowsize;
	}
	bool has_windowsize() const { return _init_windowsize; }
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
	bool _init_ack;
	size_t _ack;
	bool _init_acknum;
	size_t _acknum;
	bool _init_checksum;
	size_t _checksum;
	bool _init_cwr;
	size_t _cwr;
	bool _init_dport;
	size_t _dport;
	bool _init_ece;
	size_t _ece;
	bool _init_fin;
	size_t _fin;
	bool _init_ns;
	size_t _ns;
	bool _init_offset;
	size_t _offset;
	bool _init_offset_bytes;
	size_t _offset_bytes;
	bool _init_option_len;
	size_t _option_len;
	bool _init_psh;
	size_t _psh;
	bool _init_rst;
	size_t _rst;
	bool _init_seqnum;
	size_t _seqnum;
	bool _init_sport;
	size_t _sport;
	bool _init_syn;
	size_t _syn;
	bool _init_urg;
	size_t _urg;
	bool _init_windowsize;
	size_t _windowsize;
	bool _init_zero;
	size_t _zero;
	bool _init_options;
	string _options;
	ProtocolState _state;
};

}  // namespace pmaps
}  // namespace protodecode

#endif  // __PROTODECODE__PMAPS__tcp__H__

