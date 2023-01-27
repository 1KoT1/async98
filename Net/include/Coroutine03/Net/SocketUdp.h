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

		/** The refference to array of bytes.
		 *
		 * You can get the pointer to data and size of the array.
		 * This class doesn't owns of the array but only provides
		 * an access to.
		 **/
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
			void send(const void *buffer, size_t size, Core::Timeout timeout);
			void send(const void *buffer, size_t size);
			void sendTo(const void *buffer, size_t size, const Poco::Net::SocketAddress &address, Core::Timeout timeout);
			void sendTo(const void *buffer, size_t size, const Poco::Net::SocketAddress &address);
			ssize_t recv(void *bufferForRecv, size_t size, Core::Timeout timeout);
			ssize_t recv(void *bufferForRecv, size_t size);

			/** The event when new UDP packet is received.
			 *
			 * Handlers will be called in syncon way. The argumetn is a reference to the
			 * udp paccket data. This reference is valid while handler is executing. It
			 * will be invalid after the handler will return.
			 **/
			Poco::BasicEvent<const BufferRef> packetReceived;

		protected:
			void handle(Core::EPoll::Events events);
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