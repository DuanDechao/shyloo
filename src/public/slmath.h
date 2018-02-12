#ifndef SL_PUBLIC_MATH_H
#define SL_PUBLIC_MATH_H
#include "slmulti_sys.h"
#include "g3dlite/G3D/g3dmath.h"
#include "g3dlite/G3D/Vector2.h"
#include "g3dlite/G3D/Vector3.h"
#include "g3dlite/G3D/Vector4.h"
#include "g3dlite/G3D/Matrix3.h"
#include "g3dlite/G3D/Quat.h"

typedef G3D::Matrix3							Matrix;
typedef G3D::Quat								Quaternion;
typedef G3D::Vector2							Vector2;
typedef G3D::Vector3							Vector3;
typedef G3D::Vector4							Vector4;

#define SL_PI									3.1415926535898
#define SL_2PI									6.2831853071796
#define SL_DegreeToRadian						G3D::toRadians
#define SL_RadianToDegree						G3D::toRadians
	
#define SLVec2Length(v)						    (v)->length()
#define SLVec2LengthSq(v)						(v)->squaredLength()
#define SLVec2Normalize(v, vv)					(v)->normalise()
#define SLVec2Dot(v, vv)						(v)->dot(static_cast<const G3D::Vector2 &>(*(vv)))
#define SLVec3Length(v)						    (v)->length()
#define SLVec3LengthSq(v)						(v)->squaredLength()
#define SLVec3Dot(v, vv)						(v)->dot(static_cast<const G3D::Vector3 &>(*(vv)))
#define SLVec3Cross(v)							(v)->cross()
#define SLVec3Normalize(v, vv)					(v)->normalise()
#define SLVec3Lerp(v)							(v)->lerp()
#define SLVec3Transform(v)						D3DXVec3Transform
#define SLVec3TransformCoord(v)				    D3DXVec3TransformCoord
#define SLVec3TransformNormal(v)				D3DXVec3TransformNormal
#define SLVec4Transform(v)						D3DXVec4Transform
#define SLVec4Length(v)						    (v)->length()
#define SLVec4LengthSq(v)						(v)->squaredLength()
#define SLVec4Normalize(v, vv)					(v)->normalise()
#define SLVec4Lerp(v)							(v)->lerp()
#define SLVec4Dot(v, vv)						(v)->dot(static_cast<const G3D::Vector4 &>(*(vv)))
#define SLMatrixIdentity						Matrix3::identity()
#define SLMatrixInverse(v)						Matrix3::inverse()
#define SLMatrixRotationQuaternion(v)			D3DXMatrixRotationQuaternion
#define SLMatrixTranspose(v)					D3DXMatrixTranspose
#define SLMatrixfDeterminant(v)				    D3DXMatrixDeterminant
#define SLMatrixOrthoLH(v)						D3DXMatrixOrthoLH
#define SLMatrixLookAtLH(v)					    D3DXMatrixLookAtLH
#define SLMatrixMultiply(v)					    D3DXMatrixMultiply
#define SLMatrixPerspectiveFovLH(v)		    	D3DXMatrixPerspectiveFovLH
#define SLMatrixRotationX(v)					D3DXMatrixRotationX
#define SLMatrixRotationY(v)					D3DXMatrixRotationY
#define SLMatrixRotationZ(v)					D3DXMatrixRotationZ
#define SLMatrixScaling(v)						D3DXMatrixScaling
#define SLMatrixTranslation(v)					D3DXMatrixTranslation
#define SLMatrixRotationYawPitchRoll(v)		    D3DXMatrixRotationYawPitchRoll
#define SLQuaternionDot(v)						D3DXQuaternionDot
#define SLQuaternionNormalize(v)				D3DXQuaternionNormalize
#define SLQuaternionRotationMatrix(v)			D3DXQuaternionRotationMatrix
#define SLQuaternionSlerp(v)					D3DXQuaternionSlerp
#define SLQuaternionRotationAxis(v)			    D3DXQuaternionRotationAxis
#define SLQuaternionMultiply(v)				    D3DXQuaternionMultiply
#define SLQuaternionInverse(v)					D3DXQuaternionInverse
#define SLClamp								    G3D::clamp
#endif

// 从2个3d向量忽略y计算出2d长度
inline float SLVec3CalcVec2Length(const Vector3& v1, const Vector3& v2){
	float x = v1.x - v2.x;
	float z = v1.z - v2.z;
	return sqrt(x*x + z*z);
}

inline float int82angle(int8 angle, bool half = false){
	return float(angle) * float((SL_PI / (half ? 254.f : 128.f)));
}

inline int8 angle2int8(float v, bool half = false){
	int8 angle = 0;
	if(!half){
		angle = (int8)floorf((v * 128.f) / float(SL_PI) + 0.5f);
	}
	else{
		angle = (int8)SLClamp(floorf( (v * 254.f) / float(SL_PI) + 0.5f ), -128.f, 127.f );
	}

	return angle;
}

typedef Vector3													Position3D;												// 表示3D位置变量类型	
typedef std::vector<Position3D>				                    VECTOR_POS3D;										// 指向Position3D数组的智能指针类型声明

struct Direction3D																										// 表示方向位置变量类型
{
	Direction3D():dir(0.f, 0.f, 0.f) {};
	Direction3D(const Vector3 & v):dir(v){}
	Direction3D(float r, float p, float y):dir(r, p, y){}
	Direction3D(const Direction3D & v) :dir(v.dir){}

	Direction3D& operator=(const Direction3D& v)
	{
		dir = v.dir;
		return *this;
	}

	float roll() const{ return dir.x; }		
	float pitch() const{ return dir.y; }		
	float yaw() const{ return dir.z; }		

	void roll(float v){ dir.x = v; }		
	void pitch(float v){ dir.y = v; }		
	void yaw(float v){ dir.z = v; }	

	// roll, pitch, yaw
	Vector3 dir;
};

/** 浮点数比较 */
#define floatEqual(v1, v3) (abs(v1 - v2) < std::numeric_limits<float>::epsilon())
inline bool almostEqual(const float f1, const float f2, const float epsilon = 0.0004f)
{
	return fabsf( f1 - f2 ) < epsilon;
}

inline bool almostEqual(const double d1, const double d2, const double epsilon = 0.0004)
{
	return fabs( d1 - d2 ) < epsilon;
}

inline bool almostZero(const float f, const float epsilon = 0.0004f)
{
	return f < epsilon && f > -epsilon;
}

inline bool almostZero(const double d, const double epsilon = 0.0004)
{
	return d < epsilon && d > -epsilon;
}

template<typename T>
inline bool almostEqual(const T& c1, const T& c2, const float epsilon = 0.0004f)
{
	if( c1.size() != c2.size() )
		return false;
	typename T::const_iterator iter1 = c1.begin();
	typename T::const_iterator iter2 = c2.begin();
	for( ; iter1 != c1.end(); ++iter1, ++iter2 )
		if( !almostEqual( *iter1, *iter2, epsilon ) )
			return false;
	return true;
}
