#include <Asynch98/Core/Cin.h>
#include <Asynch98/Core/Cout.h>
#include <Asynch98/Core/DicpatcherEPoll.h>
#include <Asynch98/Core/StopBySignals.h>
#include <Asynch98/Net/SocketUdp.h>
#include <iostream>
#include <Poco/Delegate.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/SharedPtr.h>
#include <string>

using Asynch98::Core::CanceledException;
using Asynch98::Core::Cin;
using Asynch98::Core::Cout;
using Asynch98::Core::DispatcherEPoll;
using Asynch98::Core::StopBySignals;
using Asynch98::Net::BufferRef;
using Asynch98::Net::SocketUdp;
using Poco::delegate;
using Poco::Net::SocketAddress;
using Poco::SharedPtr;
using std::endl;
using std::string;

const size_t BUFF_SIZE = 1024;

Cin & operator>>(Cin &stream, SocketAddress &address) {
	string rawAddress;
	stream >> rawAddress;
	address = SocketAddress(rawAddress);
	return stream;
}

class Server {
public:
	Server(SharedPtr<SocketUdp> udp, SocketAddress clientAddress)
		: _clientAddress(clientAddress)
		, _udp(udp)
	{}
	void processPaccket(const BufferRef &packet) {
		_udp->sendTo(packet.data(), packet.size(), _clientAddress);
	}
private:
	SocketAddress _clientAddress;
	SharedPtr<SocketUdp> _udp;
};

int main() {
	try {
		SharedPtr<DispatcherEPoll> dispatcher = new DispatcherEPoll();
		StopBySignals appExit(dispatcher);
		Cout cout(dispatcher);
		Cin cin(dispatcher);
		cin.tie(&cout);
		SharedPtr<SocketUdp> udpServer = new SocketUdp(dispatcher);
		cout << "Enter server port: " << endl;
		unsigned short serverPort;
		cin >> serverPort;
		udpServer->bind(SocketAddress(serverPort));
		cout << "Enter client address:" << endl;
		SocketAddress clientAddress;
		cin >> clientAddress;
		Server server(udpServer, clientAddress);
		udpServer->packetReceived += delegate(&server, &Server::processPaccket);
		dispatcher->run();
	} catch(const CanceledException &ex) {
		return 0;
	}
	return 1;
}


