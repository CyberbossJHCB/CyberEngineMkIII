//! @file UTF8String.hpp Defines CYB::API::String::Dynamic
#pragma once
namespace CYB {
	namespace API {
		namespace String {
			//! @brief Used for string of allocated variable length
			class Dynamic : public CStyle {
			private:
				/*!
					@brief Allocates data for and initializes a copy of a CStyle string
					@param AData The data to copy
					@return An allocated char array that contains the contents of AData
					@par Thread Safety
						This function requires no thread safety
					@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE. Thrown if the current hep runs out of memory
				*/
				static char* CopyCStyle(const CStyle& AData);

				/*!
					@brief Frees the data of the current string
					@par Thread Safety
						This function requires synchronization at the object level
					@throws CYB::Exception::Violation Error code: CYB::Exception::Violation::INVALID_HEAP_BLOCK. Thrown if heap memory has been corrupted
				*/
				void DeallocateData(void);
			protected:
				/*!
					@brief Construct a Dynamic string from a pre-allocated char array
					@param AData The pre-allocated char array, will be deallocated along with the Dynamic
					@par Thread Safety
						This function requires no thread safety
				*/
				Dynamic(char* const AData) noexcept;
			public:
				/*!
					@brief Allocate a char array that is @p ASize bytes in length. Call @p APopulateData with the char array as a parameter and then calculate the length
					@param ASize The amount of bytes to preallocate
					@param APopulateData The data population function, takes a void* as an argument and has a length of @p ASize and returns a bool
					@tparam ALambda The type of @p APopulateData
					@param ADynamic The Dynamic to instantiate
					@return The return value of @p APopulateData
					@par Thread Safety
						This function requires no thread safety
					@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE. Thrown if the current heap runs out of memory. ADynamic will not be modified
				*/
				template <typename ALambda> static bool BuildAndPopulateBuffer(const int ASize, const ALambda APopulateData, Dynamic& ADynamic);

				/*!
					@brief Construct an empty Dynamic string
					@par Thread Safety
						This function requires no thread safety
				*/
				Dynamic() noexcept;
				/*!
					@brief Construct a Dynamic string from a char array
					@param AData The char array
					@par Thread Safety
						This function requires no thread safety
					@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE. Thrown if the current heap runs out of memory
				*/
				Dynamic(const char* const AData);
				/*!
					@brief Construct a Dynamic string. This will allocate enough data to copy the contents of @p AData
					@param AData The data to populate the char array with. Must be at least a pointer to a null terminating character
					@par Thread Safety
						This function requires no thread safety
					@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE. Thrown if the current heap runs out of memory
				*/
				Dynamic(const CStyle& AData);
				/*!
					@brief See @ref structors
					@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE. Thrown if the current heap runs out of memory
				*/
				Dynamic(const Dynamic& ACopy);
				Dynamic(Dynamic&& AMove) noexcept;	//!< @brief See @ref structors
				/*!
					@brief See @ref structors
					@throws CYB::Exception::Violation Error code: CYB::Exception::Violation::INVALID_HEAP_BLOCK. Thrown if heap memory has been corrupted
				*/
				Dynamic& operator=(Dynamic&& AMove) RELEASE_NOEXCEPT;
				/*!
					@brief Frees allocated string
					@throws CYB::Exception::Violation Error code: CYB::Exception::Violation::INVALID_HEAP_BLOCK. Thrown if heap memory has been corrupted
				*/
				~Dynamic();

				/*!
					@brief Appends to a Dynamic string and returns a new one
					@param ARHS The string to append
					@return The new string
					@par Thread Safety
						This function requires synchronization at the object level
					@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE. Thrown if the current heap runs out of memory
				*/
				Dynamic operator+(const CStyle& ARHS) const;
				/*!
					@brief Appends to a Dynamic string in place
					@param ARHS The string to append
					@return A self reference
					@par Thread Safety
						This function requires synchronization at the object level
					@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE. Thrown if the current heap runs out of memory
				*/
				Dynamic& operator+=(const CStyle& ARHS);

				/*!
					@brief Move the null terminator of the string so that the readable length is @p AMaxBytes
					@param AMaxBytes The maximum index of the new null terminator
					@par Thread Safety
						This function requires synchronization at the object level
				*/
				virtual void Shrink(const int AMaxBytes) noexcept;

				//! @copydoc CYB::API::String::CStyle::Length()
				int Length(void) const noexcept override;
			};
		};
	};
};