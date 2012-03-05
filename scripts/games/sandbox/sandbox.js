Game.name = 'Sandbox';

Game.tools = [
	{
		title: 'place',
		func: function( pl ) {
			var rs = world.fireRay( pl.getEyeCast() );
			world.createBlock( meta.blocks.availableTypes[this.currentBlock], rs.worldHit.x + (rs.normal.x /2),  rs.worldHit.y + (rs.normal.y /2),  rs.worldHit.z + (rs.normal.z /2) );
		}
	},
	{
		title: 'remove',
		func: function( pl ) {
			var rs = world.fireRay( pl.getEyeCast() );
			world.removeBlock( rs.worldHit.x - (rs.normal.x /2),  rs.worldHit.y - (rs.normal.y /2),  rs.worldHit.z - (rs.normal.z /2) );
		}
	},
	{
		title: 'explode',
		func: function( pl ) {
		
		}
	}
	];
Game.toolIndex = 0;

Game.onLoad = function()
{
	var ray = new Ray();
	ray.origin = { x: 10, y: 200, z : 10 };
	ray.direction = { x: 0, y: -1, z: 0 };
	var result = world.fireRay( ray );
	if( result.block )
	{
		console.log( 'type: ' + result.block.type );
	}
	this.currentBlock = 0;
}

Game.onSpawn = function( p )
{
	this.player = p;
	this.player.setPosition( { x: 10, y: 130, z: 10 } );
	this.player.moveDir = { x: 0, y: 0, z: 0 };
}

Game.changeBlock = function( v )
{
	this.currentBlock += v;
	if( this.currentBlock < 0 )
		this.currentBlock = meta.blocks.availableTypes.length - 1;
	else if( this.currentBlock > meta.blocks.availableTypes.length - 1 )
	{
		this.currentBlock = 0;
	}
	console.log( meta.blocks.availableTypes[this.currentBlock] );
}

Game.keyDown = function(k)
{
	switch( k )
	{
		case 68:
			this.changeBlock(-1);
			break;
		case 67:
			this.changeBlock( 1);
			break;
		case 22:
			// W
			this.player.moveDir.z = -10;
			break;
		case 0:
			// A
			this.player.moveDir.x = 10;
			break;
		case 18:
			// S
			this.player.moveDir.z = 10;
			break;
		case 3:
			// D
			this.player.moveDir.x = -10;
			break;
		case 16:
			this.player.moveDir.y = 10;
			break;
		case 4:
			this.player.moveDir.y = -10;
			break;
	}
}

Game.keyUp = function(k)
{
	switch( k )
	{
		case 22:
			// W
			this.player.moveDir.z = 0;
			break;
		case 0:
			// A
			this.player.moveDir.x = -0;
			break;
		case 18:
			// S
			this.player.moveDir.z = -0;
			break;
		case 3:
			// D
			this.player.moveDir.x = 0;
			break;
		case 16:
		case 4:
			this.player.moveDir.y = 0;
			break;
	}
}

Game.onPrimary = function( player )
{
	this.tools[ this.toolIndex % this.tools.length ].func.call( this, player );
}

Game.onAlt = function( player )
{
	this.toolIndex++;
	console.log( this.tools[ this.toolIndex % this.tools.length ].title );
}

Game.think = function( dt )
{
	if( this.player.moveDir.x != 0 || this.player.moveDir.y != 0 || this.player.moveDir.z != 0 )
	{
		this.player.move( this.player.moveDir.z * dt,  this.player.moveDir.y * dt, this.player.moveDir.x * dt );
	}
}

Game.draw = function()
{
}