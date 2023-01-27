#ifndef CANCELEDEXCEPTION_H
#define CANCELEDEXCEPTION_H
#include <stdexcept>

namespace Coroutine03 {
	namespace Core {
		class CanceledException : public std::runtime_error {
		public:
			CanceledException();
		};
	} // namespace Core
} // namespace Coroutine03
#endif // CANCELEDEXCEPTION_H
