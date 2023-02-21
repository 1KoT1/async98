#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <Poco/Timespan.h>
#include <stdexcept>

namespace Asynch98 {
namespace Core {

	typedef int TimeoutInMilliseconds;
	const TimeoutInMilliseconds INFINITE = -1;

} // namespace Core
} // namespace Asynch98

#endif // TIMEOUT_H
