#include "Asynch98/Core/HandlerEPollEvents.h"
#include <Poco/SharedPtr.h>

using Poco::SharedPtr;

namespace Asynch98 {
	namespace Core {

		HandlerEPollEvents::HandlerEPollEvents(SharedPtr<DispatcherEPoll> dispatcher, int fd, EPoll::Events interestedEvents)
			: _dispatcher(dispatcher)
			, _fd(fd)
		{
			_dispatcher->subscribe(_fd, interestedEvents, new Delegate(*this));
		}

	} // namespace Core
} // namespace Asynch98
