#include "Matrix.h"

Matrix4::Matrix4(void)
{
	reset();
}

Matrix4::~Matrix4(void)
{

}

void Matrix4::reset()
{
	for(int i = 0; i < 16; i++)
	{
			matrix[i]=0;
	}
	matrix[0] = matrix[5] =
	matrix[10] = matrix[15]=1;

	m00 = &matrix[0];
	m01 = &matrix[1];
	m02 = &matrix[2];
	m03 = &matrix[3];

	m10 = &matrix[4];
	m11 = &matrix[5];
	m12 = &matrix[6];
	m13 = &matrix[7];

	m20 = &matrix[8];
	m21 = &matrix[9];
	m22 = &matrix[10];
	m23 = &matrix[11];

	m30 = &matrix[12];
	m31 = &matrix[13];
	m32 = &matrix[14];
	m33 = &matrix[15];
}

std::string Matrix4::toString()
{
	std::stringstream ss;
	ss << "\n" << matrix[0] << " " << matrix[4] << " " << matrix[8] << " " << matrix[12] << "\n";
	ss << matrix[1] << " " << matrix[5] << " " << matrix[9] << " " << matrix[13] << "\n";
	ss << matrix[2] << " " << matrix[6] << " " << matrix[10] << " " << matrix[14] << "\n";
	ss << matrix[3] << " " << matrix[7] << " " << matrix[11] << " " << matrix[15] << "";
	return ss.str();
}

Vector3 Matrix4::getTranslation()
{
	return Vector3(matrix[12],matrix[13],matrix[14],matrix[15]);
}

void Matrix4::translate(Vector3& v)
{
	matrix[12] += v.x;
	matrix[13] += v.y;
	matrix[14] += v.z;
	matrix[15] = 1;
}

Matrix4 Matrix4::rotateX( float angle )
{
	Matrix4 matrix;
	matrix.matrix[0] = 1;
	matrix.matrix[5] = cos(angle);
	matrix.matrix[6] = sin(angle);
	matrix.matrix[9] = -sin(angle);
	matrix.matrix[10] = cos(angle);
	return matrix;
}

Matrix4 Matrix4::rotateY( float angle )
{
	Matrix4 matrix;
	matrix.matrix[0] = cos(angle);
	matrix.matrix[2] = sin(angle);
	matrix.matrix[5] = 1;
	matrix.matrix[8] = -sin(angle);
	matrix.matrix[10] = cos(angle);
	return matrix;
}

Matrix4 Matrix4::rotateZ( float angle )
{
	Matrix4 matrix;
	matrix.matrix[0] = cos(angle);
	matrix.matrix[1] = sin(angle);
	matrix.matrix[4] = -sin(angle);
	matrix.matrix[5] = cos(angle);
	return matrix;
}

Vector3 Matrix4::operator* (Vector3 &vec) 
{
	Vector3 final;
	final.x = vec.x * matrix[0] + vec.y * matrix[4] + vec.z * matrix[8] + matrix[12];
	final.y = vec.x * matrix[1] + vec.y * matrix[5] + vec.z * matrix[9] + matrix[13];
	final.y = vec.x * matrix[2] + vec.y * matrix[6] + vec.z * matrix[10] + matrix[14];
	final.w = vec.x * matrix[3] + vec.y * matrix[7] + vec.z * matrix[11] + matrix[15];
	return final;
}

Matrix4 Matrix4::operator * (Matrix4 &mat)
{
	Matrix4 C;
    *C.m00 = *m00 * *mat.m00 + *m01 * *mat.m10 + *m02 * *mat.m20 + *m03 * *mat.m30;
    *C.m01 = *m00 * *mat.m01 + *m01 * *mat.m11 + *m02 * *mat.m21 + *m03 * *mat.m31;
    *C.m02 = *m00 * *mat.m02 + *m01 * *mat.m12 + *m02 * *mat.m22 + *m03 * *mat.m32;
    *C.m03 = *m00 * *mat.m03 + *m01 * *mat.m13 + *m02 * *mat.m23 + *m03 * *mat.m33;
    *C.m10 = *m10 * *mat.m00 + *m11 * *mat.m10 + *m12 * *mat.m20 + *m13 * *mat.m30;
    *C.m11 = *m10 * *mat.m01 + *m11 * *mat.m11 + *m12 * *mat.m21 + *m13 * *mat.m31;
    *C.m12 = *m10 * *mat.m02 + *m11 * *mat.m12 + *m12 * *mat.m22 + *m13 * *mat.m32;
    *C.m13 = *m10 * *mat.m03 + *m11 * *mat.m13 + *m12 * *mat.m23 + *m13 * *mat.m33;
    *C.m20 = *m20 * *mat.m00 + *m21 * *mat.m10 + *m22 * *mat.m20 + *m23 * *mat.m30;
    *C.m21 = *m20 * *mat.m01 + *m21 * *mat.m11 + *m22 * *mat.m21 + *m23 * *mat.m31;
    *C.m22 = *m20 * *mat.m02 + *m21 * *mat.m12 + *m22 * *mat.m22 + *m23 * *mat.m32;
    *C.m23 = *m20 * *mat.m03 + *m21 * *mat.m13 + *m22 * *mat.m23 + *m23 * *mat.m33;
    *C.m30 = *m30 * *mat.m00 + *m31 * *mat.m10 + *m32 * *mat.m20 + *m33 * *mat.m30;
    *C.m31 = *m30 * *mat.m01 + *m31 * *mat.m11 + *m32 * *mat.m21 + *m33 * *mat.m31;
    *C.m32 = *m30 * *mat.m02 + *m31 * *mat.m12 + *m32 * *mat.m22 + *m33 * *mat.m32;
    *C.m33 = *m30 * *mat.m03 + *m31 * *mat.m13 + *m32 * *mat.m23 + *m33 * *mat.m33;
    
    return C;
}