﻿//! @file CYBException.cpp Exception constructors of the non-unit kind
#include "CYB.hpp"

namespace CYB {
	namespace Exception {
		//! @brief Used to better verify tests, should be optimized out by the linker. See: http://stackoverflow.com/questions/1229430/how-do-i-prevent-my-unused-global-variables-being-compiled-out
		thread_local unsigned int FLastInstantiatedExceptionCode;
	};
};

CYB::API::String::Static CYB::Exception::Violation::ErrorMessage(const ErrorCode AErrorCode) noexcept {
	API::Assert::LessThanOrEqual(AErrorCode, UNSUPPORTED_ALLOCATION_AMOUNT);
	API::String::Static ErrorMessage;
	switch (AErrorCode)	{
	case INVALID_ENUM:
		ErrorMessage = API::String::Static(u8"An operation was attempted with an invalid enum code");
		break;
	case INVALID_INTEROP_CONSTRUCTOR:
		ErrorMessage = API::String::Static(u8"An engine allocation was attempted with a malformed constructor");
		break;
	case INVALID_OPERATION:
		ErrorMessage = API::String::Static(u8"An illegal call was made given prexisting conditions");
		break;
	case INVALID_PARAMETERS:
		ErrorMessage = API::String::Static(u8"The current arrangment of arguments passed the function is invalid");
		break;
	case NEGATIVE_HEAP_ALLOCATION:
		ErrorMessage = API::String::Static(u8"Alloc or Realloc called with a negative size value");
		break;
	case UNSUPPORTED_ALLOCATION_AMOUNT:
		ErrorMessage = API::String::Static(u8"An allocation was attempted with a size greater than 2047MB");
		break;
	}
	return ErrorMessage;
}
CYB::Exception::Violation::Violation(const ErrorCode AErrorCode) noexcept :
	Base(ErrorMessage(AErrorCode), AErrorCode, Level::VIOLATION)
{
	FLastInstantiatedExceptionCode = AErrorCode;
}

CYB::API::String::Static CYB::Exception::SystemData::ErrorMessage(const ErrorCode AErrorCode) noexcept {
	API::Assert::LessThanOrEqual(AErrorCode, THREAD_CREATION_FAILURE);
	API::String::Static ErrorMessage;
	switch (AErrorCode) {
	case DIRECTORY_NOT_EMPTY:
		ErrorMessage = API::String::Static(u8"Tried to delete a non-empty directory");
		break;
	case FILE_EXISTS:
		ErrorMessage = API::String::Static(u8"Tried to exclusively create a file that exists");
		break;
	case FILE_NOT_FOUND:
		ErrorMessage = API::String::Static(u8"A required file was not found.");
		break;
	case STREAM_NOT_READABLE:
		ErrorMessage = API::String::Static(u8"Generic error for read failures. See functions for further documentation");
		break;
	case STREAM_NOT_WRITABLE:
		ErrorMessage = API::String::Static(u8"Generic error for write failures. See functions for further documentation");
		break;
	case HEAP_ALLOCATION_FAILURE:
		ErrorMessage = API::String::Static(u8"Current heap has no block large enough for a requested allocation and expansion failed");
		break;
	case MUTEX_INITIALIZATION_FAILURE:
		ErrorMessage = API::String::Static(u8"Failed to initialize a new mutex.");
		break;
	case PATH_LOST:
		ErrorMessage = API::String::Static(u8"Path has become invalidated, most likely due to deletion");
		break;
	case PATH_TOO_LONG:
		ErrorMessage = API::String::Static(u8"Attempted to use a path greater than the maximum allowed byte value");
		break;
	case STRING_VALIDATION_FAILURE:
		ErrorMessage = API::String::Static(u8"A string could not be validated");
		break;
	case SYSTEM_PATH_RETRIEVAL_FAILURE:
		ErrorMessage = API::String::Static(u8"A system path could not be retrieved");
		break;
	case THREAD_CREATION_FAILURE:
		ErrorMessage = API::String::Static(u8"OS failed to create requested thread.");
		break;
	}
	return ErrorMessage;
}

CYB::Exception::SystemData::SystemData(const ErrorCode AErrorCode) noexcept :
	Base(ErrorMessage(AErrorCode), AErrorCode, Level::SYSTEM_DATA)
{
	FLastInstantiatedExceptionCode = AErrorCode;
}
CYB::API::String::Static CYB::Exception::Internal::ErrorMessage(const ErrorCode AErrorCode) noexcept {
	API::Assert::LessThanOrEqual(AErrorCode, PROCESS_TERMINATION_ERROR);
	API::String::Static ErrorMessage;
	switch (AErrorCode) {
	case MEMORY_COMMITAL_FAILURE:
		ErrorMessage = API::String::Static(u8"Failed to commit virtual memory from the OS.");
		break;
	case MEMORY_PROTECT_FAILURE:
		ErrorMessage = API::String::Static(u8"Failed to implement requested virtual memory protection level.");
		break;
	case MEMORY_RESERVATION_FAILURE:
		ErrorMessage = API::String::Static(u8"Failed to reserve virtual memory from the OS.");
		break;
	case MODULE_FUNCTION_LOAD_FAILURE:
		ErrorMessage = API::String::Static(u8"Failed to load a requested function from a module.");
		break;
	case MODULE_LOAD_FAILURE:
		ErrorMessage = API::String::Static(u8"Failed to load a requested module.");
		break;
	case PATH_EVALUATION_FAILURE:
		ErrorMessage = API::String::Static(u8"Failed to evaluate a Path.");
		break;
	case PROCESS_CREATION_ERROR:
		ErrorMessage = API::String::Static(u8"Failed to create Process for unknown reasons.");
		break;
	case PROCESS_EXIT_CODE_UNCHECKABLE:
		ErrorMessage = API::String::Static(u8"Tried to check the error code of a Process the OS would not allow.");
		break;
	case PROCESS_TERMINATION_ERROR:
		ErrorMessage = API::String::Static(u8"Failed to terminate process, most likely due to insufficient permissions.");
		break;
	};
	return ErrorMessage;
}

CYB::Exception::Internal::Internal(const ErrorCode AErrorCode) noexcept :
	Base(ErrorMessage(AErrorCode), AErrorCode, Level::INTERNAL)
{
	FLastInstantiatedExceptionCode = AErrorCode;
}