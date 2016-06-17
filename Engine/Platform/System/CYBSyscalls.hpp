//! @file CYBSyscalls.hpp Defines the System Call interface
#pragma once

namespace CYB {
	namespace Platform {
		namespace System {
			//Note, keep public inheritance for call number access
			class Sys : public Implementation::Sys {
			public:
				union Union64 {
				public:
					void* FPointer;
					unsigned long long FNumber;
				public:
					Union64(void* const APointer) :
						FPointer(APointer)
					{}
					Union64(const unsigned long long ANumber) :
						FNumber(ANumber)
					{}
				};
			private:
				static bool CallRedirected(const CallNumber ACallNumber) noexcept;
				static unsigned long long RedirectedCall(const CallNumber ACallNumber, const Union64 AArg1 = Union64(nullptr), const Union64 AArg2 = Union64(nullptr), const Union64 AArg3 = Union64(nullptr), const Union64 AArg4 = Union64(nullptr), const Union64 AArg5 = Union64(nullptr), const Union64 AArg6 = Union64(nullptr));

				static unsigned long long DoCall(const CallNumber ACallNumber) noexcept;
				static unsigned long long DoCall(const CallNumber ACallNumber, const Union64 AArg1) noexcept;
				static unsigned long long DoCall(const CallNumber ACallNumber, const Union64 AArg1, const Union64 AArg2) noexcept;
				static unsigned long long DoCall(const CallNumber ACallNumber, const Union64 AArg1, const Union64 AArg2, const Union64 AArg3) noexcept;
				static unsigned long long DoCall(const CallNumber ACallNumber, const Union64 AArg1, const Union64 AArg2, const Union64 AArg3, const Union64 AArg4) noexcept;
				static unsigned long long DoCall(const CallNumber ACallNumber, const Union64 AArg1, const Union64 AArg2, const Union64 AArg3, const Union64 AArg4, const Union64 AArg5) noexcept;
				static unsigned long long DoCall(const CallNumber ACallNumber, const Union64 AArg1, const Union64 AArg2, const Union64 AArg3, const Union64 AArg4, const Union64 AArg5, const Union64 AArg6) noexcept;

				static unsigned long long LinkedCall(const CallNumber ACallNumber) noexcept;
				static unsigned long long LinkedCall(const CallNumber ACallNumber, const Union64 AArg1) noexcept;
				static unsigned long long LinkedCall(const CallNumber ACallNumber, const Union64 AArg1, const Union64 AArg2) noexcept;
				static unsigned long long LinkedCall(const CallNumber ACallNumber, const Union64 AArg1, const Union64 AArg2, const Union64 AArg3) noexcept;
				static unsigned long long LinkedCall(const CallNumber ACallNumber, const Union64 AArg1, const Union64 AArg2, const Union64 AArg3, const Union64 AArg4) noexcept;
				static unsigned long long LinkedCall(const CallNumber ACallNumber, const Union64 AArg1, const Union64 AArg2, const Union64 AArg3, const Union64 AArg4, const Union64 AArg5) noexcept;
				static unsigned long long LinkedCall(const CallNumber ACallNumber, const Union64 AArg1, const Union64 AArg2, const Union64 AArg3, const Union64 AArg4, const Union64 AArg5, const Union64 AArg6) noexcept;

				static void VerifyArgumentCount(const CallNumber ACallNumber, const unsigned long long ANumArgs) noexcept;
			public:
				template <typename... Args> static unsigned long long Call(const CallNumber ACallNumber, Args&&... AArgs) noexcept;
			};
		};
	};
};

#include "CYBSyscalls.inl"