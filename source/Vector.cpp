#include "Vector.h"

Vector3::Vector3( float _x, float _y, float _z, float _w )
{
	x = _x; y = _y; z = _z; w = _w;
}

Vector3 Vector3::operator -() {
	return Vector3(-x,-y,-z);
}

Vector3 Vector3::operator -(const Vector3& right) {
	return Vector3(x - right.x, y - right.y, z - right.z);
}

Vector3 Vector3::operator +(const Vector3& right) {
	return Vector3(x + right.x, y + right.y, z + right.z);
}



Vector3 Vector3::operator *( float scal ) {
	return Vector3(x*scal,y*scal,z*scal);
}

Vector3 Vector3::operator /( float scal ) {
	return Vector3(x/scal,y/scal,z/scal);
}
 
float Vector3::operator[] ( int index ) {
	if( index == 0 )
		return x;
	if( index == 1 )
		return y;
	if( index == 2 )
		return z;
	if( index == 3 )
		return w;
	return 0;
}

float Vector3::length()
{
	return (x*x)+(y*y)+(z*z);
}

Vector3 Vector3::normalize()
{
	return *this/length();
}