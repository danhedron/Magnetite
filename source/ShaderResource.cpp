#include "ShaderResource.h"
#include <fstream>

ShaderResource::ShaderResource( std::string file )
: 	BaseResource( file ),
mSource(""),
mName( 0 )

{

}

void ShaderResource::compile( GLenum type )
{
	load();
	mType = type;

	if( mName != 0 ) {
		Util::log("Shader Already Compiled");
		return;
	}
	if( mSource.length() == 0 ) {
		Util::log("Shader empty ["+mFilename+"]");
		return;
	}
	
	const char* source = mSource.c_str();

	mName = glCreateShader( type );

	glShaderSource( mName, 1, &source, NULL );
	glCompileShader( mName );

	GLint success;
	glGetObjectParameterivARB( mName, GL_COMPILE_STATUS, &success );
	if( success ) {
		Util::log("Shader [" + mFilename + "] compiled succesfully");
	}
	else {
		Util::log("Error compiling shader  [" + mFilename + "]");
	}

	GLint blen = 0;
	GLsizei slen = 0;

	glGetShaderiv( mName, GL_INFO_LOG_LENGTH, &blen );
	if( blen > 1 ) {
		GLchar* compiler_log = (GLchar*)malloc(blen);
		glGetInfoLogARB(mName, blen, &slen, compiler_log);
		Util::log("\t" + std::string(compiler_log));
		free(compiler_log);
	}
}

void ShaderResource::load()
{
	std::ifstream file( mFilename.c_str() );
	if( !file ) return;

	std::stringstream ss;
	std::string s;
	while( !file.eof() ) {
		std::getline( file, s );
		ss << s << '\n';
	}

	mSource = ss.str();

	file.close();
}

GLuint ShaderResource::getName()
{
	return mName;
}
