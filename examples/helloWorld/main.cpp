#include <Coroutine03/Core/Cin.h>
#include <Coroutine03/Core/Cout.h>
#include <Coroutine03/Core/DicpatcherEPoll.h>
#include <iostream>
#include <Poco/SharedPtr.h>
#include <string>

using Coroutine03::Core::Cin;
using Coroutine03::Core::Cout;
using Coroutine03::Core::DispatcherEPoll;
using Poco::SharedPtr;
using std::endl;
using std::string;

int main() {
	SharedPtr<DispatcherEPoll> dispatcher = new DispatcherEPoll();
	Cin cin(dispatcher);
	Cout cout(dispatcher);
	cout << "What is your name?" << endl;
	string name;
	cin >> name;
	cout << "Hello " << name << "!" << endl;
	return 0;
}
