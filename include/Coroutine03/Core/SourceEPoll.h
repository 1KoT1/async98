#ifndef SOURCEEPOLL_H
#define SOURCEEPOLL_H

#include <map>
#include "Coroutine03/Core/QueueOneThread.h"
#include "Coroutine03/Core/TaskFD.h"
#include <exception>
#include <Poco/SharedPtr.h>
#include <stdexcept>

namespace Poco { class Logger; }

namespace Coroutine03 {
	namespace Core {

		class SourceEPoll {
		public:
			SourceEPoll(const Poco::SharedPtr<QueueOneThread> &queue);
			~SourceEPoll();
			void checkIO();
		private:
			Poco::SharedPtr<QueueOneThread> _queue;
			int _epollFd;
			std::map<int, Poco::SharedPtr<TaskFD> > _taskFDCollection;
			Poco::Logger &_log;

			void subscribe(const Poco::SharedPtr<TaskFD> &taskFD, int events);
			void unsubscribe(int fd);
			void modifySubscription(int fd, int newEvents);
		};

		class EPollFailException : std::runtime_error {
		public:
			explicit EPollFailException(const std::string &message) : std::runtime_error(message) {}
		};

	} // namespace Core
} // namespace Coroutine03


#endif // SOURCEEPOLL_H
