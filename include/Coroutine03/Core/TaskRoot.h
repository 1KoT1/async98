#ifndef TASKROOT_H
#define TASKROOT_H

#include "Coroutine03/Core/Task.h"

namespace Coroutine03 {
	namespace Core {

		class TaskRoot : public Task {
		public:
			bool operator==(const Task &other) const;
		private:
			// Disable copying
			TaskRoot(const TaskRoot &);
		};

	} // namespace Core
} // namespace Coroutine03

#endif // TASKROOT_H
