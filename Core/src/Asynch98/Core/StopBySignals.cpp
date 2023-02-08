#include "Asynch98/Core/StopBySignals.h"
#include <csignal>
#include <cstring>
#include <Poco/Format.h>
#include <Poco/Logger.h>
#include <Poco/SharedPtr.h>
#include <stdexcept>
#include <sys/signalfd.h>

using Poco::format;
using Poco::Logger;
using Poco::SharedPtr;
using std::runtime_error;
using std::string;

namespace Asynch98 {
	namespace Core {

		const int CREATE_NEW = -1;

		int createSignalsFd() {
			sigset_t sset;
			sigaddset(&sset, SIGINT);
			sigaddset(&sset, SIGQUIT);
			sigaddset(&sset, SIGTERM);
			pthread_sigmask(SIG_BLOCK, &sset, NULL);
			int fd = signalfd(
				CREATE_NEW,
				&sset,
				SFD_CLOEXEC // does not inherited by child processes
				|
				SFD_NONBLOCK // for simple implement a reading of all received signals
				);
			if(fd == -1) {
				throw SignalFDFail();
			}
			return fd;
		}

		StopBySignals::StopBySignals(Poco::SharedPtr<DispatcherEPoll> dispatcher)
			: HandlerEPollEvents(dispatcher, createSignalsFd(), EPOLLIN)
			, _log(Logger::root())
		{}

		StopBySignals::~StopBySignals() {
			if(close(_fd) == -1) {
				poco_error_f2(_log, "Fail of close the signals fd. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0))));
			}
		}

		void StopBySignals::handle(EPoll::Events events) {
			struct signalfd_siginfo buff;
			int n;
			// Read all before a signals queue be empty.
			// We can don't handle different types of signals because
			// all signals in the set are signals of an app close.
			// We only should read all data for don't make an infinite
			// loop on the epoll_wait method.
			do {
				n = read(_fd, &buff, sizeof(buff));
			} while (n == sizeof(buff));
			if(n == 0 || (n == -1 && errno == EAGAIN)) {
				_dispatcher->stop();
			} else {
				poco_error_f2(_log, "Fail of a read signal information. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0))));
			}
		}


		SignalFDFail::SignalFDFail()
			: runtime_error(format("Fail of the signalfd method. Errno: %d, %s", errno, string(strerror_l(errno, static_cast<locale_t>(0))))) 
		{}

	} // namespace Core
} // namespace Asynch98
