#include "ProgramResource.h"
#include "MagnetiteCore.h"
#include "ResourceManager.h"
#include "ShaderResource.h"
#include <fstream>

ProgramResource::ProgramResource( std::string file )
	: BaseResource( file ),
	mName( 0 ),
	mVertexShader( NULL ),
	mGeometryShader( NULL ),
	mFragmentShader( NULL )
{

}

ProgramResource::~ProgramResource() {
	if( mVertexShader ) {
		delete mVertexShader;
	}
	if( mFragmentShader ) {
		delete mFragmentShader;
	}
	if( mGeometryShader ) {
		delete mGeometryShader;
	}
	
	if( mName != 0 ) {
		glDeleteProgram( mName );
	}
}

void ProgramResource::setVertexShader( ShaderResource* shader )
{
	mVertexShader = shader;
}

void ProgramResource::setGeometryShader( ShaderResource* shader )
{
	mGeometryShader = shader;
}

void ProgramResource::setFragmentShader( ShaderResource* shader )
{
	mFragmentShader = shader;
}

void ProgramResource::link( )
{
	if( mName != 0 ) {
		Util::log("Program Already Linked");
		return;
	}
	else {
		load();
	}

	mName = glCreateProgram();

	if( mVertexShader != NULL ) {
		if( mVertexShader->getName() == 0 ) {
			mVertexShader->compile( GL_VERTEX_SHADER );
		}
		if( mVertexShader->getName() != 0 ) {
			glAttachShader( mName, mVertexShader->getName() );
		}	
	}

/*	if( mGeometryShader != NULL ) {
		if( mGeometryShader->getName() == 0 ) {
			mGeometryShader->compile( GL_GEOMETRY_SHADER );
		}
		if( mGeometryShader->getName() != 0 ) {
			glAttachShader( mName, mGeometryShader->getName() );
		}	
	}*/

	if( mFragmentShader != NULL ) {
		if( mFragmentShader->getName() == 0 ) {
			mFragmentShader->compile( GL_FRAGMENT_SHADER );
		}
		if( mFragmentShader->getName() != 0 ) {
			glAttachShader( mName, mFragmentShader->getName() );
		}	
	}

	glLinkProgram( mName );

	GLint linked;
	glGetProgramiv( mName, GL_LINK_STATUS, &linked );
	if(linked) {
		Util::log("Program Linked Ok");
	}
	else {
		Util::log("Error Linking Program");
	}

	GLint blen = 0;
	GLsizei slen = 0;
	glGetProgramiv( mName, GL_INFO_LOG_LENGTH, &blen );
	if( blen > 1 ) {
		GLchar* compile_log = (GLchar*)malloc(blen);
		glGetInfoLogARB( mName, blen, &slen, compile_log );
		Util::log("\t" + std::string(compile_log));
		free(compile_log);
	}
}

void ProgramResource::load()
{
	std::ifstream file( mFilename.c_str() );
	if( !file ) return;

	std::string token;
	char c;
	ShaderResource** sh = NULL;
	while( !file.eof() ) {
		file.get(c);
		if(c == '#') {
			std::string comment;
			std::getline(file,comment);
		}
		else if(c == ':') {
			// Digest the token
			if( token == "vertex" ) {
				sh = &mVertexShader;
			} else if( token == "geometry" ) {
				sh = &mGeometryShader;
			} else if( token == "fragment" ) {
				sh = &mFragmentShader;
			}
			token = "";
		}
		else if( c == '\n' && sh != NULL ) {
			ShaderResource* shader = MagnetiteCore::Singleton->getResourceManager()->getResource<ShaderResource>(token);
			*sh = shader;
			token = "";
		}
		else {
			token += c;
		}

	}

	file.close();
}

bool ProgramResource::isLoaded() const
{
	return mName != 0;
}

GLuint ProgramResource::getName()
{
	return mName;
}

GLint ProgramResource::getAttributeIndex( const std::string& attribute )
{
	if( mName > 0 ) 
	{
		std::map<std::string, int>::iterator it = mAttributes.find(attribute);
		if( it != mAttributes.end() )
		{
			return it->second;
		}
		GLint p = glGetAttribLocation( mName, attribute.c_str() );
		mAttributes[attribute] = p;
		return p;
	}
	return 0;
}

GLint ProgramResource::getUniformLocation( const std::string& uniform )
{
	if( mName > 0 ) 
	{
		std::map<std::string, int>::iterator it = mUniforms.find(uniform);
		if( it != mUniforms.end() )
		{
			return it->second;
		}
		GLint p = glGetUniformLocation( mName, uniform.c_str() );
		mUniforms[uniform] = p;
		return p;
	}
	return 0;
}

void ProgramResource::makeActive()
{
	glUseProgram( mName );
}

void ProgramResource::deactivate()
{
	disableVertexAttributes();
	glUseProgram(0);
}

void ProgramResource::setVertexAttribute( const std::string& name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid*  pointer )
{
	auto attributeIndex = getAttributeIndex(name);
	glEnableVertexAttribArray(attributeIndex);
	mEnabledArrays.push_back(attributeIndex);
	glVertexAttribPointer( attributeIndex, size, type, normalized, stride, pointer );

}

void ProgramResource::disableVertexAttributes()
{
	for(size_t x = 0; x < mEnabledArrays.size(); x++ )
	{
		glDisableVertexAttribArray(mEnabledArrays[x]);
	}
	mEnabledArrays.clear();	
}