//! @file CYBEntry.cpp The entry point to the program. Sole purpose is to call Main::Entry and return. Also contains exceptional functions that cannot be included in main library for testing purposes

#include "../Engine/CYB.hpp"

#ifdef TARGET_OS_WINDOWS
#define main wmain	//!< @brief Wide main definition for Windows systems
#endif

/*! 
	@brief Program entry point. Call CYB::Engine::Init::Run
	@param ANumArguments The number of command line arguments
	@param AArguments The command line arguments
	@return Never returns
	@par Thread Safety
		This function may only be called once by the C++ runtime
	@par Exception Safety
		This function does not throw exceptions
*/
int main(const int ANumArguments, const oschar_t* const* const AArguments) {
	CYB::API::Assert::LessThanOrEqual(0, ANumArguments);
	CYB::Engine::Core::Run(static_cast<unsigned int>(ANumArguments), AArguments);
	CYB::API::Assert::HCF();
}

//placed here for code coverage reasons
void CYB::API::Assert::HCF[[noreturn]](void) noexcept {
	BREAK;
#ifdef TARGET_OS_WINDOWS
	__assume(false);
#else
	__builtin_unreachable();
#endif
}

bool CYB::Platform::System::Sys::CallRedirected(const CallNumber ACallNumber) noexcept {
	static_cast<void>(ACallNumber);
	return false;
}

unsigned long long CYB::Platform::System::Sys::RedirectedCall(const CallNumber, const Union64, const Union64, const Union64, const Union64, const Union64, const Union64) {
	API::Assert::HCF();
}

void CYB::API::Assert::Unimplemented(const char* const AFunction, const char* const AFile, const unsigned int ALine) noexcept {
	static_cast<void>(AFunction);
	static_cast<void>(AFile);
	static_cast<void>(ALine);
	API::Assert::HCF();
}

/*!
	@brief Forces a log write in testing, no-op in the runtime
	@param AMsg The message to log
	@par Thread Safety
		This function requires external synchronization
*/
void TestLog(const char* AMsg) noexcept {
	static_cast<void>(AMsg);
}
/*!
	@brief Forces a log write in testing, no-op in the runtime
	@param ANumber The number to log
	@par Thread Safety
		This function requires external synchronization
*/
void TestLogNum(const long long ANumber) noexcept {
	static_cast<void>(ANumber);
}

void CYB::Engine::Memory::Heap::AllocCheck(void) {}