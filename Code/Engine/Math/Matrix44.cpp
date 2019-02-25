#include "Engine/Math/Matrix44.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec4.hpp"

const STATIC Matrix44 Matrix44::IDENTITY;

const Vec2 Matrix44::TransformPosition2D( const Vec2& posQuantity ) const				//assumes z = 0, w = 1;
{
	Vec2 transformedPos;
	
	transformedPos.x = m_values[Ix] * posQuantity.x;
	transformedPos.x += m_values[Jx] * posQuantity.y;
	transformedPos.x += m_values[Tx];

	transformedPos.y = m_values[Iy] * posQuantity.x;
	transformedPos.y += m_values[Jy] * posQuantity.y;
	transformedPos.y += m_values[Ty];

	return transformedPos;
}

const Vec3 Matrix44::TransformPosition3D( const Vec3& posQuantity ) const				//assumes w = 1;
{
	Vec3 transformedPos;

	transformedPos.x = m_values[Ix] * posQuantity.x;
	transformedPos.x += m_values[Jx] * posQuantity.y;
	transformedPos.x += m_values[Kx] * posQuantity.z;
	transformedPos.x += m_values[Tx];

	transformedPos.y = m_values[Iy] * posQuantity.x;
	transformedPos.y += m_values[Jy] * posQuantity.y;
	transformedPos.y += m_values[Ky] * posQuantity.z;
	transformedPos.y += m_values[Ty];

	transformedPos.z = m_values[Iz] * posQuantity.x;
	transformedPos.z += m_values[Jz] * posQuantity.y;
	transformedPos.z += m_values[Kz] * posQuantity.z;
	transformedPos.z += m_values[Tz];

	return transformedPos;
}

const Vec2 Matrix44::TransformVector2D( const Vec2& vecQuantity ) const				//assumes z = 0, w = 0;
{
	Vec2 transformedVec;

	transformedVec.x = m_values[Ix] * vecQuantity.x;
	transformedVec.x += m_values[Jx] * vecQuantity.y;

	transformedVec.y = m_values[Iy] * vecQuantity.x;
	transformedVec.y += m_values[Jy] * vecQuantity.y;

	return transformedVec;
}

const Vec3 Matrix44::TransformVector3D( const Vec3& vecQuantity ) const				//assumes w = 0;
{
	Vec3 transformedVec;

	transformedVec.x = m_values[Ix] * vecQuantity.x;
	transformedVec.x += m_values[Jx] * vecQuantity.y;
	transformedVec.x += m_values[Kx] * vecQuantity.z;

	transformedVec.y = m_values[Iy] * vecQuantity.x;
	transformedVec.y += m_values[Jy] * vecQuantity.y;
	transformedVec.y += m_values[Ky] * vecQuantity.z;	

	transformedVec.z = m_values[Iz] * vecQuantity.x;
	transformedVec.z += m_values[Jz] * vecQuantity.y;
	transformedVec.z += m_values[Kz] * vecQuantity.z;

	return transformedVec;
}

const Vec4 Matrix44::TransformHomogeneousPoint3D( const Vec4& homogeneousVec ) const
{
	Vec4 transformedPos;

	transformedPos.x = m_values[Ix] * homogeneousVec.x;
	transformedPos.x += m_values[Jx] * homogeneousVec.y;
	transformedPos.x += m_values[Kx] * homogeneousVec.z;
	transformedPos.x += m_values[Tx] * homogeneousVec.w;

	transformedPos.y = m_values[Iy] * homogeneousVec.x;
	transformedPos.y += m_values[Jy] * homogeneousVec.y;
	transformedPos.y += m_values[Ky] * homogeneousVec.z;
	transformedPos.y += m_values[Ty] * homogeneousVec.w;

	transformedPos.z = m_values[Iz] * homogeneousVec.x;
	transformedPos.z += m_values[Jz] * homogeneousVec.y;
	transformedPos.z += m_values[Kz] * homogeneousVec.z;
	transformedPos.z += m_values[Tz] * homogeneousVec.w;

	transformedPos.w = m_values[Iw] * homogeneousVec.x;
	transformedPos.w += m_values[Jw] * homogeneousVec.y;
	transformedPos.w += m_values[Kw] * homogeneousVec.z;
	transformedPos.w += m_values[Tw] * homogeneousVec.w;

	return transformedPos;
}

