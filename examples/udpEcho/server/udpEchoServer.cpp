#include <Asynch98/Core/Cin.h>
#include <Asynch98/Core/Cout.h>
#include <Asynch98/Core/DicpatcherEPoll.h>
#include <Asynch98/Core/StopBySignals.h>
#include <Asynch98/Net/SocketUdp.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/SharedPtr.h>
#include <iostream>
#include <string>

using Asynch98::Core::CanceledException;
using Asynch98::Core::Cin;
using Asynch98::Core::Cout;
using Asynch98::Core::DispatcherEPoll;
using Asynch98::Core::StopBySignals;
using Asynch98::Net::SocketUdp;
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

int main() {
	try {
		SharedPtr<DispatcherEPoll> dispatcher = new DispatcherEPoll();
		StopBySignals appExit(dispatcher);
		Cout cout(dispatcher);
		Cin cin(dispatcher);
		cin.tie(&cout);
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
	} catch(const CanceledException &ex) {
		return 0;
	}
	return 1;
}


