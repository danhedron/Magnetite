(new(function() {
	this.structures = [];
	
	this.create = function( type, position, flipxz, length )
	{
		if( flipxz == undefined ) { 
			flipxz = false;
		}
		
		var template = require('./scripts/games/fortress/structures/' + type + '.js');
		var blocks = template.blocks;
		var yoff = template.yoffset ? template.yoffset : 0;
		
		if( template.repeat && this.startPos == undefined )
		{
			this.startPos = position;
			return;
		}
		else if( template.repeat && length == undefined )
		{
			var min = minv( this.startPos, position );
			var max = maxv( this.startPos, position );
			var lx = max.x - min.x;
			var lz = max.z - min.z
			if( lx > lz )
				this.create( type, min, false, parseInt(lx) );
			else
				this.create( type, min, true, parseInt(lz) );
			this.startPos = undefined;
			return;
		}
		
		var xb, yb, zb;
		var wx, wy, wz;
		
		yb = blocks;
		for( var y = 0; y < yb.length; y++ ) {
			xb = yb[y];
			for( var x = 0; x < (template.repeat ? length : xb.length); x++ ) {
				zb = xb[(x % xb.length)];
				for( var z = 0; z < zb.length; z++ ) {
					
					wy = position.y + y + yoff;
					if( flipxz ) {
						wx = position.x + z;
						wz = position.z + x;
					}
					else {
						wx = position.x + x;
						wz = position.z + z;
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
})());