//! @file CYBEngineParameters.cpp Engine compilation configuration string variables
#include "CYB.hpp"

using Params = CYB::Engine::Parameters;

const char* const Params::FTempPathName(u8"CyberEngineMkIII");

CYB::API::EngineInformation CYB::Engine::Parameters::CreateEngineInformation(void) noexcept {
	return{
		u8"CyberEngine Mark III",
		u8"Copyright Dextraspace Entertainment",
		COPYRIGHT_YEAR,
		VERSION_MAJOR,
		VERSION_MINOR,
		VERSION_REVISION,
		__DATE__,
		__TIME__ " EST",
	};
}