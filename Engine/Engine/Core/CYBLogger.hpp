//! @file CYBLogger.hpp Defines CYB::Engine::Logger
#pragma once

namespace CYB {
	namespace Engine {
		//! @brief The threaded, queued engine logger
		class Logger : public API::Logger, private API::Threadable {
			ENABLE_TEST_HOOKS
		private:
			//! @brief Log entries to be inserted into the queue
			struct LogEntry {
				API::UniquePointer<LogEntry> FNext;	//!< @brief Next item in the linked list
				API::String::Dynamic FMessage;	//!< @brief The pre-formatted text of the log
				Level FLevel;	//!< @brief The Level of the message
			};
		private:
			Memory::Heap FHeap;	//!< @brief The isolated Heap
			Context FContext;	//!< @brief The Context to be used when calling the Logger

			Platform::System::File FFile;	//!< @brief The File being written to
			mutable Platform::System::Mutex FQueueLock,	//!< @brief The lock used to acquire access to FQueue
				FFileLock;	//!< @brief The lock used to acquire access to FFile
			
			API::UniquePointer<LogEntry> FQueueHead;	//!< @brief The message queue head
			LogEntry* FQueueTail;	//!< @brief The message queue tail
			API::Interop::Object<Platform::System::Thread> FThread;	//!< @brief The thread used for writing to the log file

			std::atomic_bool FCancelled,	//!< @brief Cancel flag for FThread
				FDevLog,	//!< @brief Flag for enabling/disabling DEV logs
				FPaused;	//!< @brief Flag for sleeping the logging thread
		private:
			/*!
				@brief Retrieve a string of the given time
				@param AHour The hour to display
				@param AMinute The minute to display
				@param ASecond The second to display
				@param AColons If true, print with brackets and colons. Otherwise use dashes
				@return An API::String::Dynamic with the given time in the format "[HH:MM:SS]" or "HH-MM-SS" depending on @p AColons
				@par Thread Safety
					This function requires no thread safety
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE. Thrown if the current heap runs out of memory
			*/
			static API::String::Dynamic TimeString(const int AHour, const int AMinute, const int ASecond, const bool AColons);
			/*!
				@brief Retrieve a string of the current time
				@param AColons If true, print with brackets and colons. Otherwise use dashes
				@return An API::String::Dynamic with the given time in the format "[HH:MM:SS]" or "HH-MM-SS" depending on @p AColons
				@par Thread Safety
					This function requires no thread safety
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE. Thrown if the current heap runs out of memory
			*/
			static API::String::Dynamic TimeString(const bool AColons);

			/*!
				@brief Prepend the level and time to a log message
				@param AMessage The message to format
				@param ALevel The Level of @p AMessage
				@return @p AMessage prepended with the current time and Level string
				@par Thread Safety
					This function requires no thread safety
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE. Thrown if the current heap runs out of memory
				@throws CYB::Exception::Violation Error code: CYB::Exception::Violation::INVALID_ENUM. Thrown if the @p ALevel is invalid
			*/
			static API::String::Dynamic FormatLogMessage(const API::String::CStyle& AMessage, const Level ALevel);
			
			/*!
				@brief Prepares the logging File for writing. May block for one second if the preferred filename is taken in order to generate a new one
				@param ABasePath A Path to the directory to log in
				@return The File to be used for logging
				@par Thread Safety
					This function requires no thread safety
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::STREAM_NOT_WRITABLE. Thrown if the log File could not be opened
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::SYSTEM_PATH_RETRIEVAL_FAILURE. Thrown if the temporary Path could not be retrieved
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::STREAM_NOT_READABLE. Thrown if Path errors occured while setting up the file
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::PATH_TOO_LONG. Thrown if the new Path would exceed the limitation
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::PATH_LOST. Thrown if Path errors occured while setting up the file
				Theoretically this could throw CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE on some string allocations, but given the initial empty Heap this is practically impossible
			*/
			static Platform::System::File OpenFileImpl(const Platform::System::Path& ABasePath);
			/*!
				@brief Prepares the logging File for writing. May block for one second if the preferred filename is taken in order to generate a new one
				@return The File to be used for logging
				@par Thread Safety
					This function requires no thread safety
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::STREAM_NOT_WRITABLE. Thrown if the log File could not be opened
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::SYSTEM_PATH_RETRIEVAL_FAILURE. Thrown if the temporary Path could not be retrieved
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::STREAM_NOT_READABLE. Thrown if Path errors occured while setting up the file
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::PATH_TOO_LONG. Thrown if the new Path would exceed the limitation
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::PATH_LOST. Thrown if Path errors occured while setting up the file
				Theoretically this could throw CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE on some string allocations, but given the initial empty Heap this is practically impossible
			*/
			static Platform::System::File OpenFile(void);

