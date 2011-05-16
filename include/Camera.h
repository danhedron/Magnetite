#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "prerequisites.h"
#include "Matrix.h"

class Camera
{
protected:
	float mYaw;
	float mPitch;
	Vector3 mPosition;
public:
	Camera(void);
	~Camera(void);

	Matrix4 getMatrix();

	void setPosition(const Vector3& v);
	Vector3 getPosition();

	void pitch( float amt );
	void yaw( float amt );
	void setPitch( float p );
	void setYaw( float y );

	float getPitch();
	float getYaw();

	Vector3 getForward( );

	void translate(const Vector3& v );

	void applyMatrix( bool rot = true, bool pos = true );
};

#endif
