#ifndef __IB__TINY_TIMER__H__
#define __IB__TINY_TIMER__H__

#include <chrono>
#include <ctime>
#include <memory>
#include <mutex>
#include <string>
#include <sys/time.h>

using namespace std;

namespace ib {

class TinyTimer {
public:
	static uint64_t now() {
		struct timeval tv;
                gettimeofday(&tv, nullptr);
                time_t tt = tv.tv_sec;
                chrono::time_point<chrono::system_clock> retval =
                        chrono::system_clock::from_time_t(tt);
                retval += chrono::microseconds(tv.tv_usec);

		time_t zero = 0;
                chrono::time_point<chrono::system_clock> notime
                        = chrono::system_clock::from_time_t(zero);
                uint64_t curtime = chrono::duration_cast<chrono::microseconds>(
                        retval - notime).count();
		return curtime;
	}

	TinyTimer() : TinyTimer("[undefined]") {}
	TinyTimer(const string& name) {
		assert(name.find(": ") == string::npos);
		_then = now();
		_name = name;
	}

	virtual ~TinyTimer() {
		cout << "[tt] " << _name << ": " << now() - _then <<
		    " us" << endl;
	}

protected:
	uint64_t _then;
	string _name;
};

}  // namespace ib

#endif  // __IB__TINY_TIMER__H__
