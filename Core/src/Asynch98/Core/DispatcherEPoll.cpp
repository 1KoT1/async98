#include "Asynch98/Core/DicpatcherEPoll.h"
#include <Poco/SharedPtr.h>
#include <Poco/Timespan.h>
#include <Poco/Timestamp.h>
#include <stdexcept>
#include <string>

using Poco::Optional;
using Poco::SharedPtr;
using Poco::Timespan;
using Poco::Timestamp;
using std::logic_error;
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

		void DispatcherEPoll::run() {
			struct epoll_event events[MAX_EVENTS];
			while(_isStarted) {
				int happened = _epoll.wait(events, MAX_EVENTS, INFINITE);

				if(happened == 0) {
					throw logic_error("Unexpected timeout of epoll_wait");
				}

				for(int i = 0; i < happened; ++i) {
					_handlers.at(events[i].data.fd)->run(events[i].events);
					if(!_isStarted) {
						throw CanceledException();
					}
				}
			}
			throw CanceledException();
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

		TimeoutInMilliseconds toMilliseconds(Timestamp::TimeDiff value) {
			return Timespan(value).totalMilliseconds();
		}

		class TimeoutStrategy {
		public:
			virtual TimeoutInMilliseconds nextTimeout() = 0;
			virtual bool inProgress() const = 0;
		};
		class TimeoutStrategyInfinite : public TimeoutStrategy {
		public:
			TimeoutInMilliseconds nextTimeout() { return INFINITE; }
			bool inProgress() const { return true; }
		};
		class TimeoutStrategyDecrement : public TimeoutStrategy {
		public:
			TimeoutStrategyDecrement(TimeoutInMilliseconds timeout)
				: _startTime(now())
				, _timeout(timeout)
			{
				if(_timeout < 0) throw std::logic_error("Timeout MUST be better 0");
			}
			TimeoutInMilliseconds nextTimeout() {
				return _timeout -= toMilliseconds(now() - _startTime);
			}
			bool inProgress() const {
				return _timeout > 0;
			}
		private:
			Timestamp _startTime;
			TimeoutInMilliseconds _timeout;
		};

		EPoll::Events DispatcherEPoll::wait(int currentFd, EPoll::Events eventsMask, TimeoutInMilliseconds timeout) const {
			struct epoll_event events[MAX_EVENTS];
			SharedPtr<TimeoutStrategy> timeoutStrategy = (timeout == INFINITE)
				? SharedPtr<TimeoutStrategy>(new TimeoutStrategyInfinite)
				: new TimeoutStrategyDecrement(timeout);
			for(TimeoutInMilliseconds nextTimeout = timeoutStrategy->nextTimeout(); timeoutStrategy->inProgress(); nextTimeout = timeoutStrategy->nextTimeout()) {
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

	} // namespace Core
} // namespace Asynch98
