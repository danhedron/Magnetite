#ifndef _EXPLOSION_H_
#define _EXPLOSION_H_
#include "prerequisites.h"

struct explosion_t 
{
	Vector3 center;
	int power;
	int radius;
};

class Explosion 
{
protected:
	explosion_t mInfo;
public:
	Explosion( explosion_t );
	~Explosion();

	void explode();
};

#endif
