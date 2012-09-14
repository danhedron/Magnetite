(new(function() {
	this.structures = [];
	
	this.create = function( type, position )
	{
		var template = require('./scripts/games/fortress/structures/' + type + '.js');
		var blocks = template.blocks;
		
		var xb;
		var yb;
		var zb;
		yb = blocks;
		for( var y = 0; y < yb.length; y++ ) {
			xb = yb[y];
			for( var x = 0; x < xb.length; x++ ) {
				zb = xb[x];
				for( var z = 0; z < zb.length; z++ ) {
					if( zb[z] != '' || zb[z] != undefined )
					{
						world.createBlock( zb[z], position.x + x, position.y + y, position.z + z );
					}
				}
			}
		}
	}
})());