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
	ProtocolState(const string& init_data) {
		data = init_data;
		data_pos = 0;
		data_subpos = 0;
	}

	string data;
	size_t data_pos;
	size_t data_subpos;

	virtual void tidy() {
		if (data_subpos == 8) {
			data_subpos = 0;
			++data_pos;
		}
	}
};

}  // namespace protodecode

#endif  // __PROTODECODE__PROTOCOL_STATE__H__
