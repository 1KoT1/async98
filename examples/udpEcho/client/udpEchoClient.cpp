#include <Asynch98/Core/Cin.h>
#include <Asynch98/Core/Cout.h>
#include <Asynch98/Core/DicpatcherEPoll.h>
#include <Asynch98/Core/StopBySignals.h>
#include <Asynch98/Net/SocketUdp.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/SharedPtr.h>
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
	} catch(const CanceledException &ex) {
		return 0;
	}
	return 1;
}

