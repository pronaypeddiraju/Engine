#pragma once
#include "Engine/PhysXSystem/PhysXSystem.hpp"

//Contact modification values.
#define WHEEL_TANGENT_VELOCITY_MULTIPLIER 0.1f
#define MAX_IMPULSE PX_MAX_F32

//The class WheelCCDContactModifyCallback identifies and modifies ccd contacts
//that involve a wheel.  Contacts that can be identified and managed by the suspension
//system are ignored.  Any contacts that remain are modified to account for the rotation
//speed of the wheel around the rolling axis.
class PhysXWheelCCDContactModifyCallback : public PxCCDContactModifyCallback
{
public:

	PhysXWheelCCDContactModifyCallback()
		: PxCCDContactModifyCallback()
	{
	}

	~PhysXWheelCCDContactModifyCallback() {}

	void onCCDContactModify(PxContactModifyPair* const pairs, PxU32 count)
	{
		for (PxU32 i = 0; i < count; i++)
		{
			const PxRigidActor** actors = pairs[i].actor;
			const PxShape** shapes = pairs[i].shape;

			//Search for actors that represent vehicles and shapes that represent wheels.
			for (PxU32 j = 0; j < 2; j++)
			{
				const PxActor* actor = actors[j];
				if (actor->userData && (static_cast<ActorUserData*>(actor->userData))->vehicle)
				{
					const PxVehicleWheels* vehicle = (static_cast<ActorUserData*>(actor->userData))->vehicle;
					PX_ASSERT(vehicle->getRigidDynamicActor() == actors[j]);

					const PxShape* shape = shapes[j];
					if (shape->userData && (static_cast<ShapeUserData*>(shape->userData))->isWheel)
					{
						const PxU32 wheelId = (static_cast<ShapeUserData*>(shape->userData))->wheelId;
						PX_ASSERT(wheelId < vehicle->mWheelsSimData.getNbWheels());

						//Modify wheel contacts.
						PxVehicleModifyWheelContacts(*vehicle, wheelId, WHEEL_TANGENT_VELOCITY_MULTIPLIER, MAX_IMPULSE, pairs[i]);
					}
				}
			}
		}
	}
};
PhysXWheelCCDContactModifyCallback gWheelCCDContactModifyCallback;