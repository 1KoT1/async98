#include "Coroutine03/Core/SourceEPoll.h"
#include <cstring>
#include <Poco/Format.h>
#include <Poco/Logger.h>
#include <sys/epoll.h>
#include <unistd.h>

using Poco::format;
using Poco::Logger;
using Poco::SharedPtr;
using std::string;

namespace Coroutine03 {
	namespace Core {

		Events::Events(int size)
			: _size(size)
			, _data(new struct epoll_event[size])
		{}

		Events::~Events() {
			delete [] _data;
		}

		struct epoll_event *Events::data() {
			return _data;
		}

		int Events::size() const {
			return _size;
		}

		SourceEPoll::SourceEPoll()
			: _epollFd(epoll_create1(EPOLL_CLOEXEC))
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

		void SourceEPoll::wait(Events &events) {
			int n = epoll_wait(_epollFd, events.data(), events.size(), infinite);
			if(n == -1) {
				throw EPollFailException(format("Fail of the epoll_wait method. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
			if(n == 0) {
				poco_error(_log, "Fail of the epoll_wait method: Unexpected elapse timeout.");
			}
		}

		void SourceEPoll::subscribe(int fd, int events) {
			epoll_event ev;
			ev.data.fd = fd;
			ev.events = events;

			if(epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &ev)) {
				throw EPollFailException(format("Fail of the epoll_ctl method. EPOLL_CTL_ADD. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
		}

		void SourceEPoll::unsubscribe(int fd) {
			if(epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) {
				throw EPollFailException(format("Fail of the epoll_ctl method. EPOLL_CTL_DEL. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
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
