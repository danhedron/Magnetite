#include "Renderer.h"

Renderer::Renderer(void)
: totalTime( 0 )
{
}

Renderer::~Renderer(void)
{
}

void Renderer::initialize()
{
	// Set color and depth clear value
	glClearDepth(1.f);
	glClearColor(0.f, 0.f, 0.f, 0.f);

	// Enable Z-buffer read and write
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// Setup a perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.f, 1.f, 1.f, 500.f);
}

void Renderer::resizeViewport(size_t x, size_t y, size_t w, size_t h)
{
	glViewport(x, y, w, h);
}

void Renderer::render(double dt)
{
	totalTime += dt;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.f, 0.f, -200.f);
	glRotatef(totalTime * 50, 1.f, 0.f, 0.f);
	glRotatef(totalTime * 30, 0.f, 1.f, 0.f);
	glRotatef(totalTime * 90, 0.f, 0.f, 1.f);

	glBegin(GL_QUADS);

		glVertex3f(-50.f, -50.f, -50.f);
		glVertex3f(-50.f,  50.f, -50.f);
		glVertex3f( 50.f,  50.f, -50.f);
		glVertex3f( 50.f, -50.f, -50.f);

		glVertex3f(-50.f, -50.f, 50.f);
		glVertex3f(-50.f,  50.f, 50.f);
		glVertex3f( 50.f,  50.f, 50.f);
		glVertex3f( 50.f, -50.f, 50.f);

		glVertex3f(-50.f, -50.f, -50.f);
		glVertex3f(-50.f,  50.f, -50.f);
		glVertex3f(-50.f,  50.f,  50.f);
		glVertex3f(-50.f, -50.f,  50.f);

		glVertex3f(50.f, -50.f, -50.f);
		glVertex3f(50.f,  50.f, -50.f);
		glVertex3f(50.f,  50.f,  50.f);
		glVertex3f(50.f, -50.f,  50.f);

		glVertex3f(-50.f, -50.f,  50.f);
		glVertex3f(-50.f, -50.f, -50.f);
		glVertex3f( 50.f, -50.f, -50.f);
		glVertex3f( 50.f, -50.f,  50.f);

		glVertex3f(-50.f, 50.f,  50.f);
		glVertex3f(-50.f, 50.f, -50.f);
		glVertex3f( 50.f, 50.f, -50.f);
		glVertex3f( 50.f, 50.f,  50.f);

	glEnd();
}