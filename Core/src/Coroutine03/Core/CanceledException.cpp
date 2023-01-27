#include "Coroutine03/Core/CanceledException.h"
namespace Coroutine03 {
	namespace Core {
		CanceledException::CanceledException() : std::runtime_error("The events dispatcher was stopped.") {}
	} // namespace Core
} // namespace Coroutine03
