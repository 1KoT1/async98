#include "Asynch98/Core/DicpatcherEPoll.h"
#include <Poco/SharedPtr.h>
#include <Poco/Timespan.h>
#include <Poco/Timestamp.h>
#include <string>

using Poco::Optional;
using Poco::SharedPtr;
using Poco::Timespan;
using Poco::Timestamp;
using std::string;

namespace Asynch98 {
	namespace Core {

		DispatcherEPoll::DispatcherEPoll()
			: _isStarted(true)
		{}

		const int MAX_EVENTS = 10;

		void DispatcherEPoll::subscribe(int fd, EPoll::Events interestedEvents, SharedPtr<Handler> handler) {
			_handlers[fd] = handler;
			_epoll.add(fd, interestedEvents);
		}

		void DispatcherEPoll::modSubscription(int fd, EPoll::Events interestedEvents, SharedPtr<Handler> handler) {
			_epoll.mod(fd, interestedEvents);
			_handlers[fd] = handler;
		}

		void DispatcherEPoll::modSubscription(int fd, EPoll::Events interestedEvents) {
			_epoll.mod(fd, interestedEvents);
		}

		void DispatcherEPoll::unsubscribe(int fd) {
			_handlers.erase(fd);
			_epoll.del(fd);
		}

		void DispatcherEPoll::stop() {
			_isStarted = false;
		}

		inline Optional<EPoll::Events> find(const epoll_event *events, int eventsSize, int currentFd, EPoll::Events eventsMask) {
			for(int i = 0; i < eventsSize; ++i) {
				if(events[i].data.fd == currentFd && events[i].events & eventsMask) {
					return events[i].events;
				}
			}
			return Optional<EPoll::Events>();
		}

		inline Timestamp now() { 
			return Timestamp(); // Current time;
		}

		EPoll::Events DispatcherEPoll::wait(int currentFd, EPoll::Events eventsMask, Timeout timeout) const {
			struct epoll_event events[MAX_EVENTS];
			Timestamp startTime; // Current time.
			for(Timespan nextTimeout = timeout; nextTimeout > 0; nextTimeout -= (now() - startTime)) {
				int happened = _epoll.wait(events, MAX_EVENTS, nextTimeout);
				if(!_isStarted) {
					throw CanceledException();
				}

				if(happened == 0) {
					return 0; // Timedout
				}

				Optional<EPoll::Events> eventsForCurrent = find(events, happened, currentFd, eventsMask);
				if(eventsForCurrent.isSpecified()) {
					return eventsForCurrent.value();
				}

				for(int i = 0; i < happened; ++i) {
					_handlers.at(events[i].data.fd)->run(events[i].events);
					if(!_isStarted) {
						throw CanceledException();
					}
				}
			}
			return 0; // Timedout
		}

		EPoll::Events DispatcherEPoll::wait(int currentFd, EPoll::Events eventsMask) const {
			struct epoll_event events[MAX_EVENTS];
			while(true) {
				int happened = _epoll.wait(events, MAX_EVENTS);
				if(!_isStarted) {
					throw CanceledException();
				}

				Optional<EPoll::Events> eventsForCurrent = find(events, happened, currentFd, eventsMask);
				if(eventsForCurrent.isSpecified()) {
					return eventsForCurrent.value();
				}

				for(int i = 0; i < happened; ++i) {
					_handlers.at(events[i].data.fd)->run(events[i].events);
					if(!_isStarted) {
						throw CanceledException();
					}
				}
			}
		}

	} // namespace Core
} // namespace Asynch98