/*

Matrix.Append()

A= (AIx, AJx, AKx, ATx,
	AIy, AJy, AKy, ATy,
	AIz, AJz, AKz, ATz,
	AIw, AJw, AKw, ATw )

B= (BIx, BJx, BKx, BTx,
	BIy, BJy, BKy, BTy,
	BIz, BJz, BKz, BTz,
	BIw, BJw, BKw, BTw )

C= (BIxAIx+(BIyAJx+(BIzAKx+BIwATx)), BJxAIx+(BJyAJx+(BJzAKx+BJwATx)), BKxAIx+(BKyAJx+(BKzAKx+BKwATx)), BTxAIx+(BTyAJx+(BTzAKx+BTwATx)),
	BIxAIy+(BIyAJy+(BIzAKy+BIwATy)), BJxAIy+(BJyAJy+(BJzAKy+BJwATy)), BKxAIy+(BKyAJy+(BKzAKy+BKwATy)), BTxAIy+(BTyAJy+(BTzAKy+BTwATy)),
	BIxAIz+(BIyAJz+(BIzAKz+BIwATz)), BJxAIz+(BJyAJz+(BJzAKz+BJwATz)), BKxAIz+(BKyAJz+(BKzAKz+BKwATz)), BTxAIz+(BTyAJz+(BTzAKz+BTwATz)),
	BIxAIw+(BIyAJw+(BIzAKw+BIwATw)), BJxAIw+(BJyAJw+(BJzAKw+BJwATw)), BKxAIw+(BKyAJw+(BKzAKw+BKwATw)), BTxAIw+(BTyAJw+(BTzAKw+BTwATw)) )
	
*/

