//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Trigger2D.hpp"
#include <vector>

class TriggerBucket
{
public:
	TriggerBucket();
	~TriggerBucket();

public:
	std::vector<Trigger2D*>	m_triggerBucket[NUM_SIMULATION_TYPES];
};