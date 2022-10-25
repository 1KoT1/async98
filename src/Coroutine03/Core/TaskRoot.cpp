#include "Coroutine03/Core/TaskRoot.h"

namespace Coroutine03 {
	namespace Core {

		bool TaskRoot::operator==(const Task &other) const {
			return this == &other;
		}

	} // namespace Core
} // namespace Coroutine03
