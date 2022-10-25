#ifndef TASKFD_H
#define TASKFD_H

#include "Coroutine03/Core/Task.h"

namespace Coroutine03 {
	namespace Core {

		class TaskFD : public Task {
		public:
			explicit TaskFD(int fd);
			bool operator==(const Task &other) const;
			bool operator==(const TaskFD &other) const;
			int fd() const;
			void setCurrentEvents(unsigned int events);
		private:
			// Disable copying
			TaskFD(const TaskFD &);
			TaskFD &operator=(const TaskFD &);

			int _fd;
			int _currentEvents;
		};

	} // namespace Core
} // namespace Coroutine03

#endif // TASKFD_H