Matrix44 Matrix44::AppendMatrix( const Matrix44& matrix )
{
	Matrix44 appendMatrix;

	//Row 1
	appendMatrix.m_values[Ix] = (m_values[Ix] * matrix.m_values[Ix]) +
								(m_values[Jx] * matrix.m_values[Iy]) + 
								(m_values[Kx] * matrix.m_values[Iz]) +
								(m_values[Tx] * matrix.m_values[Iw]);

	appendMatrix.m_values[Jx] = (m_values[Ix] * matrix.m_values[Jx]) +
								(m_values[Jx] * matrix.m_values[Jy]) + 
								(m_values[Kx] * matrix.m_values[Jz]) +
								(m_values[Tx] * matrix.m_values[Jw]);

	appendMatrix.m_values[Kx] = (m_values[Ix] * matrix.m_values[Kx]) +
								(m_values[Jx] * matrix.m_values[Ky]) + 
								(m_values[Kx] * matrix.m_values[Kz]) +
								(m_values[Tx] * matrix.m_values[Kw]);

	appendMatrix.m_values[Tx] = (m_values[Ix] * matrix.m_values[Tx]) +
								(m_values[Jx] * matrix.m_values[Ty]) + 
								(m_values[Kx] * matrix.m_values[Tz]) +
								(m_values[Tx] * matrix.m_values[Tw]);

	//Row 2
	appendMatrix.m_values[Iy] = (m_values[Iy] * matrix.m_values[Ix]) +
								(m_values[Jy] * matrix.m_values[Iy]) + 
								(m_values[Ky] * matrix.m_values[Iz]) +
								(m_values[Ty] * matrix.m_values[Iw]);

	appendMatrix.m_values[Jy] = (m_values[Iy] * matrix.m_values[Jx]) +
								(m_values[Jy] * matrix.m_values[Jy]) + 
								(m_values[Ky] * matrix.m_values[Jz]) +
								(m_values[Ty] * matrix.m_values[Jw]);

	appendMatrix.m_values[Ky] = (m_values[Iy] * matrix.m_values[Kx]) +
								(m_values[Jy] * matrix.m_values[Ky]) + 
								(m_values[Ky] * matrix.m_values[Kz]) +
								(m_values[Ty] * matrix.m_values[Kw]);

	appendMatrix.m_values[Ty] = (m_values[Iy] * matrix.m_values[Tx]) +
								(m_values[Jy] * matrix.m_values[Ty]) + 
								(m_values[Ky] * matrix.m_values[Tz]) +
								(m_values[Ty] * matrix.m_values[Tw]);

	//Row 3
	appendMatrix.m_values[Iz] = (m_values[Iz] * matrix.m_values[Ix]) +
								(m_values[Jz] * matrix.m_values[Iy]) + 
								(m_values[Kz] * matrix.m_values[Iz]) +
								(m_values[Tz] * matrix.m_values[Iw]);

	appendMatrix.m_values[Jz] = (m_values[Iz] * matrix.m_values[Jx]) +
								(m_values[Jz] * matrix.m_values[Jy]) + 
								(m_values[Kz] * matrix.m_values[Jz]) +
								(m_values[Tz] * matrix.m_values[Jw]);

	appendMatrix.m_values[Kz] = (m_values[Iz] * matrix.m_values[Kx]) +
								(m_values[Jz] * matrix.m_values[Ky]) + 
								(m_values[Kz] * matrix.m_values[Kz]) +
								(m_values[Tz] * matrix.m_values[Kw]);

	appendMatrix.m_values[Tz] = (m_values[Iz] * matrix.m_values[Tx]) +
								(m_values[Jz] * matrix.m_values[Ty]) + 
								(m_values[Kz] * matrix.m_values[Tz]) +
								(m_values[Tz] * matrix.m_values[Tw]);

	//Row 4
	appendMatrix.m_values[Iw] = (m_values[Iw] * matrix.m_values[Ix]) +
								(m_values[Jw] * matrix.m_values[Iy]) + 
								(m_values[Kw] * matrix.m_values[Iz]) +
								(m_values[Tw] * matrix.m_values[Iw]);

	appendMatrix.m_values[Jw] = (m_values[Iw] * matrix.m_values[Jx]) +
								(m_values[Jw] * matrix.m_values[Jy]) + 
								(m_values[Kw] * matrix.m_values[Jz]) +
								(m_values[Tw] * matrix.m_values[Jw]);

	appendMatrix.m_values[Kw] = (m_values[Iw] * matrix.m_values[Kx]) +
								(m_values[Jw] * matrix.m_values[Ky]) + 
								(m_values[Kw] * matrix.m_values[Kz]) +
								(m_values[Tw] * matrix.m_values[Kw]);

	appendMatrix.m_values[Tw] = (m_values[Iw] * matrix.m_values[Tx]) +
								(m_values[Jw] * matrix.m_values[Ty]) + 
								(m_values[Kw] * matrix.m_values[Tz]) +
								(m_values[Tw] * matrix.m_values[Tw]);

	return appendMatrix;
}

Matrix44::Matrix44()
{
	//Create an identity matrix by default

	m_values[Ix] = 1.f;
	m_values[Iy] = 0.f;
	m_values[Iz] = 0.f;
	m_values[Iw] = 0.f;

	m_values[Jx] = 0.f;
	m_values[Jy] = 1.f;
	m_values[Jz] = 0.f;
	m_values[Jw] = 0.f;

	m_values[Kx] = 0.f;
	m_values[Ky] = 0.f;
	m_values[Kz] = 1.f;
	m_values[Kw] = 0.f;

	m_values[Tx] = 0.f;
	m_values[Ty] = 0.f;
	m_values[Tz] = 0.f;
	m_values[Tw] = 1.f;

}

