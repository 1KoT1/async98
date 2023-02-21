#ifndef SOCKETTCP_H
#define SOCKETTCP_H

#include "Asynch98/Core/HandlerEPollEvents.h"
#include "Asynch98/Core/Timeout.h"
#include "Poco/BasicEvent.h"
#include <Poco/Net/SocketAddress.h>
#include <Poco/SharedPtr.h>
#include <stdexcept>
#include <string>

namespace Asynch98 {
	namespace Net {

		class TcpClosed : std::runtime_error {
		public:
			TcpClosed() : std::runtime_error("The TCP peer closed its end of the channel.") {}
		};

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

		class SocketTcp : public Core::HandlerEPollEvents {
		public:
			SocketTcp(Poco::SharedPtr<Core::DispatcherEPoll> dispatcher, size_t bufSize = 1024);
			~SocketTcp();

			void connect(const Poco::Net::SocketAddress &address, Core::TimeoutInMilliseconds timeout = Core::INFINITE);
			void bind(const Poco::Net::SocketAddress &address);
			void listen(int backlog = 4096);
			struct acceptResult {
				Poco::Net::SocketAddress addressOfIncoming;
				Poco::SharedPtr<SocketTcp> newSocket;
			};
			acceptResult accept(Core::TimeoutInMilliseconds timeout = Core::INFINITE) const;
			void send(const void *buffer, size_t size, Core::TimeoutInMilliseconds timeout = Core::INFINITE);
			ssize_t recv(void *bufferForRecv, size_t size, Core::TimeoutInMilliseconds timeout = Core::INFINITE);

			/** The event when new TCP packet is received.
			 *
			 * Handlers will be called in syncon way. The argumetn is a reference to the
			 * tcp paccket data. This reference is valid while handler is executing. It
			 * will be invalid after the handler will return.
			 **/
			Poco::BasicEvent<const BufferRef> packetReceived;

		protected:
			void handle(Core::EPoll::Events events);
		private:
			char *_buf;
			size_t _bufSize;

			SocketTcp(int fd, Poco::SharedPtr<Core::DispatcherEPoll> dispatcher, size_t bufSize = 1024);

			// Disable copying
			SocketTcp(const SocketTcp &);
			SocketTcp &operator=(const SocketTcp &);
		};

	} // namespace Net
} // namespace Asynch98

#endif // SOCKETTCP_H
