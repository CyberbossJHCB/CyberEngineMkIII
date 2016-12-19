//! @file CYBLogger.cpp Implements CYB::Engine::Logger
#include "CYB.hpp"

#include <cstdio>
#include <ctime>

CYB::Platform::System::File CYB::Engine::Logger::OpenFile(void) {
	//All allocation is done using the Logger's Allocator
	Platform::System::Path LogPath(API::Path::SystemPath::TEMPORARY);

	auto Time(time(0));   // get time now
	auto Now = localtime(&Time);

	const int Year(Now->tm_year + 1900), Month(Now->tm_mon + 1), Day(Now->tm_mday), Hour(Now->tm_hour), Min(Now->tm_min), Sec(Now->tm_sec);

	char Buffer[50];
	const auto Length(sprintf(Buffer, u8"LogFile %d-%d-%d %d:%d:%d.txt", Year, Month, Day, Hour, Min, Sec));

	API::Assert::LessThan(-1, Length);
	API::Assert::LessThan(Length, 50);

#ifdef TARGET_OS_WINDOWS
	//Code analysis is stupid
	__assume(-1 < Length && Length < 50);
#endif

	Buffer[Length] = 0;
	API::String::Static AsStatic(Buffer);
	API::String::UTF8 Formatted(AsStatic);

	LogPath.Append(std::move(Formatted), false, false);

	do {
		try {							//Yes, copy LogPath, if it throws it won't be valid anymore
			return Platform::System::File(LogPath, API::File::Mode::WRITE, API::File::Method::CREATE);
		}
		catch (CYB::Exception::SystemData& AException) {
			if (AException.FErrorCode != CYB::Exception::SystemData::FILE_EXISTS)
				throw;
		}
	} while (true);
}

CYB::Engine::Logger::Logger(API::Logger& AEmergencyLogger) :
	FHeap(Parameters::LOGGER_HEAP_INITIAL_COMMIT_SIZE),
	FContext(FHeap, AEmergencyLogger, true),
	FFile(OpenFile()),
	FQueueHead(nullptr),
	FQueueTail(nullptr),
	FCancelled(false),
	FThread(nullptr)
{
	FThread = FContext.FAllocator.NewObject<Platform::System::Thread, API::Interop::NullConstructor>(static_cast<API::Threadable&>(*this));
}

CYB::Engine::Logger::~Logger() {
	CancelThreadedOperation();
	FThread().Wait();
	EmptyQueue();
}

void CYB::Engine::Logger::EmptyQueue(void) {
	LogEntry* Queue, *NextNode;
	{
		API::LockGuard Lock(FQueueLock);
		Queue = FQueueHead;
		FQueueHead = nullptr;
		FQueueTail = nullptr;
	}

	class CleanLogEntry {
	private:
		LogEntry* const FLogEntry;
	public:
		CleanLogEntry(LogEntry* const ALogEntry, const bool ARecursiveShutdown) :
			FLogEntry(ALogEntry)
		{
			if (ARecursiveShutdown) {
				//Emergency log the message
				Context::GetContext().FLogger.Log(ALogEntry->FMessage, ALogEntry->FLevel);
				if (FLogEntry->FNext != nullptr)
					CleanLogEntry(FLogEntry->FNext, true);
			}
		}
		~CleanLogEntry() {
			Context::GetContext().FAllocator.DeleteObject<LogEntry>(FLogEntry);
		}
	};


	API::LockGuard LockFile(FFileLock);
	for (auto Node(Queue); Node != nullptr; Node = NextNode) {
		NextNode = Node->FNext;
		CleanLogEntry Cleanup(Node, false);
		const auto Len(static_cast<unsigned int>(Node->FMessage.RawLength()));
		auto Written(0U);
		do {
			const auto CurrentWrite(FFile.Write(Node->FMessage.CString() + Written, Len - Written));
			if (CurrentWrite == 0) {
				//CurrentContext will most certainly be FContext at this point
				//but just in case we add some weird behaviour overrides in the future....
				auto& EmergencyLogger(Context::GetContext().FLogger);
				EmergencyLogger.Log(API::String::Static(u8"Failed to write to primary log. Message follows:"), Level::ERR);
				EmergencyLogger.Log(Node->FMessage, Node->FLevel);
				if (NextNode != nullptr) {
					EmergencyLogger.Log(API::String::Static(u8"Remaining entries follow:"), Level::ERR);
					CleanLogEntry(NextNode, true);
				}
				throw CYB::Exception::SystemData(CYB::Exception::SystemData::STREAM_NOT_WRITABLE);
			}

		} while (Written < Len);
		API::Assert::Equal(Written, Len);
	}
}

void CYB::Engine::Logger::BeginThreadedOperation(void) {
	FContext.MakeCurrent();
	while (!FCancelled.load(std::memory_order_relaxed)) {
		EmptyQueue();
		Platform::System::Thread::Sleep(1);
	}
}

void CYB::Engine::Logger::CancelThreadedOperation(void) {
	FCancelled.store(true, std::memory_order_relaxed);
}

void CYB::Engine::Logger::Log(const API::String::CStyle& AMessage, const Level ALevel) noexcept {
	PushContext Push(FContext);	//Use ourselves for allocation
	bool CritFail(false);
	while (!CritFail) {
		try {

			//! @todo Format this

			auto Entry(static_cast<Allocator&>(Context::GetContext().FAllocator).RawObject<LogEntry>());
			Entry->FNext = nullptr;
			Entry->FMessage = AMessage;
			Entry->FLevel = ALevel;

			API::LockGuard Lock(FQueueLock);
			if (FQueueTail)
				FQueueTail->FNext = Entry;
			else
				FQueueHead = Entry;
			FQueueTail = Entry;
			break;
		}
		catch (CYB::Exception::SystemData& AException) {
			API::Assert::Equal<unsigned int>(AException.FErrorCode, CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE);
			try {
				//Empty the queue, free the memory, and try again
				EmptyQueue();
			}
			catch(CYB::Exception::SystemData& AInnerException){
				API::Assert::Equal<unsigned int>(AException.FErrorCode, CYB::Exception::SystemData::STREAM_NOT_WRITABLE);
				//Now give up
				CritFail = true;
			}
		}
	}
}

const CYB::API::String::CStyle& CYB::Engine::Logger::CurrentLog(void) const noexcept {
	return FFile.GetPath()();
}