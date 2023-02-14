#include "Asynch98/Core/EPoll.h"
#include <cstring>
#include <Poco/Format.h>
#include <Poco/Logger.h>
#include <sys/epoll.h>
#include <unistd.h>

using Poco::format;
using Poco::Logger;
using std::string;

namespace Asynch98 {
	namespace Core {

		EPoll::EPoll()
			: _epollFd(epoll_create1(EPOLL_CLOEXEC))
			, _log(Logger::root())
		{
			if(_epollFd == -1) {
				throw EPollFailException(format("Fail of the epoll_create1 method. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
		}

		EPoll::~EPoll() {
			if(_epollFd != -1) {
				if(close(_epollFd) == -1) {
					poco_error_f2(_log, "Fail of close the epoll fd. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0))));
				}
			}
		}

		int EPoll::wait(struct epoll_event *events, int maxevents, Timeout timeout) const {
			int n = epoll_wait(_epollFd, events, maxevents, timeout.totalMilliseconds());
			if(n == -1) {
				throw EPollFailException(format("Fail of the epoll_wait method. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
			return n;
		}

		const int INFINITE = -1;

		int EPoll::wait(struct epoll_event *events, int maxevents) const {
			int n = epoll_wait(_epollFd, events, maxevents, INFINITE);
			if(n == -1) {
				throw EPollFailException(format("Fail of the epoll_wait method. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
			if(n == 0) {
				throw EPollFailException("Fail of the epoll_wait method: Unexpected elapse timeout.");
			}
			return n;
		}

		void EPoll::add(int fd, int events) {
			epoll_event ev;
			ev.data.fd = fd;
			ev.events = events;

			if(epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &ev)) {
				throw EPollFailException(format("Fail of the epoll_ctl method. EPOLL_CTL_ADD. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
		}

		void EPoll::del(int fd) {
			if(epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) == -1) {
				throw EPollFailException(format("Fail of the epoll_ctl method. EPOLL_CTL_DEL. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
		}

		void EPoll::mod(int fd, int newEvents) {
			epoll_event ev;
			ev.data.fd = fd;
			ev.events = newEvents;
			if(epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &ev ) == -1) {
				throw EPollFailException(format("Fail of the epoll_ctl method. EPOLL_CTL_MOD. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
		}

	} // namespace Core
} // namespace Asynch98
