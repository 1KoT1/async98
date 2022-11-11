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

			Poco::Optional<EPoll::Events> wait(int fd, Timeout timeout);
		private:
			EPoll _epoll;
			std::map<int, TaskFD*> _tasks;
		};

	} // namespace Core
} // namespace Coroutine03
#endif // DICPATCHEREPOLL_H