Matrix44::Matrix44( float* sixteenValuesBasisMajor )
{
	m_values[Ix] = sixteenValuesBasisMajor[0];
	m_values[Iy] = sixteenValuesBasisMajor[1];
	m_values[Iz] = sixteenValuesBasisMajor[2];
	m_values[Iw] = sixteenValuesBasisMajor[3];

	m_values[Jx] = sixteenValuesBasisMajor[4];
	m_values[Jy] = sixteenValuesBasisMajor[5];
	m_values[Jz] = sixteenValuesBasisMajor[6];
	m_values[Jw] = sixteenValuesBasisMajor[7];

	m_values[Kx] = sixteenValuesBasisMajor[8];
	m_values[Ky] = sixteenValuesBasisMajor[9];
	m_values[Kz] = sixteenValuesBasisMajor[10];
	m_values[Kw] = sixteenValuesBasisMajor[11];

	m_values[Tx] = sixteenValuesBasisMajor[12];
	m_values[Ty] = sixteenValuesBasisMajor[13];
	m_values[Tz] = sixteenValuesBasisMajor[14];
	m_values[Tw] = sixteenValuesBasisMajor[15];

}

Matrix44::Matrix44( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D )
{
	m_values[Ix] = iBasis2D.x;
	m_values[Iy] = iBasis2D.y;
	m_values[Iz] = 0.f;
	m_values[Iw] = 0.f;

	m_values[Jx] = jBasis2D.x;
	m_values[Jy] = jBasis2D.y;
	m_values[Jz] = 0.f;
	m_values[Jw] = 0.f;

	m_values[Kx] = 0.f;
	m_values[Ky] = 0.f;
	m_values[Kz] = 1.f;
	m_values[Kw] = 0.f;

	m_values[Tx] = translation2D.x;
	m_values[Ty] = translation2D.y;
	m_values[Tz] = 0.f;
	m_values[Tw] = 1.f;
}



Matrix44::Matrix44( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D )
{
	m_values[Ix] = iBasis3D.x;
	m_values[Iy] = iBasis3D.y;
	m_values[Iz] = iBasis3D.z;
	m_values[Iw] = 0.f;

	m_values[Jx] = jBasis3D.x;
	m_values[Jy] = jBasis3D.y;
	m_values[Jz] = jBasis3D.z;
	m_values[Jw] = 0.f;

	m_values[Kx] = kBasis3D.x;
	m_values[Ky] = kBasis3D.y;
	m_values[Kz] = kBasis3D.z;
	m_values[Kw] = 0.f;

	m_values[Tx] = translation3D.x;
	m_values[Ty] = translation3D.y;
	m_values[Tz] = translation3D.z;
	m_values[Tw] = 1.f;
}

Matrix44::Matrix44( const Vec4& iBasisHomogeneous, const Vec4& jBasisHomogeneous, const Vec4& kBasisHomogeneous, const Vec4& translationHomogeneous )
{
	m_values[Ix] = iBasisHomogeneous.x;
	m_values[Iy] = iBasisHomogeneous.y;
	m_values[Iz] = iBasisHomogeneous.z;
	m_values[Iw] = iBasisHomogeneous.w;

	m_values[Jx] = jBasisHomogeneous.x;
	m_values[Jy] = jBasisHomogeneous.y;
	m_values[Jz] = jBasisHomogeneous.z;
	m_values[Jw] = jBasisHomogeneous.w;

	m_values[Kx] = kBasisHomogeneous.x;
	m_values[Ky] = kBasisHomogeneous.y;
	m_values[Kz] = kBasisHomogeneous.z;
	m_values[Kw] = kBasisHomogeneous.w;

	m_values[Tx] = translationHomogeneous.x;
	m_values[Ty] = translationHomogeneous.y;
	m_values[Tz] = translationHomogeneous.z;
	m_values[Tw] = translationHomogeneous.w;
}

