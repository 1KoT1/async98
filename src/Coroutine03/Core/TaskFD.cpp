#include "Coroutine03/Core/TaskFD.h"

namespace Coroutine03 {
	namespace Core {

		TaskFD::TaskFD(int fd)
			: _fd(fd)
			, _currentEvents(0)
		{}

		int TaskFD::fd() const {
			return _fd;
		}

		void TaskFD::setCurrentEvents(unsigned int events) {
			_currentEvents = events;
		}

		bool TaskFD::operator==(const TaskFD &other) const {
			return _fd == other._fd;
		}

		bool TaskFD::operator==(const Task &other) const {
			const TaskFD *o = dynamic_cast<const TaskFD*>(&other);
			return o && (*o == *this);
		}

	} // namespace Core
} // namespace Coroutine03
