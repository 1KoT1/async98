#ifndef SOURCEEPOLL_H
#define SOURCEEPOLL_H

#include "Coroutine03/Core/Timeout.h"
#include <exception>
#include <stdexcept>
#include <sys/epoll.h>

namespace Poco { class Logger; }

namespace Coroutine03 {
	namespace Core {

		/** Wrapper for linux epoll api.
		 *
		 * It implement RAII for epoll fd and make access to epoll system calls by class methots.
		 * See man epoll. Method wait(...) is equals epoll_wait(...). Methods add(...),
		 * del(...), mod(...)are equals epoll_ctl(...).
		 */
		class EPoll {
		public:
			typedef int Events;

			EPoll();
			~EPoll();


			int wait(struct epoll_event *events, int maxevents, Timeout timeout);
			int wait(struct epoll_event *events, int maxevents);

			void add(int fd, int events);
			void del(int fd);
			void mod(int fd, int newEvents);
		private:
			int _epollFd;
			Poco::Logger &_log;

			// Disable copeing.
			EPoll(const EPoll &);
			EPoll &operator=(const EPoll &);
		};

		class EPollFailException : std::runtime_error {
		public:
			explicit EPollFailException(const std::string &message) : std::runtime_error(message) {}
		};

	} // namespace Core
} // namespace Coroutine03


#endif // SOURCEEPOLL_H
