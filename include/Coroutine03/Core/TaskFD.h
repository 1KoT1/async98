#ifndef TASKFD_H
#define TASKFD_H

namespace Coroutine03 {
	namespace Core {

		class TaskFD {
		public:
			explicit TaskFD(int fd);
			bool operator==(const TaskFD &other) const;
			int fd() const;
			virtual int interestedEvents() const = 0;
			virtual void run(int epollEvents) = 0;
		private:
			// Disable copying
			TaskFD(const TaskFD &);
			TaskFD &operator=(const TaskFD &);

			int _fd;
		};

	} // namespace Core
} // namespace Coroutine03

#endif // TASKFD_H
