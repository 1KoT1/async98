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
		SocketUdp udpClient(dispatcher);
		cout << "Enter server address: " << endl;
		SocketAddress serverAddress;
		cin >> serverAddress;
		cout << "Enter local port:" << endl;
		unsigned short localPort;
		cin >> localPort;
		udpClient.bind(SocketAddress(localPort));
		while(true) {
			cout << "Enter message" << endl;
			string message;
			cin >> message;
			udpClient.sendTo(message.c_str(), message.size(), serverAddress);
			char echoBuff[BUFF_SIZE];
			size_t n = udpClient.recv(echoBuff, BUFF_SIZE);
			cout << "Echo: " << string(echoBuff, n) << endl;
		}
	} catch(const DispatcherEPoll::CanceledException &ex) {
		return 0;
	}
	return 1;
}

