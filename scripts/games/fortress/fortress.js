Game.name = 'Fortress';

Number.prototype.sign = function() {
  if(this > 0) {
    return 1;
  } else if (this < 0) {
    return -1;
  } else {
    return 0;
  }
}

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
			var ex = new Explosion(); 
			var rs = world.fireRay( pl.getEyeCast() );
			ex.origin = rs.worldHit;
			ex.power = 10;
			ex.radius = 5;
			ex.explode();
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
	this.player.moveTarget = { x: 0, y: 0, z: 0 };
	this.player.moveSpeed = { x: 0, y: 0, z: 0 };
	this.player.movementSpeed = 1;
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
			this.player.moveTarget.z = -10;
			break;
		case 0:
			// A
			this.player.moveTarget.x = 10;
			break;
		case 18:
			// S
			this.player.moveTarget.z = 10;
			break;
		case 3:
			// D
			this.player.moveTarget.x = -10;
			break;
		case 16:
			this.player.moveTarget.y = 10;
			break;
		case 4:
			this.player.moveTarget.y = -10;
			break;
	}
}

Game.keyUp = function(k)
{
	switch( k )
	{
		case 22:
			// W
			this.player.moveTarget.z = 0;
			break;
		case 0:
			// A
			this.player.moveTarget.x = -0;
			break;
		case 18:
			// S
			this.player.moveTarget.z = -0;
			break;
		case 3:
			// D
			this.player.moveTarget.x = 0;
			break;
		case 16:
		case 4:
			this.player.moveTarget.y = 0;
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
}

Game.think = function( dt )
{
	var accel = 5.0;
	var decel = 10.0;
	for( var d in this.player.moveTarget )
	{
		if( Math.abs(this.player.moveTarget[d]) - Math.abs(this.player.moveSpeed[d]) > 0.0001 )
		{
			var s = this.player.moveTarget[d].sign();
			this.player.moveSpeed[d] = s * Math.min( Math.abs(this.player.moveTarget[d]), Math.abs(this.player.moveSpeed[d]) + accel * dt);
		}
		else if( Math.abs(this.player.moveSpeed[d]) - Math.abs(this.player.moveTarget[d]) > 0.0001 )
		{
			var s = this.player.moveSpeed[d].sign();
			this.player.moveSpeed[d] = s * Math.max( Math.abs(this.player.moveTarget[d]), Math.abs(this.player.moveSpeed[d]) - decel * dt);
		}
	}
	if( this.player.moveSpeed.x != 0 || this.player.moveSpeed.y != 0 || this.player.moveSpeed.z != 0 )
	{
		this.player.move( this.player.moveSpeed.z * this.player.movementSpeed * dt, this.player.moveSpeed.y * this.player.movementSpeed * dt, this.player.moveSpeed.x * this.player.movementSpeed * dt );
	}
}

Game.draw = function()
{
	console.drawText( 10, 10, this.tools[ this.toolIndex % this.tools.length ].title );
}