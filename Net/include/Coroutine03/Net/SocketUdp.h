#ifndef SOCKETUDP_H
#define SOCKETUDP_H

#include "Coroutine03/Core/HandlerEPollEvents.h"
#include "Poco/BasicEvent.h"
#include <Poco/Net/SocketAddress.h>
#include <stdexcept>
#include <string>

namespace Coroutine03 {
	namespace Net {

		class TimeoutException : std::runtime_error {
		public:
			TimeoutException(const std::string &message) : std::runtime_error(message) {}
		};

		class BufferRef {
		public:
			BufferRef(char *data, size_t size) : _data(data), _size(size) {}
			void *data() { return _data; }
			const void *data() const { return _data; }
			size_t size() const { return _size; }
		private:
			char *_data;
			size_t _size;
		};

		class SocketUdp : public Core::HandlerEPollEvents {
		public:
			SocketUdp(Poco::SharedPtr<Core::DispatcherEPoll> dispatcher, size_t bufSize = 1024);
			~SocketUdp();

			void connect(const Poco::Net::SocketAddress &address);
			void bind(const Poco::Net::SocketAddress &address);
			void send(const BufferRef &buffer, Core::Timeout timeout);
			void recv(BufferRef &bufferForRecv, Core::Timeout timeout);

			Poco::BasicEvent<const BufferRef> packetReceived;

		protected:
			void handler(Core::EPoll::Events events);
		private:
			char *_buf;
			size_t _bufSize;

			// Disable copying
			SocketUdp(const SocketUdp &);
			SocketUdp &operator=(const SocketUdp &);
		};

	} // namespace Net
} // namespace Coroutine03

#endif // SOCKETUDP_H
