#ifndef SOCKETUDP_H
#define SOCKETUDP_H

#include "Coroutine03/Core/DicpatcherEPoll.h"
#include "Coroutine03/Core/TaskFD.h"
#include "Poco/BasicEvent.h"
#include <Poco/Net/SocketAddress.h>

namespace Coroutine03 {
	namespace Core {

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

		enum Timeouted {
			Success,
			Timedout
		};

		class SocketUdp : public TaskFD {
		public:
			SocketUdp(Poco::SharedPtr<DispatcherEPoll> dispatcher, size_t bufSize = 1024);
			~SocketUdp();

			int interestedEvents() const;
			void run(int epollEvents);

			void connect(const Poco::Net::SocketAddress &address);
			void bind(const Poco::Net::SocketAddress &address);
			Timeouted send(const BufferRef &buffer, Timeout timeout);
			Timeouted recv(BufferRef &bufferForRecv, Timeout timeout);

			Poco::BasicEvent<const BufferRef> packetReceived;
		private:
			char *_buf;
			size_t _bufSize;
			Poco::SharedPtr<DispatcherEPoll> _dispatcher;

			// Disable copying
			SocketUdp(const SocketUdp &);
			SocketUdp &operator=(const SocketUdp &);
		};

	} // namespace Core
} // namespace Coroutine03

#endif // SOCKETUDP_H
