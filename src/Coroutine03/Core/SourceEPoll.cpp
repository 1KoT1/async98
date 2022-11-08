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
using std::map;
using std::string;

namespace Coroutine03 {
	namespace Core {

		SourceEPoll::SourceEPoll(const SharedPtr<QueueOneThread> &queue)
			: _queue(queue)
			, _epollFd(epoll_create1(EPOLL_CLOEXEC))
			, _taskFDCollection()
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

		void SourceEPoll::checkIO() {
			struct epoll_event events[MAX_EVENTS];
			int n = epoll_wait(_epollFd, events, MAX_EVENTS, infinite);
			if(n == -1) {
				throw EPollFailException(format("Fail of the epoll_wait method. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
			if(n == 0) {
				poco_error(_log, "Fail of the epoll_wait method: Unexpected elapse timeout.");
			} else {
				for(int i = 0; i != n; ++i) {
					SharedPtr<TaskFD> eventHappenedTask = _taskFDCollection.at(events[i].data.fd);
					eventHappenedTask->setCurrentEvents(events[i].events);
					if(_queue->parentTask() == *eventHappenedTask) {
						_queue->resumeParent();
					} else {
						_queue->add(eventHappenedTask);
					}
				}
			}
		}

		void SourceEPoll::subscibe(const SharedPtr<TaskFD> &task, int events) {
			epoll_event ev;
			ev.data.fd = task->fd();
			ev.events = events;

			if(epoll_ctl(_epollFd, EPOLL_CTL_ADD, task->fd(), &ev)) {
				throw EPollFailException(format("Fail of the epoll_ctl method. EPOLL_CTL_ADD. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}

			_taskFDCollection[task->fd()] = task;
		}

		void SourceEPoll::unsubscibe(int fd) {
			if(epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) {
				throw EPollFailException(format("Fail of the epoll_ctl method. EPOLL_CTL_DEL. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
			_taskFDCollection.erase(fd);
		}

		void SourceEPoll::modifySubscription(int fd, int newEvents) {
			epoll_event ev;
			ev.data.fd = fd;
			ev.events = newEvents;
			if(epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev ) == -1) {
				throw EPollFailException(format("Fail of the epoll_ctl method. EPOLL_CTL_MOD. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
		}

	} // namespace Core
} // namespace Coroutine03
