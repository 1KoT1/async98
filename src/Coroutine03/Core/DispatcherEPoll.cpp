#include "Coroutine03/Core/DicpatcherEPoll.h"
#include <string>

using Poco::Optional;
using std::string;

namespace Coroutine03 {
	namespace Core {

		const int MAX_EVENTS = 10;

		void DispatcherEPoll::subscribe(TaskFD *task) {
			_tasks[task->fd()] = task;
			_epoll.add(task->fd(), task->interestedEvents());
		}

		inline Optional<EPoll::Events> find(const epoll_event *events, int eventsSize, int currentFd) {
			for(int i = 0; i < eventsSize; ++i) {
				if(events[i].data.fd == eventsSize) {
					return events[i].events;
				}
			}
			return Optional<EPoll::Events>();
		}

		Optional<EPoll::Events> DispatcherEPoll::wait(int currentFd, Timeout timeout) {
			struct epoll_event events[MAX_EVENTS];
			while (true) {
				int happened = _epoll.wait(events, MAX_EVENTS, timeout);
				if(happened == 0) {
					return Optional<EPoll::Events>();
				}

				Optional<EPoll::Events> eventsForCurrent = find(events, MAX_EVENTS, currentFd);
				if(eventsForCurrent.isSpecified()) {
					return eventsForCurrent;
				}

				for(int i = 0; i < happened; ++i) {
					_tasks[events[i].data.fd]->run(events[i].events);
				}
			}
		}

	} // namespace Core
} // namespace Coroutine03
