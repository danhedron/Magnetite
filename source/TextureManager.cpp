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
	if(loadTexture("./resources/sprites/world.png")) {
		GLtexture* texture = fetchTexture("./resources/sprites/world.png");
		mBlockWidth = texture->width;
		mBlockHeight = texture->height;
	}
	loadTexture("./resources/sprites/vistest.png");
}

bool TextureManager::loadTexture(std::string textureName)
{
	TextureList::iterator lb = mTextures.lower_bound( textureName );
	Util::log("Loading Texture:" + textureName);
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
			if(!image.loadFromFile(textureName))
				return false;
			glGenTextures(1, &lTextId);
			lTexture.width = image.getSize().x;
			lTexture.height = image.getSize().y;
			lTexture.glID = lTextId;
			glBindTexture(GL_TEXTURE_2D, lTextId);
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image.getSize().x, image.getSize().y, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
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
	/*rect.x = (((float)x)*16.f)/mBlockWidth;
	rect.y = (((float)y)*16.f)/mBlockHeight;
	rect.w = 16.f/mBlockWidth;
	rect.h = 16.f/mBlockHeight;*/
	rect.x = x;
	rect.y = y;
	rect.w = 1;
	rect.h = 1;
	return rect;
}