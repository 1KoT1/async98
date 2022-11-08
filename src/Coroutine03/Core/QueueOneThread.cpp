#include "Coroutine03/Core/QueueOneThread.h"
#include <Poco/SharedPtr.h>

using Poco::SharedPtr;

namespace Coroutine03 {
	namespace Core {

		QueueOneThread::QueueOneThread(const SharedPtr<Task> &parentTask)
			: _tasks()
			, _parentTask(parentTask)
			, _resumeParent(false)
		{}

		void QueueOneThread::add(SharedPtr<Task> task) {
			_tasks.push(task);
		}

		const Task & QueueOneThread::parentTask() const {
			return *_parentTask;
		}

		void QueueOneThread::resumeParent() {
			_resumeParent = true;
		}

		void QueueOneThread::run() {
			while(true) {
				checkSources();
				while(!_tasks.empty()) {
					if(_resumeParent) {
						_resumeParent = false;
						return;
					}
					SharedPtr<Task> task = _tasks.front();
					_tasks.pop();
					task->run();
				}
			}
		}

	} // namespace Core
} // namespace Coroutine03
