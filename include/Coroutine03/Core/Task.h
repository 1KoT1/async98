#ifndef TASK_H
#define TASK_H

namespace Coroutine03 {
	namespace Core {

		class Task {
		public:
			Task() {}
			virtual ~Task() {}
			void run() {}
			virtual bool operator==(const Task &other) const = 0;
		private:
			// Disable copying
			Task(const Task &);
			Task &operator=(const Task &);
		};

	} // namespace Core
} // namespace Coroutine03

#endif // TASK_H
