#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_
#include "prerequisites.h"

struct GLuvrect {
	float x,y,w,h;
};

struct GLtexture {
	size_t width;
	size_t height;
	GLuint glID;
};

typedef std::map<std::string,GLtexture> TextureList;


class TextureManager
{
protected:
	TextureList	mTextures;
	float mBlockWidth;
	float mBlockHeight;
public:
	TextureManager(void);
	~TextureManager();

	void initalize();

	bool loadTexture(std::string textureName);
	GLtexture* fetchTexture(std::string textureName);

	GLuvrect getBlockUVs( int x, int y );
};

#endif
