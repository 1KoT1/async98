#include "Asynch98/Core/CanceledException.h"
namespace Asynch98 {
	namespace Core {
		CanceledException::CanceledException() : std::runtime_error("The events dispatcher was stopped.") {}
	} // namespace Core
} // namespace Asynch98
