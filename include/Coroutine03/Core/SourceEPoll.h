#include <stdexcept>
#ifndef SOURCEEPOLL_H

#include "Coroutine03/Core/QueueOneThread.h"
#include <exception>
#include <Poco/SharedPtr.h>

namespace Poco { class Logger; }

namespace Coroutine03 {
	namespace Core {

		class SourceEPoll {
		public:
			SourceEPoll(const Poco::SharedPtr<QueueOneThread> &queue);
			~SourceEPoll();
			void checIO();
		private:
			Poco::SharedPtr<QueueOneThread> _queue;
			int _epollFd;
			Poco::Logger &_log;
		};

		class EPollFailException : std::runtime_error {
		public:
			explicit EPollFailException(const std::string &message) : std::runtime_error(message) {}
		};

	} // namespace Core
} // namespace Coroutine03


#define SOURCEEPOLL_H
#endif // SOURCEEPOLL_H
