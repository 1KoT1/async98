#include "Coroutine03/Core/DicpatcherEPoll.h"
#include <Poco/Timespan.h>
#include <Poco/Timestamp.h>
#include <string>

using Poco::Optional;
using Poco::Timespan;
using Poco::Timestamp;
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

		inline Timestamp now() { 
			return Timestamp(); // Current time;
		}

		EPoll::Events DispatcherEPoll::wait(int currentFd, Timeout timeout) {
			struct epoll_event events[MAX_EVENTS];
			Timestamp startTime; // Current time.
			for(Timespan nextTimeout = timeout; nextTimeout > 0; nextTimeout -= (now() - startTime)) {
				int happened = _epoll.wait(events, MAX_EVENTS, nextTimeout);
				if(happened == 0) {
					return 0; // Timedout
				}

				Optional<EPoll::Events> eventsForCurrent = find(events, MAX_EVENTS, currentFd);
				if(eventsForCurrent.isSpecified()) {
					return eventsForCurrent.value();
				}

				for(int i = 0; i < happened; ++i) {
					_tasks[events[i].data.fd]->run(events[i].events);
				}
			}
			return 0; // Timedout
		}

	} // namespace Core
} // namespace Coroutine03
