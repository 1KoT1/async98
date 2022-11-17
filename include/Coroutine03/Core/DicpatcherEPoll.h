#ifndef DICPATCHEREPOLL_H
#define DICPATCHEREPOLL_H

#include "Coroutine03/Core/TaskFD.h"
#include "Coroutine03/Core/Timeout.h"
#include "EPoll.h"
#include <map>
#include <Poco/Optional.h>

namespace Coroutine03 {
	namespace Core {

		class DispatcherEPoll {
		public:
			void subscribe(TaskFD *task);
			void run();

			/** \brief Wait while some events on file descriptor.
			 * 
			 * Execute handlers for events on other file descriptors while
			 * wait events on interested file descriptor.
			 * Exit when events happened on interested file descriptor.
			 * 
			 * \param currentFd an interested file descriptor for wait events.
			 * \param timeout is time for wait events. If time is spended exit with empty value.
			 * \return Flags of happened events or empty on timeout.
			 */
			Poco::Optional<EPoll::Events> wait(int interestedFd, Timeout timeout);
		private:
			EPoll _epoll;
			std::map<int, TaskFD*> _tasks;
		};

	} // namespace Core
} // namespace Coroutine03
#endif // DICPATCHEREPOLL_H
