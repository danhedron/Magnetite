#include "Components/RenderableComponent.h"
#include <ModelResource.h>
#include <ProgramResource.h>
#include <Geometry.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Magnetite 
{
	
	RenderableComponent::RenderableComponent( BaseEntity* ent )
	: Component(ent),
	mModel(nullptr),
	mProgram(nullptr)
	{
	}
	
	void RenderableComponent::setModel( ModelResource* model )
	{
		mModel = model;
	}
	
	void RenderableComponent::setProgram( ProgramResource* prog )
	{
		mProgram = prog;
	}
	
	void RenderableComponent::event( const BaseEvent& ev )
	{
		if( ev.eventID == CE_POSITION_UPDATED )
		{
			MovementEvent* mv = (MovementEvent*)&ev;
			
			mTranslation = mv->position;
		}
	}
	
	void RenderableComponent::draw( const DrawInfo& info, float dt )
	{
		if( mProgram && mModel )
		{
			if( !mModel->isLoaded() )
			{
				mModel->load();
			}
			
			if( !mProgram->isLoaded() )
			{
				mProgram->load();
				mProgram->link();
			}
			
			auto geom = mModel->getGeometry();
			
			geom->bind();
			
			mProgram->makeActive();
			
			geom->bindVertexAttributes(mProgram);
			
			auto mwrld = mProgram->getUniformLocation("matrix_worldspace");
			auto mproj = mProgram->getUniformLocation("matrix_projection");
			auto mview = mProgram->getUniformLocation("matrix_view");

			glm::mat4 world = glm::translate( glm::mat4(), mTranslation );
			
			if( mwrld != -1 ) 
				glUniformMatrix4fv( mwrld, 1, GL_FALSE, glm::value_ptr(world) );

			if( mproj != -1 )
				glUniformMatrix4fv( mproj, 1, GL_FALSE, glm::value_ptr(info.projection) );
			
			if( mview != -1 )
				glUniformMatrix4fv( mview, 1, GL_FALSE, glm::value_ptr(info.view) );
			
			glDrawRangeElements(GL_TRIANGLES, 0, geom->vertexCount, geom->edgeCount, GL_UNSIGNED_SHORT, 0);
			
			mProgram->deactivate();
			
			geom->unbind();
		}
	}
	
};