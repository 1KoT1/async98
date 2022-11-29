#ifndef STOPBYSIGNALS_H
#define STOPBYSIGNALS_H

#include "Coroutine03/Core/HandlerEPollEvents.h"
#include <Poco/SharedPtr.h>
#include <stdexcept>

namespace Poco { class Logger; }

namespace Coroutine03 {
	namespace Core {

		class StopBySignals : public HandlerEPollEvents {
		public:
			StopBySignals(Poco::SharedPtr<DispatcherEPoll> dispatcher);
			~StopBySignals();
		protected:
			void handle(EPoll::Events events);
		private:
			Poco::Logger &_log;
			// Disable copying:
			StopBySignals(const StopBySignals &);
			StopBySignals &operator==(const StopBySignals &);
		};

		class SignalFDFail : std::runtime_error {
		public:
			SignalFDFail();
		};

	} // namespace Core
} // namespace Coroutine03
#endif // STOPBYSIGNALS_H
