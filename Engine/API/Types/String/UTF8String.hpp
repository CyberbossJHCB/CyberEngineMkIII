//! @file UTF8String.hpp Defines CYB::API::String::UTF8
#pragma once

namespace CYB {
	namespace API {
		namespace String {
			//! @brief A variable length UTF-8 string
			class UTF8 : public Dynamic {
			private:
				int FLength;	//!< @brief Length cache
			private:
				/*!
					@brief Gets the byte index of a given character index
					@param ACharIndex The character index
					@return The byte index of @p ACharIndex
					@par Thread Safety
						This function requires synchronization at the object level
				*/
				int ByteIndexOfChar(const int ACharIndex) const noexcept;

				/*!
					@brief Sets up the length cache variable. Assumes the string is validated
					@par Thread Safety
						This function requires synchronization at the object level
					@attention This requires FLength be set correctly
				*/
				void CalculateLength(void) noexcept;
			public:
				/*!
					@brief Validates if a given string is UTF8.
					@param AString The string to validate
					@return true if @p AString validated, false otherwise
					@par Thread Safety
						This function requires synchronization of AString at the object level
				*/
				static bool Validate(const CStyle& AString) noexcept;

				UTF8() noexcept = default;	//!< @brief See @ref structors
				/*!
					@brief Construct from a preallocated Dynamic array
					@param AData The Dynamic to inherit
					@par Thread Safety
						This function requires no thread safety
					@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::STRING_VALIDATION_FAILURE. Thrown if the string does not validate
				*/
				UTF8(Dynamic&& AData);
				/*!
					@brief Construct a UTF8 string from raw data
					@param AData The data to use for construction, will be copied
					@par Thread Safety
						This function requires no thread safety
					@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE. Thrown if the current heap runs out of memory
					@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::STRING_VALIDATION_FAILURE. Thrown if the string does not validate
				*/
				UTF8(const CStyle& AData);
				UTF8(const UTF8& ACopy) = default;	//!< @brief See @ref structors
				UTF8(UTF8&& AMove) noexcept = default;	//!< @brief See @ref structors
				UTF8& operator=(UTF8&& AMove) noexcept = default;	//!< @brief See @ref structors
				
				/*!
					@brief Appends to a Dynamic string and returns a new one
					@param ARHS The string to append
					@return The new string
					@par Thread Safety
						This function requires synchronization at the object level
					@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE. Thrown if the current heap runs out of memory
				*/
				UTF8 operator+(const UTF8& ARHS) const;
				/*!
					@brief Appends to a Dynamic string in place
					@param ARHS The string to append
					@return A self reference
					@par Thread Safety
						This function requires synchronization at the object level
					@throws CYB::Exception::SystemData Error code: CYB::Exception::SystemData::HEAP_ALLOCATION_FAILURE. Thrown if the current heap runs out of memory
				*/
				UTF8& operator+=(const UTF8& ARHS);
				//! @copydoc CYB::API::String::Dynamic::SubString() This version will not segment inside of multibyte characters
				UTF8 SubString(const int AIndex, const int ALength) const;

				//! @todo Implement
				/*!
					@brief Iterate over the code points
					@param AIterator The function called on each codepoint. Takes an unsigned integer denoting the code point and an integer denoting the character index. Returns a bool indicating whether or not to continue the loop
					@param AReverse Will iterate backwards over the string if true
					@tparam ALambda The type of @p AIterator
					@par Thread Safety
						This function requires synchronization at the object level
				*/
				template <typename ALambda> void IterateCodepoints(const ALambda AIterator, const bool AReverse) const noexcept(noexcept(AIterator(0U,0)));
				
				/*!
					@brief Get the char at index @p APosition. This may be a reference to a multibyte char
					@param APosition The index of character to look up
					@return A const reference to the character at that index. This may be a reference to a multibyte char
					@par Thread Safety
						This function requires synchronization at the object level
				*/
				const char& operator[](const int APosition) const noexcept;

				/*!
					@brief Move the null terminator of the string so that the maxumum number of remaining characters is @p AMaxChars
					@param AMaxChars The maximum character index of the new null terminator
					@par Thread Safety
						This function requires synchronization at the object level
				*/
				void Shrink(const int AMaxChars) noexcept final override;

				//! @copydoc CYB::API::String::CStyle::Length()
				int Length(void) const noexcept final override;
			};
		};
	};
};