//------------------------------------------------------------------------------------------------------------------------------
// Physics System to use when using NVidia PhysX
//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "ThirdParty/PhysX/include/PxPhysicsAPI.h"

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

using namespace physx;

class RenderContext;

//------------------------------------------------------------------------------------------------------------------------------
class PhysXSystem
{
	//Functions
public:
	PhysXSystem();
	~PhysXSystem();

	void				StartUp();
	void				BeginFrame();
	void				Update(float deltaTime);
	void				EndFrame();

	void				DebugRender(RenderContext* renderContext) const;

	PxScene*			GetPhysXScene() const;
	PxPhysics*			GetPhysXSDK() const;

	void				ShutDown();

private:


	//Data
private:
	PxDefaultAllocator					m_PxAllocator;
	PxDefaultErrorCallback				m_PXErrorCallback;

	PxFoundation*						m_PxFoundation = NULL;
	PxPhysics*							m_PhysX = NULL;

	PxDefaultCpuDispatcher*				m_PxDispatcher = NULL;
	PxScene*							m_PxScene = NULL;

	PxMaterial*							m_PxMaterial = NULL;

	PxPvd*								m_Pvd = NULL;

	PxReal								stackZ = 10.0f;
};
