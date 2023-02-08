#ifndef CIN_H
#define CIN_H

#include "Asynch98/Core/DicpatcherEPoll.h"
#include "Asynch98/Core/HandlerEPollEvents.h"
#include <istream>
#include <Poco/SharedPtr.h>
#include <streambuf>
#include <unistd.h>

namespace Asynch98 {
	namespace Core {

		/** @breif Implementation of std::basic_streambuf for make quals std::cin which works with Coroutnes03::Coro::DispatcherEPoll
		 */
		template < class CharT>
			class StdInBuf : public std::basic_streambuf<CharT>, public HandlerEPollEvents {
				// Don't inherit from HandlerEPollEvents because it doesn't need subscibe for events but only "blocking" method wait(...).
			public:
				StdInBuf(Poco::SharedPtr<DispatcherEPoll> dispatcher) 
					: std::basic_streambuf<CharT>() 
					, HandlerEPollEvents(dispatcher, StdInFD, EPOLLIN | EPOLLERR)
				{
					Base::setg(_buffer, _buffer, _buffer);
				}
			protected:
				typedef std::basic_streambuf<CharT> Base;
				typedef typename Base::int_type int_type;
				typedef typename Base::traits_type traits_type;

				static const int StdInFD = 0;
				static const std::streamsize StdInBufferSize = 1024;

				virtual int_type underflow() {
					EPoll::Events events = _dispatcher->wait(StdInFD, EPOLLIN | EPOLLERR);
					if(events & EPOLLIN) {
						std::streamsize n = read(StdInFD, _buffer, StdInBufferSize);
						if(n > 0) {
							Base::setg(_buffer, _buffer, _buffer + n);
							return traits_type::to_int_type(_buffer[0]);
						}
					}
					Base::setg(_buffer, _buffer, _buffer);
					return traits_type::eof();
				}

				/** Doesn't handle events because implementation of std::basic_streambuf only need "blocking" mwthod wait(...).*/
				void handle(Asynch98::Core::EPoll::Events) {}
			private:
				CharT _buffer[StdInBufferSize];
			};

		/** @breif Equals std::cin for working with Coroutnes03::Coro::DispatcherEPoll
		 */
		class Cin : public std::istream {
		public:
			Cin(Poco::SharedPtr<DispatcherEPoll> dispatcher)
				: std::istream(new StdInBuf<char>(dispatcher))
			{}
			~Cin() {
				if(rdbuf()) {
					delete rdbuf();
				}
			}
		};

	} // namespace Core
} // namespace Asynch98
#endif // CIN_H
