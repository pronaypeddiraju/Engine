//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/PhysXSystem/PhysXSystem.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "ThirdParty/PhysX/include/PxPhysicsAPI.h"

//PhysX Pragma Comments
#if ( defined( _WIN64 ) & defined( _DEBUG ) )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x64/PhysX_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x64/PhysXCommon_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x64/PhysXCooking_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x64/PhysXExtensions_static_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x64/PhysXFoundation_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x64/PhysXPvdSDK_static_64.lib" )
#elif ( defined ( _WIN64 ) & defined( NDEBUG ) )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x64/PhysX_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x64/PhysXCommon_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x64/PhysXCooking_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x64/PhysXExtensions_static_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x64/PhysXFoundation_64.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x64/PhysXPvdSDK_static_64.lib" )
#elif ( defined( _WIN32 ) & defined( _DEBUG ) )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x86/PhysX_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x86/PhysXCommon_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x86/PhysXCooking_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x86/PhysXExtensions_static_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x86/PhysXFoundation_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/debug_x86/PhysXPvdSDK_static_32.lib" )
#elif ( defined( _WIN32 ) & defined( NDEBUG ) )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x86/PhysX_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x86/PhysXCommon_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x86/PhysXCooking_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x86/PhysXExtensions_static_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x86/PhysXFoundation_32.lib" )
#pragma comment( lib, "ThirdParty/PhysX/lib/release_x86/PhysXPvdSDK_static_32.lib" )
#endif

PhysXSystem* g_PxPhysXSystem = nullptr;

//------------------------------------------------------------------------------------------------------------------------------
PhysXSystem::PhysXSystem()
{
	StartUp();
}

