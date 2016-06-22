//! @file CYBThread.hpp Contains the platform thread definition
#pragma once
namespace CYB {
	namespace Platform {
		namespace System {
			//! @brief Definition of a platform thread object
			class Thread : private Implementation::Thread {
			public:
				//! @brief The states of a thread
				enum class State {
					IDLE,		//!< @brief The thread is not running and can be started
					RUNNING,	//!< @brief The thread is running
					FINISHED,	//!< @brief The thread has finished running
				};
			private:
				API::Threadable& FThreadable;	//!< @brief A reference to the Threadable object this Thread will run
				bool FCancelSubmitted;	//!< @brief A check to make sure FThreadable.CancelThreadedOperation is not called more than once
			public:
				/*!
					@brief Put the thread to sleep for at least @p AMilliseconds
					@param AMilliseconds The minimum number of milliseconds to sleep for
					@par Thread Safety
						This function requires no thread safety				*/
				static void Sleep(const unsigned int AMilliseconds) noexcept;
				/*!
					@brief Expires the current threads processor time and sumbits it to the OS for rescheduling
					@par Thread Safety
						This function requires no thread safety				*/
				static void Yield(void) noexcept;
				/*!
					@brief Construct a Thread. Once returned, the thread will be scheduled to run and BeginThreadedOperation will soon be called on AThreadable
					@param AThreadable A reference to the threadable object the Thread will run, must remain valid for the lifetime of the Thread's run
					@par Thread Safety
						This function requires no thread safety
					@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::ErrorCode::THREAD_CREATION_FAILURE. Thrown if the thread is unable to be created
				*/
				Thread(API::Threadable& AThreadable);
				Thread(const Thread&) = delete;
				//! @brief Destroy a thread. Will block the caller until the owned thread terminates
				~Thread();

				/*!
					@brief Cancel a thread. Calls CancelThreadedOperation on the assigned Threadable in the current thread if IsFinished returns false
					@par Thread Safety
						This function requires synchronized access at the object level
				*/
				void Cancel(void) noexcept;
				/*!
					@brief Check whether or not the owned thread is terminated
					@return true if a call to Wait will immediately return, false otherwise
					@par Thread Safety
						This function requires no thread safety				*/
				bool IsFinished(void) const noexcept;
				/*!
					@brief Blocks current thread until the owned thread has terminated
					@par Thread Safety
						This function requires no thread safety				*/
				void Wait(void) const noexcept;
			};
		};
	};
};