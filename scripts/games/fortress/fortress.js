var Events = require('./scripts/events.js');
var Menu = require('./scripts/menu.js');

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

Game.onLoad = function()
{
	this.constructionMenu = new Menu({
		items: [
			{
				text:'Buildings'
			},
			{
				text:'Defences'
			},
			{
				text:'Structures'
			},
			{
				text:'Machinery'
			}
		]
	});
	this.constructionMenu.breadcrumbs = ["Menu", "Construction"];
	this.constructionMenu.position = [20, 30];
}

Game.onSpawn = function( p )
{
	this.player = p;
	this.player.setPosition( { x: 10, y: 130, z: 10 } );
	this.player.moveTarget = { x: 0, y: 0, z: 0 };
	this.player.moveSpeed = { x: 0, y: 0, z: 0 };
	this.player.turnAngle = 0;
	this.player.turnSpeed = 0;
	this.player.lookAngle = -45;
	this.player.movementSpeed = 1;
	
	this.player.enableFlying(true);
}

Game.keyDown = function(k)
{
	switch( k )
	{
		case Events.Keys.Q:
			this.player.turnSpeed += 40;
			break;
		case Events.Keys.E:
			this.player.turnSpeed -= 40;
			break;
		case 22:
			// W
			this.player.moveTarget.x = -10;
			break;
		case 0:
			// A
			this.player.moveTarget.z = -10;
			break;
		case 18:
			// S
			this.player.moveTarget.x = 10;
			break;
		case 3:
			// D
			this.player.moveTarget.z = 10;
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
		case Events.Keys.Q:
		case Events.Keys.E:
			this.player.turnSpeed = 0;
			break;
		case Events.Keys.W:
		case Events.Keys.S:
			this.player.moveTarget.x = 0;
			break;
		case Events.Keys.D:
		case Events.Keys.A:
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

Game.mouseMoved = function( x, y )
{
	return true;
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
	
	this.player.setYaw( this.player.turnAngle += (this.player.turnSpeed * dt) );
	this.player.setPitch( this.player.lookAngle );
}

Game.draw = function()
{
	this.constructionMenu.draw();
}