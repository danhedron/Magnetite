#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "prerequisites.h"
#include "Matrix.h"
#include "Frustum.h"
#include "paging/PagingCamera.h"

class World;
class Camera : public Magnetite::PagingCamera
{
protected:
	float mYaw;
	float mPitch;
	Vector3 mPosition;

	Frustum mViewFrustum;
public:
	Camera( World* world );
	~Camera(void);

	glm::mat4 getMatrix();
	glm::mat3 getOrientationMatrix();
	Frustum& getFrustum();

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
	void applyModelViewMatrix( bool rot = true, bool pos = true );
};

#endif
