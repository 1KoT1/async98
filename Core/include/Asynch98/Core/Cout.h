#ifndef COUT_H
#define COUT_H

#include "Asynch98/Core/DicpatcherEPoll.h"
#include "Asynch98/Core/HandlerEPollEvents.h"
#include <ostream>
#include <Poco/SharedPtr.h>
#include <streambuf>
#include <unistd.h>

namespace Asynch98 {
	namespace Core {

		/** @breif Implementation of std::basic_streambuf for make quals std::cout which works with Coroutnes03::Coro::DispatcherEPoll
		 */
		template < class CharT>
			class StdOutBuf : public std::basic_streambuf<CharT>, public HandlerEPollEvents {
			private:
				typedef std::basic_streambuf<CharT> Base;
				typedef typename Base::int_type int_type;
				typedef typename Base::traits_type traits_type;

				static const int StdOutFD = 1;
				static const std::streamsize StdOutBufferSize = 1024;

			public:
				StdOutBuf(Poco::SharedPtr<DispatcherEPoll> dispatcher) 
					: Base() 
					, HandlerEPollEvents(dispatcher, StdOutFD, EPOLLOUT | EPOLLERR)
				{
					Base::setp(_buffer, _buffer + StdOutBufferSize);
				}
			protected:
				virtual int_type overflow(int_type c = traits_type::eof()) {
					std::streamsize n = StdOutBufferSize;
					if(c != traits_type::eof()) {
						_buffer[StdOutBufferSize] = c;
						n = StdOutBufferSize + 1;
					}
					EPoll::Events events = _dispatcher->wait(StdOutFD, EPOLLOUT | EPOLLERR);
					if(events & EPOLLOUT) {
						std::streamsize err = write(StdOutFD, _buffer, n);
						if(err != -1) {
							Base::setp(_buffer, _buffer + StdOutBufferSize);
							return _buffer[0];
						}
					}
					return traits_type::eof();
				}

				virtual int sync() {
					EPoll::Events events = _dispatcher->wait(StdOutFD, EPOLLOUT | EPOLLERR);
					if(events & EPOLLOUT) {
						std::streamsize n = Base::pptr() - _buffer;
						Base::setp(_buffer, _buffer + StdOutBufferSize);
						return write(StdOutFD, _buffer, n);
					}
					return -1;
				}

				/** Doesn't handle events because implementation of std::basic_streambuf only need "blocking" mwthod wait(...).*/
				void handle(Asynch98::Core::EPoll::Events) {}
			private:
				CharT _buffer[StdOutBufferSize + 1];
			};

		/** @breif Equals std::cout for working with Coroutnes03::Coro::DispatcherEPoll
		 */
		class Cout : public std::ostream {
		public:
			Cout(Poco::SharedPtr<DispatcherEPoll> dispatcher)
				: std::ostream(new StdOutBuf<char>(dispatcher))
			{}
			~Cout() {
				if(rdbuf()) {
					delete rdbuf();
				}
			}
		};

	} // namespace Core
} // namespace Asynch98
#endif // COUT_H
