#include "TestHeader.hpp"
#ifndef TARGET_OS_WINDOWS
#pragma clang diagnostic ignored "-Wdelete-non-virtual-dtor"
#endif

bool BasicCmp(const char* const A, const char* const B) {
	return CYB::API::String::Static(A) == CYB::API::String::Static(B);
}

Fake::Allocator::Allocator() :
	CYB::API::Interop::Allocator(*new Heap())
{}
Fake::Allocator::~Allocator() {
	delete &FHeap;
}

CYB::API::Interop::Allocatable* Fake::Allocator::NewObject(const CYB::API::Interop::Allocatable::ID, const CYB::API::Interop::EmptyConstructor&) {
	CYB::API::Assert::HCF();
}

template<> template<> void CYB::API::Singleton<CYB::Engine::Core>::Backdoor<void*>(void*& AHooker) {
	FSingleton = static_cast<CYB::Engine::Core*>(AHooker);
}

Fake::Core::Core(const unsigned long long ASize) {
	ResetToFakeCorePointer();
	static_cast<void>(ASize);
	//K32 dependancy
//	new (&CYB::Core().FHeap) CYB::Engine::Memory::Heap(ASize);
	FAllocator = new Allocator();
}
Fake::Core::~Core() {
	delete FAllocator;
}

Fake::Core FFakeCore(CYB::API::ByteConverters::Megabytes(50));

void Fake::Core::ResetToFakeCorePointer(void) {
	auto ref(static_cast<void*>(FFakeCore.FBytes));
	CYB::API::Singleton<CYB::Engine::Core>::Backdoor(ref);
}

void Fake::Core::NullifySingleton(void) {
	void* Null(nullptr);
	CYB::API::Singleton<CYB::Engine::Core>::Backdoor(Null);
}

void* Fake::Heap::Alloc(const int AAmount) {
	return malloc(static_cast<size_t>(AAmount));
}
void* Fake::Heap::Realloc(void* const AOld, const int ANewSize) {
	return realloc(AOld, static_cast<size_t>(ANewSize));
}
void Fake::Heap::Free(void* const AOld) noexcept {
	free(AOld);
}

static bool _CallRedirected(const CYB::Platform::System::Sys::CallNumber ACallNumber) noexcept {
	for (auto Current(Fake::SysCalls::FMapList); Current != nullptr; Current = Current->FNext)
		if (Current->FCallNumber == ACallNumber)
			return true;
	return false;
}

bool CYB::Platform::System::Sys::CallRedirected(const CallNumber ACallNumber) noexcept {
	return _CallRedirected(ACallNumber);
}

Fake::SysCalls::MapList* Fake::SysCalls::FMapList(nullptr);

unsigned long long CYB::Platform::System::Sys::RedirectedCall(const CallNumber ACallNumber, const Union64 AArg1, const Union64 AArg2, const Union64 AArg3, const Union64 AArg4, const Union64 AArg5, const Union64 AArg6) {
	Fake::SysCalls::Args Args{ AArg1, AArg2, AArg3, AArg4, AArg5, AArg6 };
	for (auto Current(Fake::SysCalls::FMapList); Current != nullptr; Current = Current->FNext)
		if (Current->FCallNumber == ACallNumber)
			return Current->FFunction(Args);
	API::Assert::HCF();
}

void Fake::SysCalls::OverrideCall(const CYB::Platform::System::Sys::CallNumber ACallNumber, CallPointer ANewFunction) {
	CYB::API::Assert::False(_CallRedirected(ACallNumber));
	FMapList = new MapList{ FMapList, ANewFunction, ACallNumber };
}

void Fake::SysCalls::ResetCall(const CYB::Platform::System::Sys::CallNumber ACallNumber) {
	CYB::API::Assert::True(_CallRedirected(ACallNumber));
	for (MapList* Current(FMapList), *Previous(nullptr); Current != nullptr; Previous = Current, Current = Current->FNext)
		if (Current->FCallNumber == ACallNumber) {
			if (Previous == nullptr)
				FMapList = Current->FNext;
			else
				Previous->FNext = Current->FNext;
			delete Current;
			break;
		}
}

SysCallOverride::SysCallOverride(const CYB::Platform::System::Sys::CallNumber ACallNumber, Fake::SysCalls::CallPointer ANewFunction):
	FCallNumber(ACallNumber)
{
	Fake::SysCalls::OverrideCall(ACallNumber, ANewFunction);
}
SysCallOverride::~SysCallOverride() {
	Fake::SysCalls::ResetCall(FCallNumber);
}