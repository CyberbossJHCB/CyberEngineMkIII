//! @file LockGuard.hpp Defines the LockGuard class
#pragma once

namespace CYB {
	namespace API {
		//! @brief A RAII locking mechanism
		class LockGuard {
		private:
			Mutex* FMutex;	//!< @brief A reference to the owned mutex
		public:
			/*! 
				@brief Construct a LockGuard that will hold a lock on @p AMutex
				@param AMutex The mutex to lock
				@par Thread Safety
					This function will attempt to lock AMutex
				@par Exception Safety
					This function does not throw exceptions
			*/
			LockGuard(Mutex& AMutex);
			LockGuard(const LockGuard&) = delete;	//!< @brief See @ref structors
			LockGuard(LockGuard&& AMove);	//!< @brief See @ref structors
			LockGuard& operator=(LockGuard&& AMove);	//!< @brief See @ref structors
			/*!
				@brief Destroys the LockGuard and unlocks the Mutex
			*/
			~LockGuard();
		};
	}
}

#include "LockGuard.inl"