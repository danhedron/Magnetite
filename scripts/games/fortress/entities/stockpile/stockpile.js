// Entity for controlling Stockpile behaviour.
(new (function() {
	this.create = function() {
		var dimensions = { x: 6, y: 3, z: 6 };
		
		// Get the entity and define some usefull methods.
		var entity = this;
		
		var position = entity.position;
		
		// Returns true if there is space left in the stockpile.
		entity.isAvailable = function() {
			var b;
			for( var x = 0; x < dimensions.x; x++ )
			{
				for( var z = 0; z < dimensions.z; z++ )
				{
					for( var y = 0; y < dimensions.y; y++ )
					{
						b = world.getBlock( position.x + x, position.y + y, position.z + z );
						if( b == undefined ) return true;
					}
				}
			}
		}
		
		entity.canStore = function( type )
		{
			if( !this.isAvailable() ) return false;
			
			return true;
		}
		
		entity.store = function( type )
		{
			var b 
			for( var y = 0; y < dimensions.y; y++ )
			{
				for( var x = 0; x < dimensions.x; x++ )
				{
					for( var z = 0; z < dimensions.z; z++ )
					{
						b = world.getBlock( position.x + x, position.y + y, position.z + z );
						if( b == undefined ) 
						{
							world.createBlock( type, position.x + x, position.y + y, position.z + z );
							return true;
						}
					}
				}
			}
			return false;
		}
		
		// Hack, shouldn't access Game outside of main file.
		Game.stockpiles.push(entity);
	}
})());