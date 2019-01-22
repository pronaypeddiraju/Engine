#pragma once

struct Vec2;
struct Vec3;
struct Vec4;

struct Matrix44
{
	float m_values[16];

	enum Indices
	{
		Ix, Iy, Iz, Iw, Jx, Jy, Jz, Jw, Kx, Ky, Kz, Kw, Tx, Ty, Tz, Tw
	};

public:

	Matrix44();
	explicit Matrix44 (float* sixteenValuesBasisMajor);
	explicit Matrix44 (const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D);
	explicit Matrix44 (const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D);
	explicit Matrix44 (const Vec4& iBasisHomogeneous, const Vec4& jBasisHomogeneous, const Vec4& kBasisHomogeneous, const Vec4& translationHomogeneous);
	~Matrix44();

	const static Matrix44 IDENTITY;

	//Transformation methods
	const Vec2 TransformPosition2D( const Vec2& posQuantity ) const;			 //assumes z = 0, w = 1;
	const Vec2 TransformVector2D( const Vec2& vecQuantity ) const;				 //assumes z = 0, w = 0;
	const Vec3 TransformPosition3D( const Vec3& posQuantity ) const;			 //assumes w = 1;
	const Vec3 TransformVector3D( const Vec3& vecQuantity ) const;				 //assumes w = 0;
	const Vec4 TransformHomogeneousPoint3D ( const Vec4& homogeneousVec ) const; //assumes nothing; 

	//Static methods to create required matrix
	static const Matrix44 MakeZRotationDegrees ( float RotationZ );
	static const Matrix44 MakeTranslation2D ( const Vec2& translation );
	static const Matrix44 MakeUniformScale2D ( float uniformScale );
};