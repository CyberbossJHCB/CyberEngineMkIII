#pragma once
namespace CYB {
	namespace Platform {
		namespace Implementation {
			namespace Posix {
				#ifdef CYB_BUILDING_TESTS
				typedef ::pid_t pid_t;
				#endif
				#include <errno.h>
				#include <sys/types.h>
				#include <sys/mman.h>
				#include <unistd.h>
#ifdef _WIN32
				#include <posix/signal.h>
#else
				#include <signal.h>
#endif
				#include <sched.h>
				#include <pthread.h>
				#include <dlfcn.h> 
			};
		};
	};
};

#include "CYBPosixThread.hpp"
#include "CYBPosixProcess.hpp"
#include "CYBPosixVirtualMemory.hpp"
#include "CYBPosixModule.hpp"
#include "CYBPosixMutex.hpp"