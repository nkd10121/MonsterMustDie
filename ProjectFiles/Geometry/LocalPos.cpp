#include "LocalPos.h"

namespace
{
	MATRIX GetRotationMat(Vec3 vec)
	{
		MATRIX rotX;
		rotX.m[0][0] = 1; rotX.m[0][1] = 0;                    rotX.m[0][2] = 0;				   rotX.m[0][3] = 0;
		rotX.m[1][0] = 0; rotX.m[1][1] = std::cosf(vec.x);		   rotX.m[1][2] = std::sinf(vec.x);		   rotX.m[1][3] = 0;
		rotX.m[2][0] = 0; rotX.m[2][1] = -std::sinf(vec.x);		   rotX.m[2][2] = std::cosf(vec.x);		   rotX.m[2][3] = 0;
		rotX.m[3][0] = 0; rotX.m[3][1] = 0;                    rotX.m[3][2] = 0;                   rotX.m[3][3] = 1;

		MATRIX rotY;
		rotY.m[0][0] = std::cosf(vec.y); rotY.m[0][1] = 0; rotY.m[0][2] = -std::sinf(vec.y); rotY.m[0][3] = 0;
		rotY.m[1][0] = 0;			 rotY.m[1][1] = 1; rotY.m[1][2] = 0;  		     rotY.m[1][3] = 0;
		rotY.m[2][0] = std::sinf(vec.y); rotY.m[2][1] = 0; rotY.m[2][2] = std::cosf(vec.y);  rotY.m[2][3] = 0;
		rotY.m[3][0] = 0;	  		 rotY.m[3][1] = 0; rotY.m[3][2] = 0;             rotY.m[3][3] = 1;

		MATRIX rotZ;
		rotZ.m[0][0] = std::cosf(vec.z);  rotZ.m[0][1] = std::sinf(vec.z); rotZ.m[0][2] = 0; rotZ.m[0][3] = 0;
		rotZ.m[1][0] = -std::sinf(vec.z); rotZ.m[1][1] = std::cosf(vec.z); rotZ.m[1][2] = 0; rotZ.m[1][3] = 0;
		rotZ.m[2][0] = 0;             rotZ.m[2][1] = 0;			   rotZ.m[2][2] = 1; rotZ.m[2][3] = 0;
		rotZ.m[3][0] = 0; 			  rotZ.m[3][1] = 0;		  	   rotZ.m[3][2] = 0; rotZ.m[3][3] = 1;

		MATRIX rotMat;
		rotMat = MMult(MMult(rotX, rotY), rotZ);

		return rotMat;
	}

	Vec3 MatTransform(Vec3 vec, MATRIX mat)
	{
		Vec3 ret;
		ret.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + mat.m[3][0];
		ret.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + mat.m[3][1];
		ret.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + mat.m[3][2];

		return ret;
	}
}

LocalPos::LocalPos():
	centerPos(),
	localPos(),
	centerRotVec()
{
}

Vec3 LocalPos::GetWorldPos()
{
	Vec3 ret;

	MATRIX rotMat = GetRotationMat(centerRotVec);

	Vec3 move = MatTransform(localPos,rotMat);

	ret = move + centerPos;

	return ret;
}

void LocalPos::SetFrontPos(Vec3 frontPos)
{
	float vX = centerPos.x - frontPos.x;
	float vZ = centerPos.z - frontPos.z;

	float yAngle = std::atan2f(-vX, -vZ);

	Vec3 rot(0.0f, yAngle + DX_PI_F, 0.0f);

	centerRotVec = rot;
}
