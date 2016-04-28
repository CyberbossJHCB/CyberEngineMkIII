//! @file CYBOSXSyscalls.hpp Defines the Linux kernel Syscall table
#pragma once

namespace CYB {
	namespace Platform {
		namespace Implementation {
			class Sys {
			public:
				enum CallNumber : unsigned long long {
					YIELD = 0x2000000 + 0,
					EXIT = 0x2000000 + 1,
					KILL = 0x2000000 + 37,
					MUNMAP = 0x2000000 + 73,
					MPROTECT = 0x2000000 + 74,
					MMAP = 0x2000000 + 197,
				};
			};
		};
	};
};