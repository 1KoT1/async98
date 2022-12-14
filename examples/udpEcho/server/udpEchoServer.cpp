#include <Coroutine03/Core/Cin.h>
#include <Coroutine03/Core/Cout.h>
#include <Coroutine03/Core/DicpatcherEPoll.h>
#include <Coroutine03/Core/StopBySignals.h>
#include <Coroutine03/Net/SocketUdp.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/SharedPtr.h>
#include <string>

using Poco::Net::SocketAddress;
using Coroutine03::Core::Cin;
using Coroutine03::Core::Cout;
using Coroutine03::Core::DispatcherEPoll;
using Coroutine03::Core::StopBySignals;
using Coroutine03::Net::SocketUdp;
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

int main() {
	try {
		SharedPtr<DispatcherEPoll> dispatcher = new DispatcherEPoll();
		StopBySignals appExit(dispatcher);
		Cin cin(dispatcher);
		Cout cout(dispatcher);
		SocketUdp udpServer(dispatcher);
		cout << "Enter server port: " << endl;
		unsigned short serverPort;
		cin >> serverPort;
		udpServer.bind(SocketAddress(serverPort));
		cout << "Enter client address:" << endl;
		SocketAddress clientAddress;
		cin >> clientAddress;
		while(true) {
			char echoBuff[BUFF_SIZE];
			size_t n = udpServer.recv(echoBuff, BUFF_SIZE);
			udpServer.sendTo(echoBuff, n, clientAddress);
		}
	} catch(const DispatcherEPoll::CanceledException &ex) {
		return 0;
	}
	return 1;
}


