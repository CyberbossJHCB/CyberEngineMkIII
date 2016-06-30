#pragma once
namespace CYB {
	namespace Platform {
		namespace System{
			class Path;
			namespace Implementation {
				using namespace Posix;
				class Process {
				protected:
					pid_t FPID;
					int FExitCode;
					bool FExitCodeReady;
				private:
					static pid_t SpawnProcess(const Path& APath, const API::String::UTF8& ACommandLine);
				protected:
					Process() noexcept;
					Process(const Path& APath, const API::String::UTF8& ACommandLine);
					Process(const Process&) = delete;
					Process(Process&& AMove) noexcept = default;
					Process& operator=(Process&& AMove) noexcept = default;
				};
			};
		};
	};
};