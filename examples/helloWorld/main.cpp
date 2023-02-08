#include <Asynch98/Core/Cin.h>
#include <Asynch98/Core/Cout.h>
#include <Asynch98/Core/DicpatcherEPoll.h>
#include "Asynch98/Core/StopBySignals.h"
#include <exception>
#include <iostream>
#include <Poco/SharedPtr.h>
#include <string>

using Asynch98::Core::CanceledException;
using Asynch98::Core::Cin;
using Asynch98::Core::Cout;
using Asynch98::Core::DispatcherEPoll;
using Asynch98::Core::StopBySignals;
using Poco::SharedPtr;
using std::endl;
using std::string;

int main() {
	try {
		SharedPtr<DispatcherEPoll> dispatcher = new DispatcherEPoll();
		StopBySignals appExit(dispatcher);
		Cout cout(dispatcher);
		Cin cin(dispatcher);
		cin.tie(&cout);
		cout << "What is your name?" << endl;
		string name;
		cin >> name;
		cout << "Hello " << name << "!" << endl;
	} catch(const CanceledException &ex) {
		std::cout << "Stop: " << ex.what() << endl;
	}
	return 0;
}
