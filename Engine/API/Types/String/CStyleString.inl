//! @file CStyleString.inl Implements functions from CYB::API::String::CStyle
#pragma once

inline CYB::API::String::CStyle::CStyle(char* const AData, const int ALength) noexcept :
	FData(AData),
	FLength(ALength == -1 ? CalculateByteLength() : ALength)
{}

inline int CYB::API::String::CStyle::CalculateByteLength(void) const noexcept {
	unsigned long long Length(0);
	if (FData != nullptr) {
		for (; FData[Length] != 0; ++Length);
		Assert::LessThan(Length, static_cast<unsigned long long>(std::numeric_limits<int>().max()));
	}
	return static_cast<int>(Length);
}

inline const char* CYB::API::String::CStyle::CString(void) const noexcept {
	return FData;
}

inline int CYB::API::String::CStyle::RawLength(void) const noexcept {
	return FLength;
}

inline int CYB::API::String::CStyle::IndexOfByte(const byte AByte, int ASkip) const noexcept {
	Assert::LessThanOrEqual(0, ASkip);
	for (auto I(0); I < RawLength(); ++I) 
		if (FData[I] == AByte) {
			if (ASkip == 0)
				return I;
			--ASkip;
		}
	return -1;
}

inline bool CYB::API::String::CStyle::operator==(const CStyle& ARHS) const noexcept {
	if (FLength == ARHS.FLength) {
		for (int I(0); I < FLength; ++I)
			if (FData[I] != ARHS.FData[I])
				return false;
		return true;
	}
	return false;
}

inline bool CYB::API::String::CStyle::operator!=(const CStyle& ARHS) const noexcept {
	return !(*this == ARHS);
}

inline CYB::API::String::CStyle::operator const char*(void) const noexcept {
	auto Result(CString());
	if (Result == nullptr) {
		Assert::Equal(FLength, 0);
		return reinterpret_cast<const char*>(&FLength);
	}
	return Result;
}