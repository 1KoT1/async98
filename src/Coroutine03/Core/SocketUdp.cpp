#include "Coroutine03/Core/SocketUdp.h"
#include <Poco/Format.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Optional.h>
#include <Poco/SharedPtr.h>
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/epoll.h>

using Poco::format;
using Poco::Net::SocketAddress;
using Poco::Optional;
using Poco::SharedPtr;
using std::runtime_error;
using std::string;

namespace Coroutine03 {
	namespace Core {

		const int NO_FLAGS = 0;

		inline int createSocket() {
			int fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
			if(fd == -1) {
				throw runtime_error(format("Fail of create udp socket. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
			return fd;
		}

		SocketUdp::SocketUdp(SharedPtr<DispatcherEPoll> dispatcher, size_t bufSize)
			: TaskFD(createSocket())
			, _buf(new char[bufSize])
			, _bufSize(bufSize)
			, _dispatcher(dispatcher)
		{}

		SocketUdp::~SocketUdp() {
			delete [] _buf;
		}

		const int UDP_INTERESTED_EVENTS = EPOLLIN | EPOLLOUT | EPOLLERR;

		int SocketUdp::interestedEvents() const { return UDP_INTERESTED_EVENTS; }

		void SocketUdp::run(int epollEvents) {
			if(epollEvents & EPOLLIN) {
				int n = ::recv(fd(), _buf, _bufSize, NO_FLAGS);
				if(n == -1) {
					throw runtime_error(format("Fail of recv from udp socket. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
				}
				packetReceived.notify(this, BufferRef(_buf, _bufSize));
			}
		}

		void SocketUdp::connect(const SocketAddress &address) {
			int err = ::connect(fd(), address.addr(), address.length());
			if(err == -1) {
				throw runtime_error(format("Fail of connect udp socket to address %s. Errno: %d, %s", address.toString(), errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
		}

		void SocketUdp::bind(const SocketAddress &address) {
			int err = ::bind(fd(), address.addr(), address.length());
			if(err == -1) {
				throw runtime_error(format("Fail of bind udp socket to address %s. Errno: %d, %s", address.toString(), errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
		}

		void SocketUdp::send(const BufferRef &buffer, Timeout timeout) {
			while(true) {
				EPoll::Events events = _dispatcher->wait(fd(), timeout);
				if(events == 0) {
					throw TimeoutException("Fail of send data to udp socket by timeout.");
				}
				if(events & EPOLLERR) {
					throw runtime_error(format("Fail of send data to udp socket. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
				}
				if(events & EPOLLOUT) {
					break;
				}
			}

			int err = ::send(fd(), buffer.data(), buffer.size(), NO_FLAGS);
			if(err == -1) {
				throw runtime_error(format("Fail of send data to udp socket. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
		}

		void SocketUdp::recv(BufferRef &buffer, Timeout timeout) {
			while(true) {
				EPoll::Events events = _dispatcher->wait(fd(), timeout);
				if(events == 0) {
					throw TimeoutException("Fail of receive data from udp socket by timeout.");
				}
				if(events & EPOLLERR) {
					throw runtime_error(format("Fail of receive data from udp socket. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
				}
				if(events & EPOLLIN) {
					break;
				}
			}

			int err = ::recv(fd(), buffer.data(), buffer.size(), NO_FLAGS);
			if(err == -1) {
				throw runtime_error(format("Fail of receive data from udp socket. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0)))));
			}
		}

	} // namespace Core
} // namespace Coroutine03

