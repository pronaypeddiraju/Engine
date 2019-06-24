//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/PhysXSystem/PhysXSystem.hpp"
#include "Engine/Commons/ErrorWarningAssert.hpp"
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
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_Pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	//Create Physics! This creates an instance of the PhysX SDK
	m_PhysX = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), true, m_Pvd);

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

	m_PxMaterial = m_PhysX->createMaterial(0.5f, 0.5f, 0.6f);
}

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::BeginFrame()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::Update(float deltaTime)
{
	m_PxScene->simulate(deltaTime * 2.f);
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
physx::PxCooking* PhysXSystem::GetPhysXCookingModule()
{
	return m_PxCooking;
}

//------------------------------------------------------------------------------------------------------------------------------
physx::PxFoundation* PhysXSystem::GetPhysXFoundationModule()
{
	return m_PxFoundation; 
}

//------------------------------------------------------------------------------------------------------------------------------
physx::PxRigidDynamic* PhysXSystem::CreateDynamicObject(const PxGeometry& pxGeometry, const Vec3& velocity, const Matrix44& matrix)
{
	PxPhysics* physX = g_PxPhysXSystem->GetPhysXSDK();
	PxScene* pxScene = g_PxPhysXSystem->GetPhysXScene();
	PxMaterial* pxMaterial = physX->createMaterial(0.5f, 0.5f, 0.6f);

	PxVec3 pxVelocity = PxVec3(velocity.x, velocity.y, velocity.z);
	Vec3 position = matrix.GetTVector();
	PxVec3 pxPosition = VecToPxVector(position);

	PxTransform pxTransform(pxPosition);

	//Quaternion
	float w = sqrt(1.0f + matrix.m_values[Matrix44::Ix] + matrix.m_values[Matrix44::Jy] + matrix.m_values[Matrix44::Kz]) / 2.0f;
	float w4 = (4.0f * w);
	float x = (matrix.m_values[Matrix44::Ky] - matrix.m_values[Matrix44::Jz]) / w4;
	float y = (matrix.m_values[Matrix44::Iz] - matrix.m_values[Matrix44::Kx]) / w4;
	float z = (matrix.m_values[Matrix44::Jx] - matrix.m_values[Matrix44::Iy]) / w4;

	pxTransform.q = PxQuat(x, y, z, w);

	PxRigidDynamic* dynamic = PxCreateDynamic(*physX, pxTransform, pxGeometry, *pxMaterial, 10.0f);
   	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(pxVelocity);
	pxScene->addActor(*dynamic);

	return dynamic;
}

//------------------------------------------------------------------------------------------------------------------------------
physx::PxConvexMeshCookingType::Enum PhysXSystem::GetPxConvexMeshCookingType(PhysXConvexMeshCookingTypesT meshType)
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
physx::PxMaterial* PhysXSystem::GetDefaultPxMaterial() const
{
	return m_PxMaterial;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec3 PhysXSystem::PxVectorToVec(const PxVec3& pxVector) const
{
	Vec3 vector;
	vector.x = pxVector.x;
	vector.y = pxVector.y;
	vector.z = pxVector.z;
	return vector;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec4 PhysXSystem::PxVectorToVec(const PxVec4& pxVector) const
{
	Vec4 vector;
	vector.x = pxVector.x;
	vector.y = pxVector.y;
	vector.z = pxVector.z;
	vector.w = pxVector.w;
	return vector;
}

//------------------------------------------------------------------------------------------------------------------------------
physx::PxVec3 PhysXSystem::VecToPxVector(const Vec3& vector) const
{
	PxVec3 pxVector(vector.x, vector.y, vector.z);
	return pxVector;
}

//------------------------------------------------------------------------------------------------------------------------------
physx::PxVec4 PhysXSystem::VecToPxVector(const Vec4& vector) const
{
	PxVec4 pxVector(vector.x, vector.y, vector.z, vector.w);
	return pxVector;
}

//------------------------------------------------------------------------------------------------------------------------------
Vec3 PhysXSystem::QuaternionToEulerAngles(const PxQuat& quat, Vec3& eulerAngles)
{
	// roll (x-axis rotation)
	float sinr_cosp = 2.0f * (quat.w * quat.x + quat.y * quat.z);
	float cosr_cosp = 1.0f - 2.0f * (quat.x * quat.x + quat.y * quat.y);
	eulerAngles.x = atan2f(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	float sinp = +2.0 * (quat.w * quat.y - quat.z * quat.x);
	if (fabs(sinp) >= 1)
		eulerAngles.y = copysign(PI / 2, sinp); // use 90 degrees if out of range
	else
		eulerAngles.y = asin(sinp);

	// yaw (z-axis rotation)
	float siny_cosp = 2.0f * (quat.w * quat.z + quat.x * quat.y);
	float cosy_cosp = 1.0f - 2.0f * (quat.y * quat.y + quat.z * quat.z);
	eulerAngles.z = atan2(siny_cosp, cosy_cosp);
}

//------------------------------------------------------------------------------------------------------------------------------
void PhysXSystem::ShutDown()
{
	//Handle all shutdown code here for Nvidia PhysX
	PX_RELEASE(m_PxScene);
	PX_RELEASE(m_PxDispatcher);
	PX_RELEASE(m_PhysX);
	if (m_Pvd)
	{
		PxPvdTransport* transport = m_Pvd->getTransport();
		m_Pvd->release();	m_Pvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(m_PxFoundation);
}

