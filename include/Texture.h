#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#include "BaseResource.h"

class Texture : public BaseResource
{
	size_t mWidth;
	
	size_t mHeight;
	
	GLuint mName;
	
	GLint mFilter;
public:
	Texture(const std::string& file);
	~Texture();
	
	virtual void load();
	
	virtual void unload();
	
	virtual bool isLoaded() const;
	
	GLuint getName();
	
	size_t getWidth();
	
	size_t getHeight();
	
	void setFilter( const GLint filter );
};

#endif