#include "glshader.h"
#include <util.h>

GLshader::GLshader( const std::string& src, const std::string& filename )
: mName(0),
mType(0),
mSource(src),
mFilename(filename)
{
	
}

GLshader::~GLshader()
{
	if( isValid() )
	{
		glDeleteShader( mName );
	}
}

void GLshader::create()
{
	if( mName != 0 || mSource.length() == 0 )
		return;

	mName = glCreateShader( mType );
	if( mName == 0 )
		return;

	const char* src = mSource.c_str();
	glShaderSource( mName, 1, &src, NULL );
	glCompileShader( mName );
	
	GLint success;
	glGetObjectParameterivARB( mName, GL_COMPILE_STATUS, &success );
	if( success ) {
		Util::log("Shader Compiled OK: " + Util::toString(mName));
	}
	else {
		Util::log("Error compiling shader (" + mFilename + "): ");
	}

	GLint blen = 0;	
	GLsizei slen = 0;

	glGetShaderiv(mName, GL_INFO_LOG_LENGTH , &blen);       
	if (blen > 1)
	{
		GLchar* compiler_log = (GLchar*)malloc(blen);
		glGetInfoLogARB(mName, blen, &slen, compiler_log);
		Util::log("\t" + std::string(compiler_log));
		free (compiler_log);
	}
}

void GLshader::setType(GLenum type)
{
	mType = type;
}

GLuint GLshader::getName()
{
	return mName;
}

bool GLshader::isValid()
{
	return mName > 0;
}