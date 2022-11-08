#ifndef SOURCEEPOLL_H
#define SOURCEEPOLL_H

#include "Coroutine03/Core/QueueOneThread.h"
#include <exception>
#include <stdexcept>
#include <sys/epoll.h>

namespace Poco { class Logger; }

namespace Coroutine03 {
	namespace Core {

		class Events {
		public:
			Events(int size);
			~Events();
			struct epoll_event *data();
			int size() const;
		private:
			int _size;
			struct epoll_event *_data;
		};

		class SourceEPoll {
		public:
			SourceEPoll();
			~SourceEPoll();
			void wait(Events &happenedEvents);

			void subscribe(int fd, int events);
			void unsubscribe(int fd);
			void modifySubscription(int fd, int newEvents);
		private:
			int _epollFd;
			Poco::Logger &_log;
		};

		class EPollFailException : std::runtime_error {
		public:
			explicit EPollFailException(const std::string &message) : std::runtime_error(message) {}
		};

	} // namespace Core
} // namespace Coroutine03


#endif // SOURCEEPOLL_H
