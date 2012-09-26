(new(function() {
	this.structures = [];
	
	// a structure 'request', to make things a bit simpler.
	function StructureRequest( template, position, flipxz, length ) {
		this.position = position;
		this.create = function( )
		{
			var blocks = template.blocks;
		
			var yoff = template.yoffset ? template.yoffset : 0;
			var xoff = template.xoffset ? template.xoffset : 0;
			var zoff = template.zoffset ? template.zoffset : 0;
			
			var xb, yb, zb;
			var wx, wy, wz;
			
			yb = blocks;
			for( var y = 0; y < yb.length; y++ ) {
				xb = yb[y];
				for( var x = 0; x < (template.repeat ? length : xb.length); x++ ) {
					zb = xb[(x % xb.length)];
					for( var z = 0; z < zb.length; z++ ) {
						
						wy = position.y + y + yoff;
						wx = position.x;
						wz = position.z;
						if( flipxz ) {
							wx += z + zoff;
							wz += x + xoff;
						}
						else {
							wx += x + xoff;
							wz += z + zoff;
						}
						
						if( zb[z] != '' || zb[z] != undefined )
						{
							world.createBlock( zb[z], wx, wy, wz );
						}
						if( zb[z] == '' ) 
						{
							world.removeBlock( wx, wy, wz );
						}
					}
				}
			}
			
			var entities = template.entities;
			if( entities ) {
				for( var i = 0; i < entities.length; i++ )
				{
					var e = entities[i];
					var ent = world.createEntity( e.type );
					ent.position = add( add( e.position, position ), { x: 0, y: template.yoffset, z: 0 } );
					ent.create();
					ent.setPosition( add( add( e.position, position ), { x: 0, y: template.yoffset, z: 0 } ) );
				}
			}
		}
	}
	
	this.create = function( type, position  )
	{
		var template = require('./scripts/games/fortress/structures/' + type + '.js');
		
		if( template.repeat && this.startPos == undefined )
		{
			this.startPos = position;
			return undefined;
		}
		else if( template.repeat )
		{
			var min = minv( this.startPos, position );
			var max = maxv( this.startPos, position );
			var lx = max.x - min.x;
			var lz = max.z - min.z;
			this.startPos = undefined;
			if( lx > lz )
				return new StructureRequest( template, min, false, parseInt(Math.ceil(lx)) );
			else
				return new StructureRequest( template, min, true, parseInt(Math.ceil(lz)) );
		}
		
		return new StructureRequest( template, position );
	}
})());