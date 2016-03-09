#pragma once
//!	@file SystemChecks.hpp Checks the target that the CyberEngine component is being compiled on

#define TARGET_OS_WINDOWS //!< @brief Designates a Windows NT based operating system
#define TARGET_OS_MAC	//!< @brief Designates an OSX based operating system
#define TARGET_OS_LINUX //!< @brief Designates a Unix based operating system

#ifndef _WIN32
#undef TARGET_OS_WINDOWS
#endif
#ifndef __APPLE__
#undef TARGET_OS_MAC
#endif
#ifndef __linux__
#undef TARGET_OS_LINUX
#endif

#if !defined(TARGET_OS_WINDOWS) && !defined(TARGET_OS_MAC) && !defined(TARGET_OS_LINUX)
#error CyberEngineMkII must be compiled on Windows, OSX, or Linux
#endif
#if !defined(_M_X64) && !defined(__x86_64__)
#error CyberEngineMkII must be compiled on an amd64 architecture CPU
#endif

#include <cstddef>

static_assert(
	sizeof(unsigned char) == 1
	&& sizeof(char) == 1
	&& sizeof(unsigned short) == 2
	&& sizeof(short) == 2
	&& sizeof(unsigned int) == 4
	&& sizeof(int) == 4
	&& sizeof(unsigned long long) == 8
	&& sizeof(long long) == 8
	&& sizeof(size_t) == 8
	&& sizeof(void*) == 8,
	"A type size assertion failed");