#include "Vector.h"
#include <algorithm>

Vector3::Vector3( float _x, float _y, float _z, float _w )
{
	x = _x; y = _y; z = _z; w = _w;
}

Vector3 Vector3::operator -() const {
	return Vector3(-x,-y,-z);
}

Vector3 Vector3::operator -(const Vector3& right) const {
	return Vector3(x - right.x, y - right.y, z - right.z);
}

Vector3 Vector3::operator +(const Vector3& right) const {
	return Vector3(x + right.x, y + right.y, z + right.z);
}

Vector3 Vector3::operator /(const Vector3& right) const {
	return Vector3(x / right.x, y / right.y, z / right.z);
}

Vector3& Vector3::operator +=(const Vector3& right) {
	this->x += right.x;
	this->y += right.y;
	this->z += right.z;
	return *this;
}

Vector3& Vector3::operator -=(const Vector3& right) {
	this->x -= right.x;
	this->y -= right.y;
	this->z -= right.z;
	return *this;
}

Vector3 Vector3::operator *( float scal ) const {
	return Vector3(x*scal,y*scal,z*scal);
}

Vector3 Vector3::operator *(  const Vector3& rhs ) const {
	Vector3 res;
	res.x = y * rhs.z - z * rhs.y;
	res.y = z * rhs.x - x * rhs.z;
	res.z = x * rhs.y - y * rhs.x;
	return res;
}

Vector3 Vector3::operator /( float scal ) const {
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
	return 0.f;
}

bool Vector3::operator < (const Vector3& right) {
	return (this->x < right.x && this->y < right.y && this->z < right.z);
}

bool Vector3::operator== (const Vector3& rhs ) const 
{
	return x == rhs.x && y == rhs.y && rhs.z == z;
}

float Vector3::dotProduct(const Vector3 &rhs)
{
	return (this->x * rhs.x) + (this->y * rhs.y) + (this->z * rhs.z);
}

float Vector3::length()
{
	return (x*x)+(y*y)+(z*z);
}

Vector3 Vector3::normalize()
{
	return *this/length();
}

float Vector3::biggestDimension()
{
	return std::max( std::max( x, y ), z );
}

float Vector3::smallestDimension()
{
	return std::min( std::min( x, y ), z );
}
