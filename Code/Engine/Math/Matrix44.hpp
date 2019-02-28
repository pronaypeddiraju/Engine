//------------------------------------------------------------------------------------------------------------------------------
#pragma once

struct Vec2;
struct Vec3;
struct Vec4;

enum eRotationOrder
{
	ROTATION_ORDER_XYZ,     // Unreal/Squirrel Defualt
	ROTATION_ORDER_ZXY,     // Unity/Forseth Defualt

	ROTATION_ORDER_DEFAULT = ROTATION_ORDER_ZXY 
}; 

//------------------------------------------------------------------------------------------------------------------------------
struct Matrix44
{
	float m_values[16];

public:
	enum Indices
	{
		Ix, Iy, Iz, Iw, Jx, Jy, Jz, Jw, Kx, Ky, Kz, Kw, Tx, Ty, Tz, Tw
	};

	Matrix44();
	explicit Matrix44 (float* sixteenValuesBasisMajor);
	explicit Matrix44 (const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D);
	explicit Matrix44 (const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D);
	explicit Matrix44 (const Vec4& iBasisHomogeneous, const Vec4& jBasisHomogeneous, const Vec4& kBasisHomogeneous, const Vec4& translationHomogeneous);
	explicit Matrix44 (float ix, float iy, float iz, float iw, 
					   float jx, float jy, float jz, float jw,
					   float kx, float ky, float kz, float kw,
					   float tx, float ty, float tz, float tw);

	~Matrix44();

	const static Matrix44 IDENTITY;

	//Transformation methods
	const Vec2			TransformPosition2D( const Vec2& posQuantity ) const;			 //assumes z = 0, w = 1;
	const Vec2			TransformVector2D( const Vec2& vecQuantity ) const;				 //assumes z = 0, w = 0;
	const Vec3			TransformPosition3D( const Vec3& posQuantity ) const;			 //assumes w = 1;
	const Vec3			TransformVector3D( const Vec3& vecQuantity ) const;				 //assumes w = 0;
	const Vec4			TransformHomogeneousPoint3D ( const Vec4& homogeneousVec ) const; //assumes nothing; 

	//Get axis methods
	const Vec3			GetIVector();
	const Vec3			GetJVector();
	const Vec3			GetKVector();

	Matrix44			AppendMatrix( const Matrix44& matrix);

	void				InverseMatrix();

	//Static methods to create required matrix
	static const Matrix44	MakeZRotationDegrees ( float RotationZ );
	static const Matrix44	MakeXRotationDegrees ( float RotationX );
	static const Matrix44	MakeYRotationDegrees ( float RotationY );
	static const Matrix44	MakeOrthoMatrix (const Vec2& min,const Vec2& max, float nearZ, float farZ);
	static const Matrix44	MakePerspectiveMatrix ( float FieldOfView, float nearZ, float farZ, float aspectRatio);

	static const Matrix44	MakeTranslation2D ( const Vec2& translation );
	static const Matrix44	MakeTranslation3D ( const Vec3& translation );
	static const Matrix44	MakeUniformScale2D ( float uniformScale );
	static const Matrix44	MakeFromEuler (const Vec3& euler, eRotationOrder rot = ROTATION_ORDER_ZXY);
	static const Matrix44	InvertOrthoNormal(const Matrix44 sourceMatrix);
	static const Matrix44	SetTranslation3D (const Vec3& position, Matrix44& setMatrix);
	static const Matrix44	TransposeRotationComponents( const Matrix44& sourceMatrix);
	static const Matrix44	Transpose( const Matrix44& sourceMatrix );

	//Operations on Matrix
	// Mat44 * Vec3
	// Mat44 * Vec2
	// Mat44 * Mat44 ?

};
