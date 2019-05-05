//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/TriggerBucket.hpp"

//------------------------------------------------------------------------------------------------------------------------------
TriggerBucket::TriggerBucket()
{

}

//------------------------------------------------------------------------------------------------------------------------------
TriggerBucket::~TriggerBucket()
{
	for (int numTypes = 0; numTypes < NUM_SIMULATION_TYPES; numTypes++)
	{
		int numTriggers = static_cast<int>(m_triggerBucket[numTypes].size());
		for (int triggerIndex = 0; triggerIndex < numTriggers; triggerIndex++)
		{
			delete m_triggerBucket[numTypes][triggerIndex];
			m_triggerBucket[numTypes][triggerIndex] = nullptr;
		}
	}
}
