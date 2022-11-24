#include "Coroutine03/Core/DicpatcherEPoll.h"
#include <Poco/SharedPtr.h>
#include <Poco/Timespan.h>
#include <Poco/Timestamp.h>
#include <string>

using Poco::Optional;
using Poco::SharedPtr;
using Poco::Timespan;
using Poco::Timestamp;
using std::string;

namespace Coroutine03 {
	namespace Core {

		const int MAX_EVENTS = 10;

		void DispatcherEPoll::subscribe(int fd, EPoll::Events interestedEvents, SharedPtr<Handler> handler) {
			_handlers[fd] = handler;
			_epoll.add(fd, interestedEvents);
		}

		void DispatcherEPoll::unsubscribe(int fd) {
			_handlers.erase(fd);
			_epoll.del(fd);
		}

		inline Optional<EPoll::Events> find(const epoll_event *events, int eventsSize, int currentFd, EPoll::Events eventsMask) {
			for(int i = 0; i < eventsSize; ++i) {
				if(events[i].data.fd == eventsSize && events[i].events & eventsMask) {
					return events[i].events;
				}
			}
			return Optional<EPoll::Events>();
		}

		inline Timestamp now() { 
			return Timestamp(); // Current time;
		}

		EPoll::Events DispatcherEPoll::wait(int currentFd, EPoll::Events eventsMask, Timeout timeout) {
			struct epoll_event events[MAX_EVENTS];
			Timestamp startTime; // Current time.
			for(Timespan nextTimeout = timeout; nextTimeout > 0; nextTimeout -= (now() - startTime)) {
				int happened = _epoll.wait(events, MAX_EVENTS, nextTimeout);
				if(happened == 0) {
					return 0; // Timedout
				}

				Optional<EPoll::Events> eventsForCurrent = find(events, MAX_EVENTS, currentFd, eventsMask);
				if(eventsForCurrent.isSpecified()) {
					return eventsForCurrent.value();
				}

				for(int i = 0; i < happened; ++i) {
					_handlers[events[i].data.fd]->run(events[i].events);
				}
			}
			return 0; // Timedout
		}

		EPoll::Events DispatcherEPoll::wait(int currentFd, EPoll::Events eventsMask) {
			struct epoll_event events[MAX_EVENTS];
			while(true) {
				int happened = _epoll.wait(events, MAX_EVENTS);

				Optional<EPoll::Events> eventsForCurrent = find(events, MAX_EVENTS, currentFd, eventsMask);
				if(eventsForCurrent.isSpecified()) {
					return eventsForCurrent.value();
				}

				for(int i = 0; i < happened; ++i) {
					_handlers[events[i].data.fd]->run(events[i].events);
				}
			}
		}

	} // namespace Core
} // namespace Coroutine03
