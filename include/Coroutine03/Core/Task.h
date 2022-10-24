#ifndef TASK_H
#define TASK_H

namespace Coroutine03 {
	namespace Core {

		class Task {
		public:
			void run();
			bool isSuspended();
			bool operator==(const Task &other);
			void swithContext(const Task &other);
		private:
			// Disable copying
			Task(const Task &);
			Task &operator()(const Task &);
		};

	} // namespace Core
} // namespace Coroutine03

#endif // TASK_H
