//! @file CStyleString.inl Implements functions from CYB::API::String::CStyle
#pragma once

inline CYB::API::String::CStyle::CStyle(char* const AData) noexcept :
	FData(AData),
	FLength(CalculateByteLength())
{}

inline int CYB::API::String::CStyle::CalculateByteLength(void) const noexcept {
	unsigned long long Length(0);
	if (FData != nullptr)
		for (; FData[Length] != 0; ++Length);
	Assert::LessThan(Length, static_cast<unsigned long long>(std::numeric_limits<int>().max()));
	return static_cast<int>(Length);
}

inline const char* CYB::API::String::CStyle::CString(void) const noexcept {
	return FData;
}

inline int CYB::API::String::CStyle::RawLength(void) const noexcept {
	return FLength;
}