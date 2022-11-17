#include "Coroutine03/Core/HandlerEPollEvents.h"
#include <Poco/SharedPtr.h>

using Poco::SharedPtr;

namespace Coroutine03 {
	namespace Core {

		HandlerEPollEvents::HandlerEPollEvents(SharedPtr<DispatcherEPoll> dispatcher, int fd, EPoll::Events interestedEvents)
			: _dispatcher(dispatcher)
			, _fd(fd)
		{
			_dispatcher->subscribe(_fd, interestedEvents, new Delegate(*this));
		}

		HandlerEPollEvents::~HandlerEPollEvents() {
			_dispatcher->unsubscribe(_fd);
		}

	} // namespace Core
} // namespace Coroutine03