//------------------------------------------------------------------------------------------------------------------------------
PhysXSystem::~PhysXSystem()
{
	ShutDown();
}

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::StartUp()
{
	//PhysX starts off by setting up a Physics Foundation
	m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PxAllocator, m_PXErrorCallback);

	//Setup PhysX cooking if you need convex hull cooking support
	m_PxCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_PxFoundation, PxCookingParams(PxTolerancesScale()));

	//Create the PhysX Visual Debugger by giving it the current foundation
	m_Pvd = PxCreatePvd(*m_PxFoundation);
	//The PVD needs connection via a socket. It will run on the Address defined, in our case it's our machine
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(m_pvdIPAddress.c_str(), m_pvdPortNumber, m_pvdTimeOutSeconds);
	m_Pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	//Create Physics! This creates an instance of the PhysX SDK
	m_PhysX = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), true, m_Pvd);

	bool result = PxInitExtensions(*m_PhysX, m_Pvd);

	//What is the description of this PhysX scene?
	PxSceneDesc sceneDesc(m_PhysX->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	//This creates CPU dispatcher threads or worker threads. We will make 2
	m_PxDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_PxDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	//Create the scene now by passing the scene's description
	m_PxScene = m_PhysX->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = m_PxScene->getScenePvdClient();
	if (pvdClient)
	{
		//I have a PVD client, so set some flags that it needs
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	m_PxDefaultMaterial = m_PhysX->createMaterial(m_defaultStaticFriction, m_defaultDynamicFriction, m_defaultRestitution);
}

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::BeginFrame()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::Update(float deltaTime)
{
	m_PxScene->simulate(deltaTime);
	m_PxScene->fetchResults(true);
}

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::EndFrame()
{

}

//------------------------------------------------------------------------------------------------------------------------------
physx::PxScene* PhysXSystem::GetPhysXScene() const
{
	return m_PxScene;
}

//------------------------------------------------------------------------------------------------------------------------------
physx::PxPhysics* PhysXSystem::GetPhysXSDK() const
{
	return m_PhysX;
}

//------------------------------------------------------------------------------------------------------------------------------
physx::PxCooking* PhysXSystem::GetPhysXCookingModule() const
{
	return m_PxCooking;
}

//------------------------------------------------------------------------------------------------------------------------------
physx::PxFoundation* PhysXSystem::GetPhysXFoundationModule() const
{
	return m_PxFoundation; 
}

//------------------------------------------------------------------------------------------------------------------------------
physx::PxRigidDynamic* PhysXSystem::CreateDynamicObject(const PxGeometry& pxGeometry, const Vec3& velocity, const Matrix44& matrix, float materialDensity)
{
	if (materialDensity < 0.f)
	{
		materialDensity = m_defaultDensity;
	}

	PxPhysics* physX = g_PxPhysXSystem->GetPhysXSDK();
	PxScene* pxScene = g_PxPhysXSystem->GetPhysXScene();

	PxVec3 pxVelocity = PxVec3(velocity.x, velocity.y, velocity.z);
	Vec3 position = matrix.GetTBasis();
	PxVec3 pxPosition = VecToPxVector(position);

	PxTransform pxTransform(pxPosition);
	pxTransform.q = MakeQuaternionFromMatrix(matrix);

	PxRigidDynamic* dynamic = PxCreateDynamic(*physX, pxTransform, pxGeometry, *m_PxDefaultMaterial, materialDensity);
    dynamic->setAngularDamping(m_defaultAngularDamping);
	dynamic->setLinearVelocity(pxVelocity);
	pxScene->addActor(*dynamic);

	return dynamic;
}

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::CreateRandomConvexHull(std::vector<Vec3>& vertexArray, int gaussMapLimit, bool directInsertion)
{
	PxCooking* pxCookingModule = g_PxPhysXSystem->GetPhysXCookingModule();
	PxPhysics* pxPhysics = g_PxPhysXSystem->GetPhysXSDK();
	PxScene* pxScene = g_PxPhysXSystem->GetPhysXScene();
	PxCookingParams params = pxCookingModule->getParams();

	// Use the new (default) PxConvexMeshCookingType::eQUICKHULL
	params.convexMeshCookingType = g_PxPhysXSystem->GetPxConvexMeshCookingType(QUICKHULL);

	// If the gaussMapLimit is chosen higher than the number of output vertices, no gauss map is added to the convex mesh data (here 256).
	// If the gaussMapLimit is chosen lower than the number of output vertices, a gauss map is added to the convex mesh data (here 16).
	params.gaussMapLimit = gaussMapLimit;
	pxCookingModule->setParams(params);

	// Setup the convex mesh descriptor
	PxConvexMeshDesc desc;
	PxConvexMesh* pxConvexMesh;

	std::vector<PxVec3> pxVecArray;
	pxVecArray.reserve(vertexArray.size());

	int numVerts = (int)vertexArray.size();
	for (int vecIndex = 0; vecIndex < numVerts; vecIndex++)
	{
		pxVecArray.push_back(g_PxPhysXSystem->VecToPxVector(vertexArray[vecIndex]));
	}

	// We provide points only, therefore the PxConvexFlag::eCOMPUTE_CONVEX flag must be specified
	desc.points.data = &pxVecArray[0];
	desc.points.count = numVerts;
	desc.points.stride = sizeof(PxVec3);
	desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxU32 meshSize = 0;

	if (directInsertion)
	{
		// Directly insert mesh into PhysX
		pxConvexMesh = pxCookingModule->createConvexMesh(desc, pxPhysics->getPhysicsInsertionCallback());
		PX_ASSERT(convex);
	}
	else
	{
		// Serialize the cooked mesh into a stream.
		PxFoundation* pxFoundation = g_PxPhysXSystem->GetPhysXFoundationModule();
		PxDefaultMemoryOutputStream outStream(pxFoundation->getAllocatorCallback());
		bool res = pxCookingModule->cookConvexMesh(desc, outStream);
		PX_UNUSED(res);
		PX_ASSERT(res);
		meshSize = outStream.getSize();

		// Create the mesh from a stream.
		PxDefaultMemoryInputData inStream(outStream.getData(), outStream.getSize());
		pxConvexMesh = pxPhysics->createConvexMesh(inStream);

		//Make the geometrue
		PxConvexMeshGeometry geometry = PxConvexMeshGeometry(pxConvexMesh);
		const PxMaterial* material = g_PxPhysXSystem->GetDefaultPxMaterial();

		PxShape* shape = pxPhysics->createShape(geometry, *material);

		PxTransform localTm(PxVec3(0, 50.f, 0));
		PxRigidDynamic* body = pxPhysics->createRigidDynamic(localTm);
		body->attachShape(*shape);
		PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
		pxScene->addActor(*body);

		PX_ASSERT(convex);
	}

	pxConvexMesh->release();
}

//------------------------------------------------------------------------------------------------------------------------------
physx::PxConvexMeshCookingType::Enum PhysXSystem::GetPxConvexMeshCookingType(PhysXConvexMeshCookingTypes_T meshType)
{
	switch (meshType)
	{
	case QUICKHULL:
	{
		return PxConvexMeshCookingType::eQUICKHULL;
	}
		break;
	default:
	{
		ERROR_AND_DIE("Mesh Type is not supported by PhysX");
	}
		break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
// NOTE: Interface asks for angles in degrees but PhysX needs angles in Radians
//------------------------------------------------------------------------------------------------------------------------------
physx::PxJoint* PhysXSystem::CreateJointLimitedSpherical(PxRigidActor* actorA, const Vec3& positionA, PxRigidActor* actorB, const Vec3& positionB, float yAngleLimit, float zAngleLimit, float contactDistance)
{
	PxTransform transformA(VecToPxVector(positionA));
	PxTransform transformB(VecToPxVector(positionB));

	PxSphericalJoint* joint = PxSphericalJointCreate(*m_PhysX, actorA, transformA, actorB, transformB);
	joint->setLimitCone(PxJointLimitCone(DegreesToRadians(yAngleLimit), DegreesToRadians(zAngleLimit), contactDistance));
	joint->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
	return joint;
}

//------------------------------------------------------------------------------------------------------------------------------
// Unbreakable Fixed Joint
//------------------------------------------------------------------------------------------------------------------------------
physx::PxJoint* PhysXSystem::CreateJointSimpleFixed(PxRigidActor* actorA, const Vec3& positionA, PxRigidActor* actorB, const Vec3& positionB)
{
	PxTransform transformA(VecToPxVector(positionA));
	PxTransform transformB(VecToPxVector(positionB));

	PxFixedJoint* joint = PxFixedJointCreate(*m_PhysX, actorA, transformA, actorB, transformB);
	joint->setBreakForce(FLT_MAX, FLT_MAX);
	joint->setConstraintFlag(PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES, true);
	joint->setConstraintFlag(PxConstraintFlag::eDISABLE_PREPROCESSING, true);
	return joint;
}

//------------------------------------------------------------------------------------------------------------------------------
// Unrestricted Spherical Joint (Rotation permitted to 180 degrees in y and z)
// NOTE: PhysX needs the angles in Radians, in the simple case we pass it PxHalfPi for y and z cone limits
//------------------------------------------------------------------------------------------------------------------------------
physx::PxJoint* PhysXSystem::CreateJointSimpleSpherical(PxRigidActor* actorA, const Vec3& positionA, PxRigidActor* actorB, const Vec3& positionB)
{
	PxTransform transformA(VecToPxVector(positionA));
	PxTransform transformB(VecToPxVector(positionB));

	PxSphericalJoint* joint = PxSphericalJointCreate(*m_PhysX, actorA, transformA, actorB, transformB);
	joint->setLimitCone(PxJointLimitCone(PxHalfPi, PxHalfPi));
	joint->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
	return joint;
}

//------------------------------------------------------------------------------------------------------------------------------
// NOTE: This creates a fixed joint which we set a break force to.
// By default the breakable joint is unbreakable(max float for break force)
//------------------------------------------------------------------------------------------------------------------------------
physx::PxJoint* PhysXSystem::CreateJointBreakableFixed(PxRigidActor* actorA, const Vec3& positionA, PxRigidActor* actorB, const Vec3& positionB, float breakForce, float breakTorque)
{
	PxTransform transformA(VecToPxVector(positionA));
	PxTransform transformB(VecToPxVector(positionB));

	PxFixedJoint* joint = PxFixedJointCreate(*m_PhysX, actorA, transformA, actorB, transformB);
	joint->setBreakForce(breakForce, breakTorque);
	joint->setConstraintFlag(PxConstraintFlag::eDRIVE_LIMITS_ARE_FORCES, true);
	joint->setConstraintFlag(PxConstraintFlag::eDISABLE_PREPROCESSING, true);
	return joint;
}

//------------------------------------------------------------------------------------------------------------------------------
// NOTE: By default the max drive force limit is set to FLT_MAX and the drive is force dependent (Not acceleration dependent)
//------------------------------------------------------------------------------------------------------------------------------
physx::PxJoint* PhysXSystem::CreateJointDampedD6(PxRigidActor* actorA, const Vec3& positionA, PxRigidActor* actorB, const Vec3& positionB, float driveStiffness, float driveDamping, float driveForceLimit, bool isDriveAcceleration)
{
	PxTransform transformA(VecToPxVector(positionA));
	PxTransform transformB(VecToPxVector(positionB));

	PxD6Joint* joint = PxD6JointCreate(*m_PhysX, actorA, transformA, actorB, transformB);
	joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
	joint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
	joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
	joint->setDrive(PxD6Drive::eSLERP, PxD6JointDrive(driveStiffness, driveDamping, driveForceLimit, isDriveAcceleration));
	return joint;
}

void PhysXSystem::CreateSimpleSphericalChain(const Vec3& position, int length, const PxGeometry& geometry, float separation)
{
	PxTransform transform(PhysXSystem::VecToPxVector(position));
	PxPhysics* physX = g_PxPhysXSystem->GetPhysXSDK();
	PxScene* pxScene = g_PxPhysXSystem->GetPhysXScene();
	PxMaterial* pxMat = g_PxPhysXSystem->GetDefaultPxMaterial();

	PxVec3 offsetPx(separation / 2.f, 0, 0);
	Vec3 offset(separation / 2.f, 0.f, 0.f);
	PxTransform localTm(offsetPx);
	PxRigidDynamic* prev = nullptr;

	for (int i = 0; i < length; i++)
	{
		PxRigidDynamic* current = PxCreateDynamic(*physX, transform * localTm, geometry, *pxMat, 1.0f);

		if (prev == nullptr)
		{
			CreateJointSimpleSpherical(prev, position, current, offset * -1.f);
		}
		else
		{
			CreateJointSimpleSpherical(prev, offset, current, offset * -1.f);
		}

		pxScene->addActor(*current);
		prev = current;
		localTm.p.x += separation;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::CreateSimpleFixedChain(const Vec3& position, int length, const PxGeometry& geometry, float separation)
{
	PxTransform transform(PhysXSystem::VecToPxVector(position));
	PxPhysics* physX = g_PxPhysXSystem->GetPhysXSDK();
	PxScene* pxScene = g_PxPhysXSystem->GetPhysXScene();
	PxMaterial* pxMat = g_PxPhysXSystem->GetDefaultPxMaterial();

	PxVec3 offsetPx(separation / 2.f, 0, 0);
	Vec3 offset(separation / 2.f, 0.f, 0.f);
	PxTransform localTm(offsetPx);
	PxRigidDynamic* prev = nullptr;

	for (int i = 0; i < length; i++)
	{
		PxRigidDynamic* current = PxCreateDynamic(*physX, transform * localTm, geometry, *pxMat, 1.0f);

		if (prev == nullptr)
		{
			CreateJointSimpleFixed(prev, position, current, offset * -1.f);
		}
		else
		{
			CreateJointSimpleFixed(prev, offset, current, offset * -1.f);
		}

		pxScene->addActor(*current);
		prev = current;
		localTm.p.x += separation;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::CreateLimitedSphericalChain(const Vec3& position, int length, const PxGeometry& geometry, float separation, float yConeAngleLimit, float zConeAngleLimit, float coneContactDistance /*= -1.f*/)
{
	PxTransform transform(PhysXSystem::VecToPxVector(position));
	PxPhysics* physX = g_PxPhysXSystem->GetPhysXSDK();
	PxScene* pxScene = g_PxPhysXSystem->GetPhysXScene();
	PxMaterial* pxMat = g_PxPhysXSystem->GetDefaultPxMaterial();

	PxVec3 offsetPx(separation / 2.f, 0, 0);
	Vec3 offset(separation / 2.f, 0.f, 0.f);
	PxTransform localTm(offsetPx);
	PxRigidDynamic* prev = nullptr;

	for (int i = 0; i < length; i++)
	{
		PxRigidDynamic* current = PxCreateDynamic(*physX, transform * localTm, geometry, *pxMat, 1.0f);

		if (prev == nullptr)
		{
			CreateJointLimitedSpherical(prev, position, current, offset * -1.f, yConeAngleLimit, zConeAngleLimit, coneContactDistance);
		}
		else
		{
			CreateJointLimitedSpherical(prev, offset, current, offset * -1.f, yConeAngleLimit, zConeAngleLimit, coneContactDistance);
		}

		pxScene->addActor(*current);
		prev = current;
		localTm.p.x += separation;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::CreateBreakableFixedChain(const Vec3& position, int length, const PxGeometry& geometry, float separation, float breakForce /*= FLT_MAX*/, float breakTorque /*= FLT_MAX*/)
{
	PxTransform transform(PhysXSystem::VecToPxVector(position));
	PxPhysics* physX = g_PxPhysXSystem->GetPhysXSDK();
	PxScene* pxScene = g_PxPhysXSystem->GetPhysXScene();
	PxMaterial* pxMat = g_PxPhysXSystem->GetDefaultPxMaterial();

	PxVec3 offsetPx(separation / 2.f, 0, 0);
	Vec3 offset(separation / 2.f, 0.f, 0.f);
	PxTransform localTm(offsetPx);
	PxRigidDynamic* prev = nullptr;

	for (int i = 0; i < length; i++)
	{
		PxRigidDynamic* current = PxCreateDynamic(*physX, transform * localTm, geometry, *pxMat, 1.0f);

		if (prev == nullptr)
		{
			CreateJointBreakableFixed(prev, position, current, offset * -1.f, breakForce, breakTorque);
		}
		else
		{
			CreateJointBreakableFixed(prev, offset, current, offset * -1.f, breakForce, breakTorque);
		}

		pxScene->addActor(*current);
		prev = current;
		localTm.p.x += separation;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::CreateDampedD6Chain(const Vec3& position, int length, const PxGeometry& geometry, float separation, float driveStiffness, float driveDamping, float driveForceLimit /*= FLT_MAX*/, bool isDriveAcceleration /*= false*/)
{
	PxTransform transform(PhysXSystem::VecToPxVector(position));
	PxPhysics* physX = g_PxPhysXSystem->GetPhysXSDK();
	PxScene* pxScene = g_PxPhysXSystem->GetPhysXScene();
	PxMaterial* pxMat = g_PxPhysXSystem->GetDefaultPxMaterial();

	PxVec3 offsetPx(separation / 2.f, 0, 0);
	Vec3 offset(separation / 2.f, 0.f, 0.f);
	PxTransform localTm(offsetPx);
	PxRigidDynamic* prev = nullptr;

	for (int i = 0; i < length; i++)
	{
		PxRigidDynamic* current = PxCreateDynamic(*physX, transform * localTm, geometry, *pxMat, 1.0f);

		if (prev == nullptr)
		{
			CreateJointDampedD6(prev, position, current, offset * -1.f, driveStiffness, driveDamping, driveForceLimit, isDriveAcceleration);
		}
		else
		{
			CreateJointDampedD6(prev, offset, current, offset * -1.f, driveStiffness, driveDamping, driveForceLimit, isDriveAcceleration);
		}

		pxScene->addActor(*current);
		prev = current;
		localTm.p.x += separation;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
physx::PxMaterial* PhysXSystem::GetDefaultPxMaterial() const
{
	return m_PxDefaultMaterial;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC Vec3 PhysXSystem::PxVectorToVec(const PxVec3& pxVector)
{
	Vec3 vector;
	vector.x = pxVector.x;
	vector.y = pxVector.y;
	vector.z = pxVector.z;
	return vector;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC Vec4 PhysXSystem::PxVectorToVec(const PxVec4& pxVector) 
{
	Vec4 vector;
	vector.x = pxVector.x;
	vector.y = pxVector.y;
	vector.z = pxVector.z;
	vector.w = pxVector.w;
	return vector;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC physx::PxVec3 PhysXSystem::VecToPxVector(const Vec3& vector)
{
	PxVec3 pxVector(vector.x, vector.y, vector.z);
	return pxVector;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC physx::PxVec4 PhysXSystem::VecToPxVector(const Vec4& vector)
{
	PxVec4 pxVector(vector.x, vector.y, vector.z, vector.w);
	return pxVector;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC Vec3 PhysXSystem::QuaternionToEulerAngles(const PxQuat& quat) 
{
	Vec3 eulerAngles;

	// roll (x-axis rotation)
	float sinr_cosp = 2.0f * (quat.w * quat.x + quat.y * quat.z);
	float cosr_cosp = 1.0f - 2.0f * (quat.x * quat.x + quat.y * quat.y);
	eulerAngles.x = atan2f(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	float sinp = +2.0f * (quat.w * quat.y - quat.z * quat.x);
	if (fabs(sinp) >= 1)
		eulerAngles.y = copysign(PI / 2.f, sinp); // use 90 degrees if out of range
	else
		eulerAngles.y = asinf(sinp);

	// yaw (z-axis rotation)
	float siny_cosp = 2.0f * (quat.w * quat.z + quat.x * quat.y);
	float cosy_cosp = 1.0f - 2.0f * (quat.y * quat.y + quat.z * quat.z);
	eulerAngles.z = atan2f(siny_cosp, cosy_cosp);

	return eulerAngles;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC PxQuat PhysXSystem::MakeQuaternionFromMatrix(const Matrix44& matrix)
{
	//Quaternion
	float w = sqrt(1.0f + matrix.m_values[Matrix44::Ix] + matrix.m_values[Matrix44::Jy] + matrix.m_values[Matrix44::Kz]) / 2.0f;
	float one_over_w4 = 1.f / (4.0f * w);
	float x = (matrix.m_values[Matrix44::Ky] - matrix.m_values[Matrix44::Jz]) * one_over_w4;
	float y = (matrix.m_values[Matrix44::Iz] - matrix.m_values[Matrix44::Kx]) * one_over_w4;
	float z = (matrix.m_values[Matrix44::Jx] - matrix.m_values[Matrix44::Iy]) * one_over_w4;

	PxQuat quaternion = PxQuat(x, y, z, w);
	return quaternion;
}

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::ShutDown()
{
	//Handle all shutdown code here for Nvidia PhysX
	PX_RELEASE(m_PxScene);
	PX_RELEASE(m_PxDispatcher);
	PX_RELEASE(m_PhysX);
	PX_RELEASE(m_PxCooking);
	if (m_Pvd)
	{
		PxPvdTransport* transport = m_Pvd->getTransport();
		m_Pvd->release();	m_Pvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(m_PxFoundation);
}

