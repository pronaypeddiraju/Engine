//------------------------------------------------------------------------------------------------------------------------------
// Time.hpp
//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include <stdint.h>

//------------------------------------------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds();

uint64_t GetCurrentTimeHPC();
double GetHPCToSeconds(uint64_t hpc);