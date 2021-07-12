#include "Matrix.h"

using namespace hos;
using namespace DirectX;

hos::Matrix::Matrix(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
	:SimpleMath::Matrix(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33)
{
}

hos::Matrix::Matrix(DirectX::SimpleMath::Matrix matrix) :
	SimpleMath::Matrix(matrix)
{
}

hos::Matrix::~Matrix()
{
}

Matrix& hos::Matrix::operator=(const SimpleMath::Matrix& mat)
{
	m[0][0] = mat.m[0][0];
	m[0][1] = mat.m[0][1];
	m[0][2] = mat.m[0][2];
	m[0][3] = mat.m[0][3];
	m[1][0] = mat.m[1][0];
	m[1][1] = mat.m[1][1];
	m[1][2] = mat.m[1][2];
	m[1][3] = mat.m[1][3];
	m[2][0] = mat.m[2][0];
	m[2][1] = mat.m[2][1];
	m[2][2] = mat.m[2][2];
	m[2][3] = mat.m[2][3];
	m[3][0] = mat.m[3][0];
	m[3][1] = mat.m[3][1];
	m[3][2] = mat.m[3][2];
	m[3][3] = mat.m[3][3];

	return *this;
}

Matrix& hos::Matrix::operator=(const Matrix& mat)
{
	m[0][0] = mat.m[0][0];
	m[0][1] = mat.m[0][1];
	m[0][2] = mat.m[0][2];
	m[0][3] = mat.m[0][3];
	m[1][0] = mat.m[1][0];
	m[1][1] = mat.m[1][1];
	m[1][2] = mat.m[1][2];
	m[1][3] = mat.m[1][3];
	m[2][0] = mat.m[2][0];
	m[2][1] = mat.m[2][1];
	m[2][2] = mat.m[2][2];
	m[2][3] = mat.m[2][3];
	m[3][0] = mat.m[3][0];
	m[3][1] = mat.m[3][1];
	m[3][2] = mat.m[3][2];
	m[3][3] = mat.m[3][3];

	return *this;
}

#ifdef PHYSX_MATH
hos::Matrix::Matrix(physx::PxMat44 matrix) :
	SimpleMath::Matrix(
		matrix.column0[0], matrix.column0[1], matrix.column0[2], matrix.column0[3],
		matrix.column1[0], matrix.column1[1], matrix.column1[2], matrix.column1[3],
		matrix.column2[0], matrix.column2[1], matrix.column2[2], matrix.column2[3],
		matrix.column3[0], matrix.column3[1], matrix.column3[2], matrix.column3[3])
{
}

Matrix& hos::Matrix::operator=(const physx::PxMat44& mat)
{
	m[0][0] = mat.column0[0];
	m[0][1] = mat.column0[1];
	m[0][2] = mat.column0[2];
	m[0][3] = mat.column0[3];
	m[1][0] = mat.column1[0];
	m[1][1] = mat.column1[1];
	m[1][2] = mat.column1[2];
	m[1][3] = mat.column1[3];
	m[2][0] = mat.column2[0];
	m[2][1] = mat.column2[1];
	m[2][2] = mat.column2[2];
	m[2][3] = mat.column2[3];
	m[3][0] = mat.column3[0];
	m[3][1] = mat.column3[1];
	m[3][2] = mat.column3[2];
	m[3][3] = mat.column3[3];

	return *this;
}

hos::Matrix::operator physx::PxMat44()
{
	float m[16] = {
		_11,_12,_13,_14,
		_21,_22,_23,_24,
		_31,_32,_33,_34,
		_41,_42,_43,_44,
	};
	return physx::PxMat44(m);
}
#endif // PHYSX_MATH