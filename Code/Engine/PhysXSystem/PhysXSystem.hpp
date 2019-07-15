//------------------------------------------------------------------------------------------------------------------------------
// Physics System to use when using NVidia PhysX
//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "ThirdParty/PhysX/include/PxPhysicsAPI.h"
#include "Engine/PhysXSystem/PhysXTypes.hpp"
#include "Engine/PhysXSystem/PhysXVehicleSceneQuery.hpp"
#include "Engine/PhysXSystem/PhysXVehicleCreate.hpp"
#include <vector>
#include <functional>

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

using namespace physx;
using namespace vehicle;

class RenderContext;
struct Vec3;
struct Vec4;
struct Matrix44;

typedef unsigned int uint;
typedef PxJoint* (*JointCreateFunction)(PxRigidActor* actorA, const Vec3& positionA, PxRigidActor* actorB, const Vec3& positionB);

//------------------------------------------------------------------------------------------------------------------------------
class PhysXSystem
{
public:
	PhysXSystem();
	~PhysXSystem();

	//System
	void				StartUp();
	PxVehicleDrive4W*	StartUpVehicleSDK();
	VehicleDesc			InitializeVehicleDescription(const PxFilterData& chassisSimFilterData, const PxFilterData& wheelSimFilterData);
	void				BeginFrame();
	void				Update(float deltaTime);
	void				EndFrame();
	void				ShutDown();

	PxScene*			GetPhysXScene() const;
	PxPhysics*			GetPhysXSDK() const;
	PxCooking*			GetPhysXCookingModule() const;
	PxFoundation*		GetPhysXFoundationModule() const;
	PxBatchQuery*		GetPhysXBatchQuery() const;
	
	//Getters for Vehicle SDK
	VehicleSceneQueryData*	GetVehicleSceneQueryData() const;
	PxVehicleDrivableSurfaceToTireFrictionPairs* GetVehicleTireFrictionPairs() const;

	//Rigid body Functions
	PxRigidDynamic*		CreateDynamicObject(const PxGeometry& pxGeometry, const Vec3& velocity, const Matrix44& matrix, float materialDensity = -1.f);

	//Convex Hull
	void				CreateRandomConvexHull(std::vector<Vec3>& vertexArray, int gaussMapLimit, bool directInsertion);
	PxConvexMeshCookingType::Enum	GetPxConvexMeshCookingType(PhysXConvexMeshCookingTypes_T meshType);

	//PhysX Joints
	PxJoint*			CreateJointSimpleSpherical(PxRigidActor* actorA, const Vec3& positionA, PxRigidActor* actorB, const Vec3& positionB);
	PxJoint*			CreateJointLimitedSpherical(PxRigidActor* actorA, const Vec3& positionA, PxRigidActor* actorB, const Vec3& positionB
												, float yAngleLimit, float zAngleLimit, float contactDistance = -1.f);

	PxJoint*			CreateJointSimpleFixed(PxRigidActor* actorA, const Vec3& positionA, PxRigidActor* actorB, const Vec3& positionB);
	PxJoint*			CreateJointBreakableFixed(PxRigidActor* actorA, const Vec3& positionA, PxRigidActor* actorB, const Vec3& positionB
												, float breakForce = FLT_MAX, float breakTorque = FLT_MAX);

	PxJoint*			CreateJointDampedD6(PxRigidActor* actorA, const Vec3& positionA, PxRigidActor* actorB, const Vec3& positionB
												, float driveStiffness, float driveDamping, float driveForceLimit = FLT_MAX, bool isDriveAcceleration = false);

	//PhysX Chains
	void				CreateSimpleSphericalChain(const Vec3& position, int length, const PxGeometry& geometry, float separation);
	void				CreateLimitedSphericalChain(const Vec3& position, int length, const PxGeometry& geometry, float separation
												, float yConeAngleLimit, float zConeAngleLimit, float coneContactDistance = -1.f);

	void				CreateSimpleFixedChain(const Vec3& position, int length, const PxGeometry& geometry, float separation);
	void				CreateBreakableFixedChain(const Vec3& position, int length, const PxGeometry& geometry, float separation
												, float breakForce = FLT_MAX, float breakTorque = FLT_MAX);

	void				CreateDampedD6Chain(const Vec3& position, int length, const PxGeometry& geometry, float separation
												, float driveStiffness, float driveDamping, float driveForceLimit = FLT_MAX, bool isDriveAcceleration = false);

	//PhysX Materials
	PxMaterial*			GetDefaultPxMaterial() const;

	//Math Functions
	static Vec3			PxVectorToVec(const PxVec3& pxVector);
	static Vec4			PxVectorToVec(const PxVec4& pxVector);
	static PxVec3		VecToPxVector(const Vec3& vector);
	static PxVec4		VecToPxVector(const Vec4& vector);
	static Vec3			QuaternionToEulerAngles(const PxQuat& quat);
	static PxQuat		MakeQuaternionFromMatrix(const Matrix44& matrix);

private:

private:
	PxDefaultAllocator					m_PxAllocator;
	PxDefaultErrorCallback				m_PXErrorCallback;

	PxFoundation*						m_PxFoundation = nullptr;
	PxCooking*							m_PxCooking = nullptr;
	PxPhysics*							m_PhysX = nullptr;

	PxDefaultCpuDispatcher*				m_PxDispatcher = nullptr;
	PxScene*							m_PxScene = nullptr;

	PxMaterial*							m_PxDefaultMaterial = nullptr;

	//PhysX Visual Debugger
	PxPvd*								m_Pvd = nullptr;
	std::string							m_pvdIPAddress = "127.0.0.1";
	int									m_pvdPortNumber = 5425;
	uint								m_pvdTimeOutSeconds = 10;

	//Default values for PhysX properties to use
	float								m_defaultStaticFriction = 0.5f;
	float								m_defaultDynamicFriction = 0.5f;
	float								m_defaultRestitution = 0.6f;
	float								m_defaultDensity = 10.f;
	float								m_defaultAngularDamping = 0.5f;

	//Vehicle SDK
	bool								m_vehicleKitEnabled = false;
	VehicleSceneQueryData*				m_vehicleSceneQueryData = nullptr;
	PxBatchQuery*						m_batchQuery = nullptr;
	PxVehicleDrivableSurfaceToTireFrictionPairs* m_frictionPairs = nullptr;
	PxRigidStatic*						m_drivableGroundPlane = nullptr;

	int									m_numberOfVehicles = 1;
	ActorUserData						m_actorUserData;
	ShapeUserData						m_shapeUserData[PX_MAX_NB_WHEELS];
};
