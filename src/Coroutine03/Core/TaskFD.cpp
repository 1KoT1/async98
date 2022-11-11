#include "Coroutine03/Core/TaskFD.h"

namespace Coroutine03 {
	namespace Core {

		TaskFD::TaskFD(int fd)
			: _fd(fd)
		{}

		int TaskFD::fd() const {
			return _fd;
		}

		bool TaskFD::operator==(const TaskFD &other) const {
			return _fd == other._fd;
		}

	} // namespace Core
} // namespace Coroutine03
