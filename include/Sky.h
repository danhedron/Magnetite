#ifndef _SKY_H_
#define _SKY_H_
#include "prerequisites.h"

#define DAY_LENGTH 600

class Texture;
class ProgramResource;
struct GLtexture;
struct GLgeometry;

class Sky {
protected:
	size_t mTime;
	float mMilliTime;
	GLgeometry* mGeom;
	Texture* mSkyTexture;
	ProgramResource* mSkyProgram;
public:
	Sky( void );
	~Sky();

	void setTime( size_t time );
	size_t getTime();

	void update( float dt );

	float timeUV();

	void renderSky();
};

#endif // _SKY_H_