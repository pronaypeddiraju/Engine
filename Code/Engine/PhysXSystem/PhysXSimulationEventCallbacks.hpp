#pragma once
#include "ThirdParty/PhysX/include/PxSimulationEventCallback.h"

using namespace physx;

//------------------------------------------------------------------------------------------------------------------------------
// NOTE: We are never going to provide a defenition for any of these virtual functions in this class. The function defenitions
// should be made in game code to handle the callbacks to do some specific gameplay related logic
//------------------------------------------------------------------------------------------------------------------------------
class PhysXSimulationEventCallbacks : public PxSimulationEventCallback
{
public:
	PhysXSimulationEventCallbacks();
	~PhysXSimulationEventCallbacks();

	//////////////////////////////////////////////////////////////////////////
	//Implement PxSimulationEventCallback for the simulation events we care about 

	//NOTE:Uncomment these and add functionality when you need to call these events

//  	virtual	void					onTrigger(PxTriggerPair* pairs, PxU32 count) {}
// 	virtual void					onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {}
// 	virtual void					onWake(PxActor** actors, PxU32 count) {}
// 	virtual void					onSleep(PxActor** actors, PxU32 count) {}
// 	virtual void					onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {}
// 	virtual void					onAdvance(const PxRigidBody*const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {}
};
