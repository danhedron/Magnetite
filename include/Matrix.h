#ifndef _MATRIX_H_
#define _MATRIX_H_
#include "prerequisites.h"
#include "Vector.h"


class Matrix4 {
public:
	union {
		float matrix[16];
		float m[4][4];
	};

	Matrix4();
	~Matrix4();

	void translate( Vector3& v );

	Vector3 operator * ( const Vector3& vec );
	Matrix4 operator * ( const Matrix4& mat );

	Vector3 getTranslation();

	std::string toString();

	void reset();
	void zero();

	static Matrix4 rotateX( float angle );
	static Matrix4 rotateY( float angle );
	static Matrix4 rotateZ( float angle );

};

#endif
