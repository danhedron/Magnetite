#include "Texture.h"

Texture::Texture( const std::string& file )
: BaseResource(file),
mWidth(0),
mHeight(0),
mName(0),
mFilter(GL_NEAREST)
{
	
}

Texture::~Texture()
{
	
}

void Texture::load()
{
	sf::Image image;
	if(!image.loadFromFile(mFilename)) return;
	
	glGenTextures(1, &mName);
	mWidth = image.getSize().x;
	mHeight = image.getSize().y;
	glBindTexture(GL_TEXTURE_2D, mName);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image.getSize().x, image.getSize().y, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mFilter);
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::unload()
{
	
}

GLuint Texture::getName()
{
	return mName;
}

size_t Texture::getWidth()
{
	return mWidth;
}

size_t Texture::getHeight()
{
	return mHeight;
}

void Texture::setFilter( const GLint filter )
{
	mFilter = filter;
}