Matrix44::Matrix44( float ix, float iy, float iz, float iw, float jx, float jy, float jz, float jw, float kx, float ky, float kz, float kw, float tx, float ty, float tz, float tw )
{
	m_values[Ix] = ix;
	m_values[Iy] = iy;
	m_values[Iz] = iz;
	m_values[Iw] = iw;

	m_values[Jx] = jx;
	m_values[Jy] = jy;
	m_values[Jz] = jz;
	m_values[Jw] = jw;

	m_values[Kx] = kx;
	m_values[Ky] = ky;
	m_values[Kz] = kz;
	m_values[Kw] = kw;

	m_values[Tx] = tx;
	m_values[Ty] = ty;
	m_values[Tz] = tz;
	m_values[Tw] = tw;
}

Matrix44::~Matrix44()
{

}

const STATIC Matrix44 Matrix44::MakeTranslation2D( const Vec2& translation )
{
	Matrix44 translationMatrix;
	translationMatrix.m_values[Tx] = translation.x;
	translationMatrix.m_values[Ty] = translation.y;

	return translationMatrix;
}

const Matrix44 Matrix44::MakeTranslation3D( const Vec3& translation )
{
	Matrix44 translationMatrix;
	translationMatrix.m_values[Tx] = translation.x;
	translationMatrix.m_values[Ty] = translation.y;
	translationMatrix.m_values[Tz] = translation.z;

	return translationMatrix;
}

const STATIC Matrix44 Matrix44::MakeZRotationDegrees( float RotationZ )
{
	float consine = CosDegrees(RotationZ);
	float sine = SinDegrees(RotationZ);
	
	Matrix44 rotationMatrix;
	rotationMatrix.m_values[Ix] = consine;
	rotationMatrix.m_values[Iy] = sine;
	rotationMatrix.m_values[Jx] = -sine;
	rotationMatrix.m_values[Jy] = consine;

	return rotationMatrix;
}

const STATIC Matrix44 Matrix44::MakeXRotationDegrees( float RotationX )
{
	float cosine = CosDegrees(RotationX);
	float sine = SinDegrees(RotationX);

	Matrix44 rotationMatrix;
	rotationMatrix.m_values[Jy] = cosine;
	rotationMatrix.m_values[Jz] = -sine;
	rotationMatrix.m_values[Ky] = sine;
	rotationMatrix.m_values[Kz] = cosine;

	return rotationMatrix;
}

const STATIC Matrix44 Matrix44::MakeYRotationDegrees( float RotationY )
{
	float cosine = CosDegrees(RotationY);
	float sine = SinDegrees(RotationY);

	Matrix44 rotationMatrix;
	rotationMatrix.m_values[Ix] = cosine;
	rotationMatrix.m_values[Iz] = sine;
	rotationMatrix.m_values[Kx] = -sine;
	rotationMatrix.m_values[Kz] = cosine;

	return rotationMatrix;
}

const Matrix44 Matrix44::MakeOrthoMatrix(const Vec2& min, const Vec2& max, float nearZ, float farZ)
{
	Matrix44 orthoMatrix;

	orthoMatrix.m_values[Matrix44::Ix] = 2 / (max.x - min.x); 
	orthoMatrix.m_values[Matrix44::Jy] = 2 / (max.y - min.y);	
	orthoMatrix.m_values[Matrix44::Kz] = 1 / (farZ - nearZ); // used to remap z to [0,1]
	orthoMatrix.m_values[Matrix44::Tx] = -(min.x + max.x )/ (max.x - min.x);
	orthoMatrix.m_values[Matrix44::Ty] = -(min.y + max.y )/ (max.y - min.y);	
	orthoMatrix.m_values[Matrix44::Tz] = -nearZ / (farZ - nearZ) ;
	
	return orthoMatrix;
}

