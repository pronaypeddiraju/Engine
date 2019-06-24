//------------------------------------------------------------------------------------------------------------------------------
// Physics System to use when using NVidia PhysX
//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "ThirdParty/PhysX/include/PxPhysicsAPI.h"
#include "Engine/PhysXSystem/PhysXTypes.hpp"

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

using namespace physx;

class RenderContext;
struct Vec3;
struct Vec4;
struct Matrix44;

//------------------------------------------------------------------------------------------------------------------------------
class PhysXSystem
{
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
	PxCooking*			GetPhysXCookingModule();
	PxFoundation*		GetPhysXFoundationModule();

	//Rigibody Functions
	PxRigidDynamic*		CreateDynamicObject(const PxGeometry& pxGeometry, const Vec3& velocity, const Matrix44& matrix);

	//Convex Hull
	PxConvexMeshCookingType::Enum	GetPxConvexMeshCookingType(PhysXConvexMeshCookingTypesT meshType);

	//PhysX Materials
	PxMaterial*			GetDefaultPxMaterial() const;

	//Math Functions
	Vec3				PxVectorToVec(const PxVec3& pxVector) const;
	Vec4				PxVectorToVec(const PxVec4& pxVector) const;
	PxVec3				VecToPxVector(const Vec3& vector) const;
	PxVec4				VecToPxVector(const Vec4& vector) const;

	Vec3				QuaternionToEulerAngles(const PxQuat& quat);

	void				ShutDown();

private:


	//Data
private:
	PxDefaultAllocator					m_PxAllocator;
	PxDefaultErrorCallback				m_PXErrorCallback;

	PxFoundation*						m_PxFoundation = NULL;
	PxCooking*							m_PxCooking = NULL;
	PxPhysics*							m_PhysX = NULL;

	PxDefaultCpuDispatcher*				m_PxDispatcher = NULL;
	PxScene*							m_PxScene = NULL;

	PxMaterial*							m_PxMaterial = NULL;

	PxPvd*								m_Pvd = NULL;

	PxReal								stackZ = 10.0f;
};
