//------------------------------------------------------------------------------------------------------------------------------
// Physics System to use when using NVidia PhysX
//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "ThirdParty/PhysX/include/PxPhysicsAPI.h"
#include "Engine/PhysXSystem/PhysXTypes.hpp"
#include <vector>

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

	//System
	void				StartUp();
	void				BeginFrame();
	void				Update(float deltaTime);
	void				EndFrame();
	void				ShutDown();

	PxScene*			GetPhysXScene() const;
	PxPhysics*			GetPhysXSDK() const;
	PxCooking*			GetPhysXCookingModule();
	PxFoundation*		GetPhysXFoundationModule();

	//Rigid body Functions
	PxRigidDynamic*		CreateDynamicObject(const PxGeometry& pxGeometry, const Vec3& velocity, const Matrix44& matrix);

	//Convex Hull
	void				CreateRandomConvexHull(std::vector<Vec3>& vertexArray, int gaussMapLimit, bool directInsertion);
	PxConvexMeshCookingType::Enum	GetPxConvexMeshCookingType(PhysXConvexMeshCookingTypesT meshType);

	//PhysX Materials
	PxMaterial*			GetDefaultPxMaterial() const;

	//Math Functions
	Vec3				PxVectorToVec(const PxVec3& pxVector) const;
	Vec4				PxVectorToVec(const PxVec4& pxVector) const;
	PxVec3				VecToPxVector(const Vec3& vector) const;
	PxVec4				VecToPxVector(const Vec4& vector) const;
	Vec3				QuaternionToEulerAngles(const PxQuat& quat);

private:


	//Data
private:
	PxDefaultAllocator					m_PxAllocator;
	PxDefaultErrorCallback				m_PXErrorCallback;

	PxFoundation*						m_PxFoundation = nullptr;
	PxCooking*							m_PxCooking = NULL;
	PxPhysics*							m_PhysX = NULL;

	PxDefaultCpuDispatcher*				m_PxDispatcher = NULL;
	PxScene*							m_PxScene = NULL;

	PxMaterial*							m_PxMaterial = NULL;

	PxPvd*								m_Pvd = NULL;

	PxReal								m_tempHackStackZ = 10.0f;
};
