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

	m[0][0] = m[1][1] =
	m[2][2] = m[3][3] =1;
}

void Matrix4::zero()
{
	for(int i = 0; i < 16; i++)
	{
			matrix[i]=0;
	}
}

std::string Matrix4::toString()
{
	std::stringstream ss;
	ss << "\n";
	ss << m[0][0] << " " << m[1][0] << " " << m[2][0] << " " << m[3][0] << "\n";
	ss << m[0][1] << " " << m[1][1] << " " << m[2][1] << " " << m[3][1] << "\n";
	ss << m[0][2] << " " << m[1][2] << " " << m[2][2] << " " << m[3][2] << "\n";
	ss << m[0][3] << " " << m[1][3] << " " << m[2][3] << " " << m[3][3] << "";
	return ss.str();
}

Vector3 Matrix4::getTranslation()
{
	return Vector3(m[0][3],m[1][3],m[2][3],m[3][3]);
}

void Matrix4::translate(Vector3& v)
{
	m[0][3] += v.x;
	m[1][3] += v.y;
	m[2][3] += v.z;
	m[3][3] = 1;
}

Matrix4 Matrix4::rotateX( float angle )
{
	Matrix4 matrix;
	matrix.m[0][0] =	1;
	matrix.m[1][1] = cos(angle);
	matrix.m[2][1] = -sin(angle);
	matrix.m[1][2] = sin(angle);
	matrix.m[2][2] = cos(angle);
	return matrix;
}

Matrix4 Matrix4::rotateY( float angle )
{
	Matrix4 matrix;
	matrix.m[0][0] = cos( angle );
	matrix.m[0][2] = -sin( angle );
	matrix.m[1][1] = 1;
	matrix.m[2][0] = sin( angle );
	matrix.m[2][2] = cos( angle);
	return matrix;
}

Matrix4 Matrix4::rotateZ( float angle )
{
	Matrix4 matrix;
	matrix.m[0][0] = cos(angle);
	matrix.m[0][1] = -sin(angle);
	matrix.m[0][1] = sin(angle);
	matrix.m[1][1] = cos(angle);
	matrix.m[2][2] = 1;
	return matrix;
}

Vector3 Matrix4::operator* (const Vector3 &vec) 
{
	float w = ( m[3][0] * vec.x + m[3][1] * vec.y + m[3][2] * vec.z + m[3][3] );
	
	Vector3 final;
	final.x = (m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z )/ w;
	final.y = (m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z )/ w; 
	final.z = (m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z )/ w;
	return final;
}

Matrix4 Matrix4::operator * (const Matrix4 &mat)
{
	Matrix4 C;
    C.m[0][0] = m[0][0] * mat.m[0][0] + m[0][1] * mat.m[1][0] + m[0][2] * mat.m[2][0] + m[0][3] * mat.m[3][0];
    C.m[0][1] = m[0][0] * mat.m[0][1] + m[0][1] * mat.m[1][1] + m[0][2] * mat.m[2][1] + m[0][3] * mat.m[3][1];
    C.m[0][2] = m[0][0] * mat.m[0][2] + m[0][1] * mat.m[1][2] + m[0][2] * mat.m[2][2] + m[0][3] * mat.m[3][2];
    C.m[0][3] = m[0][0] * mat.m[0][3] + m[0][1] * mat.m[1][3] + m[0][2] * mat.m[2][3] + m[0][3] * mat.m[3][3];
    C.m[1][0] = m[1][0] * mat.m[0][0] + m[1][1] * mat.m[1][0] + m[1][2] * mat.m[2][0] + m[1][3] * mat.m[3][0];
    C.m[1][1] = m[1][0] * mat.m[0][1] + m[1][1] * mat.m[1][1] + m[1][2] * mat.m[2][1] + m[1][3] * mat.m[3][1];
    C.m[1][2] = m[1][0] * mat.m[0][2] + m[1][1] * mat.m[1][2] + m[1][2] * mat.m[2][2] + m[1][3] * mat.m[3][2];
    C.m[1][3] = m[1][0] * mat.m[0][3] + m[1][1] * mat.m[1][3] + m[1][2] * mat.m[2][3] + m[1][3] * mat.m[3][3];
    C.m[2][0] = m[2][0] * mat.m[0][0] + m[2][1] * mat.m[1][0] + m[2][2] * mat.m[2][0] + m[2][3] * mat.m[3][0];
    C.m[2][1] = m[2][0] * mat.m[0][1] + m[2][1] * mat.m[1][1] + m[2][2] * mat.m[2][1] + m[2][3] * mat.m[3][1];
    C.m[2][2] = m[2][0] * mat.m[0][2] + m[2][1] * mat.m[1][2] + m[2][2] * mat.m[2][2] + m[2][3] * mat.m[3][2];
    C.m[2][3] = m[2][0] * mat.m[0][3] + m[2][1] * mat.m[1][3] + m[2][2] * mat.m[2][3] + m[2][3] * mat.m[3][3];
    C.m[3][0] = m[3][0] * mat.m[0][0] + m[3][1] * mat.m[1][0] + m[3][2] * mat.m[2][0] + m[3][3] * mat.m[3][0];
    C.m[3][1] = m[3][0] * mat.m[0][1] + m[3][1] * mat.m[1][1] + m[3][2] * mat.m[2][1] + m[3][3] * mat.m[3][1];
    C.m[3][2] = m[3][0] * mat.m[0][2] + m[3][1] * mat.m[1][2] + m[3][2] * mat.m[2][2] + m[3][3] * mat.m[3][2];
    C.m[3][3] = m[3][0] * mat.m[0][3] + m[3][1] * mat.m[1][3] + m[3][2] * mat.m[2][3] + m[3][3] * mat.m[3][3];
    
    return C;
}
