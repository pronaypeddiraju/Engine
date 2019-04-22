//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
unsigned long				CreateFileBuffer(const std::string& fileName, char** outData);
unsigned long				CreateFileTextBuffer(const std::string& fileName, char** outData);