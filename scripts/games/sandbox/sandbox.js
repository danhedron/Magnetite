Game.name = 'Sandbox';

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
}

Game.onSpawn = function( p )
{
	this.player = p;
	this.player.setPosition( { x: 10, y: 130, z: 10 } );
	this.player.moveDir = { x: 0, y: 0, z: 0 };
}

Game.keyDown = function(k)
{
	switch( k )
	{
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
	var rs = world.fireRay( player.getEyeCast() );
	console.log( JSON.stringify( rs.block)  );
}

Game.think = function( dt )
{
	if( this.player.moveDir.x != 0 || this.player.moveDir.y != 0 || this.player.moveDir.z != 0 )
	{
		this.player.move( this.player.moveDir.z * dt,  this.player.moveDir.y * dt, this.player.moveDir.x * dt );
	}
}