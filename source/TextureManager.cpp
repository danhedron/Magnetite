#include "TextureManager.h"

TextureManager::TextureManager( void )
: mBlockWidth( 0 ),
mBlockHeight( 0 )
{

}

TextureManager::~TextureManager( void )
{

}

void TextureManager::initalize()
{
	if(loadTexture("../resources/sprites/world.png")) {
		GLtexture* texture = fetchTexture("../resources/sprites/world.png");
		mBlockWidth = texture->width;
		mBlockHeight = texture->height;
	}
	loadTexture("../resources/sprites/vistest.png");
}

bool TextureManager::loadTexture(std::string textureName)
{
	TextureList::iterator lb = mTextures.lower_bound( textureName );

	if(lb != mTextures.end() && !(mTextures.key_comp()(textureName, lb->first)))
	{
		// Do Nothing
	}
	else
	{
		// the key does not exist in the map
		// add it to the map
		GLtexture lTexture;
		{
			GLuint lTextId = 0;
			sf::Image image;
			if(!image.LoadFromFile(textureName))
				return false;
			glGenTextures(1, &lTextId);
			lTexture.width = image.GetWidth();
			lTexture.height = image.GetWidth();
			lTexture.glID = lTextId;
			glBindTexture(GL_TEXTURE_2D, lTextId);
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image.GetWidth(), image.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, image.GetPixelsPtr());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		mTextures.insert(lb, TextureList::value_type(textureName, lTexture));
	}
	return true;
}

GLtexture* TextureManager::fetchTexture(std::string textureName)
{
	TextureList::iterator lb = mTextures.find(textureName);
	if(lb != mTextures.end())
		return &lb->second;
	return NULL;
}

GLuvrect TextureManager::getBlockUVs(int x, int y)
{
	GLuvrect rect;
	rect.x = (((float)x)*16.f)/mBlockWidth;
	rect.y = (((float)y)*16.f)/mBlockHeight;
	rect.w = 16.f/mBlockWidth;
	rect.h = 16.f/mBlockHeight;
	return rect;
}