#ifndef CANCELEDEXCEPTION_H
#define CANCELEDEXCEPTION_H
#include <stdexcept>

namespace Asynch98 {
	namespace Core {
		class CanceledException : public std::runtime_error {
		public:
			CanceledException();
		};
	} // namespace Core
} // namespace Asynch98
#endif // CANCELEDEXCEPTION_H
