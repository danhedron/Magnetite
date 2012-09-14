(new(function() {
	this.structures = [];
	
	this.create = function( type, position )
	{
		var template = require('./scripts/games/fortress/structures/' + type + '.js');
		var blocks = template.blocks;
		
		var yb;
		var zb;
		for( var x = 0; x < blocks.length; x++ ) {
			yb = blocks[x];
			for( var y = 0; y < yb.length; y++ ) {
				zb = yb[y];
				for( var z = 0; z < zb.length; z++ ) {
					world.createBlock( zb[z], position.x + x, position.y + y, position.z + z );
				}
			}
		}
	}
})());