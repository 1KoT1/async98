#ifndef HANDLEREPOLLEVENTS_H
#define HANDLEREPOLLEVENTS_H

#include "Asynch98/Core/EPoll.h"
#include "Asynch98/Core/DicpatcherEPoll.h"
#include <Poco/SharedPtr.h>

namespace Asynch98 {
	namespace Core {

		/** \brief Base class for any IO classes.
		 *
		 * Use it as base for sockets, pipes, files, etc.
		 * It is implement RAII for subscribe on events from DispatcherEPoll
		 * on create and unsubscribe on destroy.
		 *
		 * Inheritor should implement of protected method void handle(EPoll::Events events).
		 * It is necessary and sufficient for handle events.
		 * All IO classes interact with events queue by DispatcherEPoll so inheritor
		 * of HandlerEPollEvents should get DispatcherEPoll in constructor and send it
		 * to HandlerEPollEvents.
		 * For implement sockets, pipes, files, etc you should inherit HandlerEPollEvents
		 * and implement method handle(EPoll::Events events).
		 */
		class HandlerEPollEvents {
		public:
			/**
			 * All IO classes should be binded to object of DispatcherEPoll for get events from it.
			 * Constructor of inheritor should get pointer to DispatcherEPoll and send it to
			 * this constructor.
			 *
			 * HandlerEPollEvents doesn't implement concret IO thing. It only use fd for subscribe
			 * and unsubscribe on epoll events. Inherirot should create concret file descriptor and
			 * send it to this constructor.
			 */
			HandlerEPollEvents(Poco::SharedPtr<DispatcherEPoll> dispatcher, int fd, EPoll::Events interestedEvents);
			virtual ~HandlerEPollEvents() {}
		protected:
			Poco::SharedPtr<DispatcherEPoll> _dispatcher;
			int _fd;
			virtual void handle(EPoll::Events events) = 0;
			friend class Delegate;


			// Details of interaction with DispatcherEPoll:
		public:
			class Delegate : public DispatcherEPoll::Handler {
			public:
				Delegate(HandlerEPollEvents &h) : _h(h) {}
				void run(EPoll::Events events) { _h.handle(events); }

				Delegate(const Delegate &other) : _h(other._h) {}
				Delegate &operator=(const Delegate &other) { _h = other._h; return *this; }
			private:
				HandlerEPollEvents &_h;
			};
		};

	} // namespace Core
} // namespace Asynch98

#endif // HANDLEREPOLLEVENTS_H
