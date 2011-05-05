#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "prerequisites.h"

class Renderer
{
protected:
	float totalTime;
public:
	Renderer(void);
	~Renderer(void);

	void initialize();
	void resizeViewport( size_t x, size_t y, size_t w, size_t h );

	void render( double dt );
};

#endif