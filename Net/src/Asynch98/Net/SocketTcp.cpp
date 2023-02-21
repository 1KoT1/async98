#include "Asynch98/Net/SocketTcp.h"
#include "Asynch98/Core/DicpatcherEPoll.h"
#include "Asynch98/Core/EPoll.h"
#include "Asynch98/Core/Timeout.h"
#include <Poco/Format.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Optional.h>
#include <Poco/SharedPtr.h>
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/epoll.h>

using Asynch98::Core::DispatcherEPoll;
using Asynch98::Core::EPoll;
using Asynch98::Core::TimeoutInMilliseconds;
using Poco::format;
using Poco::Net::SocketAddress;
using Poco::Net::SocketAddress;
using Poco::SharedPtr;
using std::runtime_error;
using std::string;

namespace Asynch98 {
	namespace Net {

		const int NO_FLAGS = 0;

		inline int createSocket() {
			int fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
			if(fd == -1) {
				throw runtime_error(format("Fail of create tcp socket. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
			return fd;
		}

		const int TCP_INTERESTED_EVENTS = EPOLLIN | EPOLLERR | EPOLLRDHUP;

		SocketTcp::SocketTcp(SharedPtr<DispatcherEPoll> dispatcher, size_t bufSize)
			: HandlerEPollEvents(dispatcher, createSocket(), TCP_INTERESTED_EVENTS)
			, _buf(new char[bufSize])
			, _bufSize(bufSize)
		{}

		SocketTcp::SocketTcp(int fd, SharedPtr<DispatcherEPoll> dispatcher, size_t bufSize)
			: HandlerEPollEvents(dispatcher, fd, TCP_INTERESTED_EVENTS)
			, _buf(new char[bufSize])
			, _bufSize(bufSize)
		{}

		SocketTcp::~SocketTcp() {
			delete [] _buf;
		}

		void SocketTcp::handle(EPoll::Events events) {
			if(events & EPOLLIN) {
				int n = ::recv(_fd, _buf, _bufSize, NO_FLAGS);
				if(n == -1) {
					throw runtime_error(format("Fail of recv from tcp socket. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
				}
				packetReceived.notify(this, BufferRef(_buf, n));
			}
		}

		void SocketTcp::connect(const SocketAddress &address, TimeoutInMilliseconds timeout) {
			int status = ::connect(_fd, address.addr(), address.length());
			if(status == 0) {
				return;
			} else {
				if(errno == EINPROGRESS) {
					_dispatcher->modSubscription(_fd, TCP_INTERESTED_EVENTS | EPOLLOUT);
					EPoll::Events events = _dispatcher->wait(
						_fd,
						EPOLLOUT | EPOLLERR,
						timeout
						);
					_dispatcher->modSubscription(_fd, TCP_INTERESTED_EVENTS);
					if(events & EPOLLERR) {
						throw runtime_error(format("Fail of connect tcp socket to address %s. Errno: %d, %s", address.toString(), errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
					}
					int SO_ERROR_value = -1;
					socklen_t retValLen = sizeof (SO_ERROR_value);
					if (getsockopt (_fd, SOL_SOCKET, SO_ERROR, &SO_ERROR_value, &retValLen) < 0) {
						throw runtime_error(format("Fail of connect tcp socket to address %s. Errno: %d, %s", address.toString(), errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
					}
					if(SO_ERROR_value != 0) {
						throw runtime_error(format("Fail of connect tcp socket to address %s. SO_ERROR: %d, %s", address.toString(), SO_ERROR_value, string(strerror_l(errno, static_cast<locale_t>(0)))));
					}
					return; // Success.
				} else {
					throw runtime_error(format("Fail of connect tcp socket to address %s. Errno: %d, %s", address.toString(), errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
				}
			}
		}

		void SocketTcp::bind(const SocketAddress &address) {
			int err = ::bind(_fd, address.addr(), address.length());
			if(err == -1) {
				throw runtime_error(format("Fail of bind tcp socket to address %s. Errno: %d, %s", address.toString(), errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
		}

		void SocketTcp::listen(int backlog) {
			int err = ::listen(_fd, backlog);
			if(err == -1) {
				throw runtime_error(format("Fail of listen tcp socket. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
		}

		SocketTcp::acceptResult SocketTcp::accept(TimeoutInMilliseconds timeout) const {
			EPoll::Events events = _dispatcher->wait(
				_fd,
				EPOLLIN | EPOLLERR,
				timeout
				);
			sockaddr_in addr;
			socklen_t l = sizeof(addr);
			int n = ::accept(_fd, reinterpret_cast<sockaddr*>(&addr), &l);
			if(n == -1) {
				throw runtime_error(format("Fail of accept tcp socket. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
			acceptResult r;
			r.addressOfIncoming = SocketAddress(reinterpret_cast<sockaddr*>(&addr), l);
			r.newSocket = new SocketTcp(n, _dispatcher, _bufSize);
			return r;
		}

		void SocketTcp::send(const void *buffer, size_t size, TimeoutInMilliseconds timeout) {
			_dispatcher->modSubscription(_fd, TCP_INTERESTED_EVENTS | EPOLLOUT);
			EPoll::Events events = _dispatcher->wait(
				_fd,
				EPOLLOUT | EPOLLERR,
				timeout
				);
			_dispatcher->modSubscription(_fd, TCP_INTERESTED_EVENTS);
			if(events == 0) {
				throw TimeoutException("Fail of send data to tcp socket by timeout.");
			}
			if(events & EPOLLERR) {
				throw runtime_error(format("Fail of send data to tcp socket. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}

			int err = ::send(_fd, buffer, size, NO_FLAGS);
			if(err == -1) {
				throw runtime_error(format("Fail of send data to tcp socket. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
		}

		ssize_t SocketTcp::recv(void *buffer, size_t size, TimeoutInMilliseconds timeout) {
			EPoll::Events events = _dispatcher->wait(
				_fd,
				EPOLLIN | EPOLLERR | EPOLLRDHUP,
				timeout
				);
			if(events == 0) {
				throw TimeoutException("Fail of receive data from tcp socket by timeout.");
			}
			if(events & EPOLLRDHUP) {
				throw TcpClosed();
			}
			if(events & EPOLLERR) {
				if(events & EPOLLHUP) {
					throw TcpClosed();
				}
				throw runtime_error(format("Fail of receive data from tcp socket. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}

			ssize_t n = ::recv(_fd, buffer, size, NO_FLAGS);
			if(n == -1) {
				throw runtime_error(format("Fail of receive data from tcp socket. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}

			return n;
		}

	} // namespace Net
} // namespace Asynch98