			/*!
				@brief Takes a LogEntry Queue and logs it out to the current given emergency logger
				@param AEntry An xvalue of a LogEntry. All its contents will be logged and deleted
				@param AEmergency the current emergency logger
				@par Thread Safety
					This function requires no thread safety
			*/
			static void LogShutdownForEntry(API::UniquePointer<LogEntry>&& AEntry, API::Logger& AEmergency) noexcept;

			/*!
				@brief Empty FQueue and write it into FFile
				@par Thread Safety
					This function requires no thread safety
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::STREAM_NOT_WRITABLE. Thrown if the log File could not be written to
			*/
			void EmptyQueue(void);

			/*!
				@brief Returns the current object as a Threadable reference. Used to get around some weird VS construction issues
				@return *this;
				@par Thread Safety
					This function requires no thread safety
			*/
			API::Threadable& SelfAsThreadable(void) noexcept;

			/*!
				@brief Writer thread. Runs in a loop until CancelThreadedOperation is called
				@par Thread Safety
					This function requires no thread safety
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::STREAM_NOT_WRITABLE. Thrown if the log File could not be written to
			*/
			void BeginThreadedOperation(void) override;
			/*!
				@brief Stops the writer thread. Does not guarantee an empty queue
				@par Thread Safety
					This function requires no thread safety
			*/
			void CancelThreadedOperation(void) override;
		public:
			/*!
				@brief Initializes and starts the Logger. Changes the current Context. May block for one second if the preferred file name is taken in order to generate a new one
				@param AEmergencyLogger The Logger to write errors to if this one fails
				@par Thread Safety
					This function requires no thread safety
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::STREAM_NOT_WRITABLE. Thrown if the log File could not be opened
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::SYSTEM_PATH_RETRIEVAL_FAILURE. Thrown if the temporary Path could not be retrieved
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::STREAM_NOT_READABLE. Thrown if Path errors occured while setting up the file
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::STREAM_NOT_WRITABLE. Thrown if the log File could not be written to
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::PATH_TOO_LONG. Thrown if the new Path would exceed the limitation
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::PATH_LOST. Thrown if Path errors occured while setting up the file
				@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::MUTEX_INITIALIZATION_FAILURE. Thrown if one of the Mutexes could not be initialized
				@throws CYB::Exception::Internal Error code: CYB::Exception::Internal::MEMORY_RESERVATION_FAILURE. Thrown if the Heap memory could not be reserved
				@throws CYB::Exception::Internal Error code: CYB::Exception::Internal::MEMORY_COMMITAL_FAILURE. Thrown if the Heap memory could not be committed
				Theoretically this could throw CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE on some string allocations, but given the initial empty Heap this is practiacally impossible
			*/
			Logger(API::Logger& AEmergencyLogger);
			//! @brief Shutdown the Logger and empty the queue
			~Logger();

			/*!
				@brief Put the logging thread to sleep
				@par Thread Safety
					This function requires no thread safety
			*/
			void Pause(void) noexcept;
			/*!
				@brief Wake the logging thread
				@par Thread Safety
					This function requires no thread safety
			*/
			void Resume(void) noexcept;

			//! @copydoc CYB::API::Logger::Log()
			void Log(const API::String::CStyle& AMessage, const Level ALevel) final override;

			/*!
				@copydoc CYB::API::Logger::Flush()
				@attention This does not call Resume and may result in a deadlock if called while paused
			*/
			void Flush(void) const noexcept final override;

			//! @copydoc CYB::API::Logger::CurrentLog()
			const API::String::CStyle& CurrentLog(void) const noexcept final override;

			//! @copydoc CYB::API::Logger::SetDebugLogging()
			void SetDebugLogging(const bool AEnable) noexcept final override;
		};
	};
};