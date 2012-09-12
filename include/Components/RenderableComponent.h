#ifndef _RENDERABLECOMPONENT_H_
#define _RENDERABLECOMPONENT_H_
#include <Component.h>

class ProgramResource;
class ModelResource;

namespace Magnetite
{
	
	class RenderableComponent : public Component
	{
	protected:
		
		ModelResource* mModel;
		
		ProgramResource* mProgram;
		
		Vector3 mTranslation;
		
	public:
		
		RenderableComponent(BaseEntity* ent);
		
		void setModel( ModelResource* model );
		
		void setProgram( ProgramResource* prog );
		
		void setPosition( const Vector3& p );
		
		virtual void event( const BaseEvent& ev );
		
		virtual void draw( const DrawInfo& info, float dt );
		
		virtual Magnetite::String getType() { return "renderable"; }
	};
	
};

#endif
