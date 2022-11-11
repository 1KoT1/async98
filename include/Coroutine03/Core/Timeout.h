#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <Poco/Timespan.h>
#include <stdexcept>

namespace Coroutine03 {
namespace Core {

	class Timeout : public Poco::Timespan {
	public:
	Timeout() : Timespan() {}
		
	Timeout(TimeDiff microseconds) : Timespan(microseconds) {
		if(totalMicroseconds() < 0) throw std::runtime_error("Timeout MUST by better 0.");
	}
	
	Timeout(long seconds, long microseconds) : Timespan(seconds, microseconds) {
		if(totalMicroseconds() < 0) throw std::runtime_error("Timeout MUST by better 0.");
	}
	
	Timeout(int days, int hours, int minutes, int seconds, int microSeconds) : Timespan(days, hours, minutes, seconds, microSeconds) {
		if(totalMicroseconds() < 0) throw std::runtime_error("Timeout MUST by better 0.");
	}

	Timeout(const Timespan &timespan) : Timespan(timespan) {
		if(totalMicroseconds() < 0) throw std::runtime_error("Timeout MUST by better 0.");
	}

	Timeout(const Timeout& timeout) : Timespan(timeout.totalMicroseconds()) {}
	};

} // namespace Core
} // namespace Coroutine03

#endif // TIMEOUT_H
