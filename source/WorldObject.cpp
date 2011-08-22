#include "WorldObject.h"

WorldObject::WorldObject()
{

}

WorldObject::~WorldObject()
{

}

Vector3 WorldObject::getPosition()
{
	return mPosition;
}

void WorldObject::setPosition( const Vector3 &v )
{
	mPosition = v;
}

void WorldObject::update( float dt )
{

}