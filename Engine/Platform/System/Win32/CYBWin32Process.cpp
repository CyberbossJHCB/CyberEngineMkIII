#include "CYB.hpp"

using namespace CYB::Platform::Win32;

void CYB::Platform::System::Process::Terminate(void) {
	//Self terminating is actually safer than exiting due to locks and shit, also allows for us to kill ourselves and other processes in one line
	//No FK32 because this can be called without Core
	if(Sys::Call(Sys::TERMINATE_PROC, FHandle) == 0)
		throw Exception::Internal(Exception::Internal::PROCESS_TERMINATION_ERROR);
}

//No FK32 because this can be called without Core
CYB::Platform::System::Implementation::Process::Process() noexcept :
	FHandle(reinterpret_cast<Win32::HANDLE>(System::Sys::Call(Sys::GET_CURRENT_PROCESS)))
{}

CYB::Platform::System::Implementation::Process::Process(Process&& AMove) noexcept :
	FHandle(AMove.FHandle)
{
	AMove.FHandle = nullptr;
}

CYB::Platform::System::Implementation::Process& CYB::Platform::System::Implementation::Process::operator=(Process&& AMove) noexcept {
	FHandle = AMove.FHandle;
	AMove.FHandle = nullptr;
	return *this;
}

CYB::Platform::System::Process::~Process() {
	if(FHandle != nullptr)
		Core().FModuleManager.Call<Modules::Kernel32::CloseHandle>(FHandle);
}

HANDLE CYB::Platform::System::Implementation::Process::CreateProcess(const CYB::Platform::System::Path& APath, const CYB::API::String::UTF8& ACommandLine) {

	CYB::API::String::UTF16 ExeAs16(APath()), CmdlAs16(ACommandLine);
	STARTUPINFO StartupInfo{ sizeof(STARTUPINFO), nullptr, nullptr, nullptr, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr, 0, 0, 0};
	PROCESS_INFORMATION ProcessInformation;
	auto& MM(CYB::Core().FModuleManager);

	const auto Result(MM.Call<CYB::Platform::Modules::Kernel32::CreateProcessW>(ExeAs16.WString(), CmdlAs16.WideData(), nullptr, nullptr, FALSE, DWORD(0), nullptr, nullptr, &StartupInfo, &ProcessInformation));

	if (Result == 0)
		switch (MM.Call<CYB::Platform::Modules::Kernel32::GetLastError>()) {
		case ERROR_ACCESS_DENIED:
			throw CYB::Exception::SystemData(CYB::Exception::SystemData::STREAM_NOT_READABLE);
		case ERROR_FILE_NOT_FOUND:
			throw CYB::Exception::SystemData(CYB::Exception::SystemData::FILE_NOT_FOUND);
		case ERROR_ELEVATION_REQUIRED:
			//try again with elevation by using shell execute
		{
			SHELLEXECUTEINFO ShellExecuteInfo{ sizeof(SHELLEXECUTEINFO), SEE_MASK_NOASYNC | SEE_MASK_NOCLOSEPROCESS, nullptr, L"runas", ExeAs16.WString(), CmdlAs16.WString(), nullptr, SW_SHOWNORMAL, 0, nullptr, nullptr, 0 };
			ShellExecuteInfo.hIcon = 0;
			ShellExecuteInfo.hMonitor = 0;
			ShellExecuteInfo.hProcess = 0;
			const auto Result2(MM.Call<CYB::Platform::Modules::Shell::ShellExecuteExW>(&ShellExecuteInfo));
			if (Result2 == TRUE) {
				CYB::API::Assert::LessThan(reinterpret_cast<HINSTANCE>(32), ShellExecuteInfo.hInstApp);
				CYB::API::Assert::NotEqual(ShellExecuteInfo.hProcess, static_cast<HANDLE>(nullptr));
				return ShellExecuteInfo.hProcess;
			}
		}
		default:
			throw CYB::Exception::Internal(CYB::Exception::Internal::PROCESS_CREATION_ERROR);
		}

	MM.Call<CYB::Platform::Modules::Kernel32::CloseHandle>(ProcessInformation.hThread);
	return ProcessInformation.hProcess;
}

CYB::Platform::System::Implementation::Process::Process(const System::Path& APath, const API::String::UTF8& ACommandLine):
	FHandle(CreateProcess(APath, ACommandLine))
{}

bool CYB::Platform::System::Process::Active(void) const noexcept {
	return Core().FModuleManager.Call<Modules::Kernel32::WaitForSingleObject>(FHandle, Win32::DWORD(0)) == WAIT_TIMEOUT;
}

bool CYB::Platform::System::Process::operator==(const Process& ARHS) const noexcept {
	if (Active() && ARHS.Active()) {
		const auto Result(Core().FModuleManager.Call<Modules::Kernel32::GetProcessId>(FHandle));
		if (Result != 0)
			return Result == Core().FModuleManager.Call<Modules::Kernel32::GetProcessId>(ARHS.FHandle);
	}
	return false;
}

bool CYB::Platform::System::Process::Wait(const unsigned int AMilliseconds) {
	const auto Result(Core().FModuleManager.Call<Modules::Kernel32::WaitForSingleObject>(FHandle, AMilliseconds == 0 ? INFINITE : AMilliseconds));
	API::Assert::NotEqual<decltype(Result)>(Result, WAIT_FAILED);
	return Result == WAIT_OBJECT_0;
}

int CYB::Platform::System::Process::GetExitCode(void) {
	Wait();
	DWORD Result;
	auto& MM(Core().FModuleManager);
	if (MM.Call<Modules::Kernel32::GetExitCodeProcess>(FHandle, &Result) == 0) {
		const auto ErrorCode(MM.Call<Modules::Kernel32::GetLastError>());
		API::Assert::NotEqual<decltype(ErrorCode)>(ErrorCode, ERROR_INVALID_HANDLE);
		throw Exception::Internal(Exception::Internal::PROCESS_EXIT_CODE_UNCHECKABLE);
	}
	return static_cast<int>(Result);
}