const Matrix44 Matrix44::MakePerspectiveMatrix( float FieldOfView, float nearZ, float farZ, float aspectRatio )
{
	Matrix44 projectionMatrix;

	float rads = DegreesToRadians(FieldOfView); 
	float size = 1.0f / tanf(rads / 2.0f);

	float width = size;
	float height = size;

	// Forseth always grows the side that is large (so fov is for the minimum dimension)
	// This is a personal choice - most engines will just always prefer either width
	// or height (usually height); 
	if (aspectRatio > 1.0f) {
		width /= aspectRatio;
	} else {
		height *= aspectRatio;
	}

	projectionMatrix.m_values[Matrix44::Ix] = width; //Scale the x coordinates of the projected point
	projectionMatrix.m_values[Matrix44::Jy] = height;	// scale the y coordinates of the projected point
	projectionMatrix.m_values[Matrix44::Kz] = farZ / (farZ - nearZ); // used to remap z to [0,1]
	projectionMatrix.m_values[Matrix44::Tz] = -farZ * nearZ / (farZ - nearZ); // used to remap z to [0,1]
	projectionMatrix.m_values[Matrix44::Kw] = 1; 
	projectionMatrix.m_values[Matrix44::Tw] = 0;

	return projectionMatrix;
}

const STATIC Matrix44 Matrix44::MakeUniformScale2D( float uniformScale )
{
	Matrix44 uniformScaleMatrix;
	uniformScaleMatrix.m_values[Ix] = uniformScale;
	uniformScaleMatrix.m_values[Jy] = uniformScale;

	return uniformScaleMatrix;
}

//To do: Write down the actual multiplication of the 3 matrices and quickly plug in those values to make this process faster
// as opposed to making the 3 rotation matrices and then appending 2 of them 
const STATIC Matrix44 Matrix44::MakeFromEuler( const Vec3& euler, const Vec3& position, eRotationOrder rotationOrder )
{
	Matrix44 rotatedX = MakeXRotationDegrees(euler.x); 
	Matrix44 rotatedY = MakeYRotationDegrees(euler.y); 
	Matrix44 rotatedZ = MakeZRotationDegrees(euler.z); 

	// for games, we usually go around
	// forward, then right, then up
	Matrix44 returnMatrix; 
	if (rotationOrder == ROTATION_ORDER_ZXY) {
		returnMatrix = rotatedZ;  // same as rotz.transform(returnMatrix) since returnMatrix is the identity
		returnMatrix = rotatedX.AppendMatrix(returnMatrix); 
		returnMatrix = rotatedY.AppendMatrix(returnMatrix); 
	} else {
		// other case of XYZ
		returnMatrix = rotatedX;  // same as rotz.transform(returnMatrix) since returnMatrix is the identity
		returnMatrix = rotatedY.AppendMatrix(returnMatrix); 
		returnMatrix = rotatedZ.AppendMatrix(returnMatrix); 
	}

	if(position == Vec3::ZERO)
	{
		return returnMatrix;
	}

	returnMatrix = SetTranslation3D(position, returnMatrix);

	return returnMatrix; 
}

const STATIC Matrix44 Matrix44::InvertOrthoNormal( const Matrix44 sourceMatrix )
{
	Matrix44 invertedMatrix = sourceMatrix;
	
	invertedMatrix.m_values[Tx] *= -1;
	invertedMatrix.m_values[Ty] *= -1;
	invertedMatrix.m_values[Tz] *= -1;

	return invertedMatrix;
}

const STATIC Matrix44 Matrix44::SetTranslation3D(const Vec3& position, Matrix44& setMatrix)
{
	setMatrix.m_values[Tx] = position.x;
	setMatrix.m_values[Ty] = position.y;
	setMatrix.m_values[Tz] = position.z;

	return setMatrix;
}
