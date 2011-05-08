#ifndef _MATRIX_H_
#define _MATRIX_H_
#include "prerequisites.h"


class Matrix4 {
public:
	float matrix[16];
	float* m00;
	float* m01;
	float* m02;
	float* m03;
	float* m10;
	float* m11;
	float* m12;
	float* m13;
	float* m20;
	float* m21;
	float* m22;
	float* m23;
	float* m30;
	float* m31;
	float* m32;
	float* m33;

	Matrix4();
	~Matrix4();

	void translate( Vector3& v );

	Vector3 operator * ( Vector3& vec );
	Matrix4 operator * ( Matrix4& mat );

	Vector3 getTranslation();

	std::string toString();

	void reset();

	static Matrix4 rotateX( float angle );
	static Matrix4 rotateY( float angle );
	static Matrix4 rotateZ( float angle );

};

#endif
