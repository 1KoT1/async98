#include "Coroutine03/Core/SourceEPoll.h"
#include "Coroutine03/Core/TaskFD.h"
#include <cstring>
#include <Poco/Format.h>
#include <Poco/Logger.h>
#include <Poco/SharedPtr.h>
#include <sys/epoll.h>
#include <unistd.h>

using Poco::format;
using Poco::Logger;
using Poco::SharedPtr;
using std::string;

namespace Coroutine03 {
	namespace Core {

		SourceEPoll::SourceEPoll(const SharedPtr<QueueOneThread> &queue)
			: _queue(queue)
				, _epollFd(epoll_create1(EPOLL_CLOEXEC))
				 , _log(Logger::root())
		{
			if(_epollFd == -1) {
				throw EPollFailException(format("Fail of the epoll_create1 method. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
		}

		SourceEPoll::~SourceEPoll() {
			if(_epollFd != -1) {
				if(close(_epollFd) == -1) {
					poco_error_f2(_log, "Fail of close the epoll fd. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0))));
				}
			}
		}

		const int infinite = -1;
		const int MAX_EVENTS = 10;

		void SourceEPoll::checIO() {
			struct epoll_event events[MAX_EVENTS];
			int n = epoll_wait(_epollFd, events, MAX_EVENTS, infinite);
			if(n == -1) {
				throw EPollFailException(format("Fail of the epoll_wait method. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
			if(n == 0) {
				poco_error(_log, "Fail of the epoll_wait method: Unexpected elapse timeout.");
			} else {
				for(int i = 0; i != n; ++i) {
					SharedPtr<TaskFD> handlerObj = *static_cast<SharedPtr<TaskFD>*>(events[i].data.ptr);
					handlerObj->setCurrentEvents(events[i].events);
					if(_queue->parentTask() == *handlerObj) {
						_queue->resumeParent();
					} else {
						_queue->add(handlerObj);
					}
				}
			}
		}

	} // namespace Core
} // namespace Coroutine03
