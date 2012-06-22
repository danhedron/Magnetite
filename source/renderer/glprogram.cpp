#include "glprogram.h"
#include "glshader.h"

GLprogram::GLprogram()
: mName(0),
mVertex(NULL),
mFragment(NULL),
mGeometry(NULL),
mValid(false)
{
}

void GLprogram::link()
{
	if( mName != 0 ) {
		Util::log("Program already linked");
		return;
	}
	if( mVertex == NULL ) {
		Util::log("Vertex Shader missing");
		return;
	}
	if( mFragment == NULL ) {
		Util::log("Fragment Shader missing");
		return;
	}
	
	if( !mVertex->isValid() )
	{
		// Init vertex shader.
		mVertex->create();
		if( !mVertex->isValid() ) {
			return;
		}
	}
	if( !mFragment->isValid() )
	{
		// Init fragment shader.
		mFragment->create();
		if( !mFragment->isValid() ) {
			return;
		}
	}

	mName = glCreateProgram();

	glAttachShader( mName, mVertex->getName() );
	glAttachShader( mName, mFragment->getName() );


	// deal with attributes n stuff
	for( std::map<int, std::string>::iterator it = mAttributes.begin(); it != mAttributes.end(); it++ )
	{
		glBindAttribLocation( mName, it->first, it->second.c_str());
	}

	glLinkProgram( mName );

	GLint linked;
	glGetProgramiv(mName, GL_LINK_STATUS, &linked);
	if (linked)
	{
		Util::log("Program Linked OK");
		mValid = true;
	}
	else
	{
		Util::log("Error Linking Program:");
		mValid = false;
	}

	GLint blen = 0;	
	GLsizei slen = 0;

	glGetProgramiv(mName, GL_INFO_LOG_LENGTH , &blen);       
	if (blen > 1)
	{
		GLchar* compiler_log = (GLchar*)malloc(blen);
		glGetInfoLogARB(mName, blen, &slen, compiler_log);
		Util::log("\t" + std::string(compiler_log));
		free (compiler_log);
	}
}

void GLprogram::bindUniformTexture( std::string var, GLint unit )
{
	//if( mUniforms.find( var ) != mUniforms.end() ) return;
	const char* v = var.c_str();
	GLuint loc = glGetUniformLocation( mName, v );
	glUseProgram( mName );
	glUniform1i( loc, unit );
	mUniforms[var] = loc;
}

void GLprogram::bindAttribute( int index, std::string attribute )
{
	mAttributes[index] = attribute;
}
