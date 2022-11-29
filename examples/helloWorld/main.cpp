#include <Coroutine03/Core/Cin.h>
#include <Coroutine03/Core/Cout.h>
#include <Coroutine03/Core/DicpatcherEPoll.h>
#include "Coroutine03/Core/StopBySignals.h"
#include <exception>
#include <iostream>
#include <Poco/SharedPtr.h>
#include <string>

using Coroutine03::Core::Cin;
using Coroutine03::Core::Cout;
using Coroutine03::Core::DispatcherEPoll;
using Coroutine03::Core::StopBySignals;
using Poco::SharedPtr;
using std::endl;
using std::string;

int main() {
	try {
		SharedPtr<DispatcherEPoll> dispatcher = new DispatcherEPoll();
		StopBySignals appExit(dispatcher);
		Cin cin(dispatcher);
		Cout cout(dispatcher);
		cout << "What is your name?" << endl;
		string name;
		cin >> name;
		cout << "Hello " << name << "!" << endl;
	} catch(const DispatcherEPoll::CanceledException &ex) {
		std::cout << "Stop: " << ex.what() << endl;
	}
	return 0;
}
