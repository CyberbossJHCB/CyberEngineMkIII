//! @file CYBCore.hpp Defines the engine core interface and static pointer as well as the Engine() access function
#pragma once

#include "CYBEngineParameters.hpp"
#include "CYBInterop.hpp"
#include "CYBLogger.hpp"

namespace CYB {
	namespace Engine {
		//! @brief An instance of this object is the entirety of the engine
		class Core : public API::Singleton<Core> {
			ENABLE_TEST_HOOKS
		private:
			static thread_local Context* FCurrentContext; //!< @brief The current Context in use
			static thread_local unsigned long long FThreadID;	//!< @brief ID of the checking thread

		public:
			API::EngineInformation FEngineInformation;	//!< @brief Information describing the engine

			Platform::Modules::Manager FModuleManager;	//!< @brief Loads and contains required modules
		private:
			Platform::System::Console FConsole;	//!< @brief The console

			std::atomic_uint_fast64_t FThreadCounter;	//!< @brief The number of threads created

			Logger FLogger;	//!< @brief The engine's primary Logger

			Memory::Heap FHeap;	//!< @brief The engine's primary Heap
			Context FEngineContext;	//!< @brief The engine Context
		public:
			CYB::Engine::Helpers::CommandLine FCommandLine;	//!< @brief The parsed command line handler

			//ThreadPool
			//Steam
			//Command Line
			//API
				//
			//Unit
				//Load basics
				//Lock mutex
				//Heap
		private:
			/*!
				@brief Constructs the engine components
				@param ANumArguments The number of command line arguments
				@param AArguments The command line arguments
				@par Thread Safety
					This function should only be called once
				@throws CYB::Exception::Internal Error code: CYB::Exception::Internal::MEMORY_RESERVATION_FAILURE. Thrown if a heap's memory could not be reserved
				@throws CYB::Exception::Internal Error code: CYB::Exception::Internal::MEMORY_COMMITAL_FAILURE. Thrown if a heap's memory could not be committed
			*/
			Core(const unsigned int ANumArguments, const oschar_t* const* const AArguments);
			//! @brief Cleans up the engine and terminates the process
			~Core();

			/*!
				@brief Run the main unit
				@return true if the Unit should be launch again, false otherwise
				@par Thread Safety
					This function must be synchronized at the object level
			*/
			bool LaunchUnit(void) noexcept;
		public:
			/*!
				@brief Retrieve the Core singleton
				@return A reference to the Core singleton
				@par Thread Safety
					This function requires no thread safety
			*/
			static Core& GetCore(void) noexcept;

			/*!
				@brief Runs the engine and self terminates
				@param ANumArguments The number of command line arguments
				@param AArguments The command line arguments
				@par Thread Safety
					This function should only be called once
			*/
			static void Run(const unsigned int ANumArguments, const oschar_t* const* const AArguments) noexcept;


			/*!
				@brief Get the current Context
				@return A reference to the current context
				@par Thread Safety
					This function requires no thread safety
			*/
			Context& CurrentContext(void) noexcept;
			/*!
				@brief Set the current Context
				@param ANewContext The new context
				@par Thread Safety
					This function requires no thread safety
			*/
			void SetCurrentContext(Context& ANewContext) noexcept;
			/*!
				@brief Set the current Context to FEngineContext
				@par Thread Safety
					This function requires no thread safety
			*/
			void DefaultContext(void) noexcept;

			/*!
				@brief Possibly assign and return the current thread's serial ID for this execution
				@return The current thread's serial ID for this execution
				@par Thread Safety
					This function requires no thread safety
			*/
			unsigned long long ThreadID(void) noexcept;
		};
	};

	//! @copydoc CYB::Engine::Core::GetCore
	Engine::Core& Core(void) noexcept;
};