#ifndef QUEUEONETHREAD_H
#define QUEUEONETHREAD_H

#include <Poco/SharedPtr.h>
#include <queue>
#include "Task.h"

namespace Coroutine03 {
	namespace Core {

		class QueueOneThread {
		public:
			QueueOneThread(const Poco::SharedPtr<Task> &parentTask);
			void add(Poco::SharedPtr<Task> task);
			void run();
			void resumeParent();
			const Task &parentTask() const;
		private:
			// Disable copying
			QueueOneThread(const QueueOneThread &);
			QueueOneThread &operator()(const QueueOneThread &);

			typedef std::queue<Poco::SharedPtr<Task> > Tasks;
			Tasks _tasks;

			Poco::SharedPtr<Task> _parentTask;
			bool _resumeParent;

			void checkSources();
		};

	} // namespace Core
} // namespace Coroutine03

#endif // QUEUEONETHREAD_H
