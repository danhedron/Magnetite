var Events = require('./scripts/events.js');
var Menu = require('./scripts/menu.js');
var Drone = require('./scripts/games/fortress/drone.js');
var Structure = require('./scripts/games/fortress/structure.js');

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

// Todo: sort out library functions again.
function subt( v1, v2 ) { return { x: v1.x - v2.x, y: v1.y - v2.y, z: v1.z - v2.z }; }
function add( v1, v2 ) { return { x: v1.x + v2.x, y: v1.y + v2.y, z: v1.z + v2.z }; }
function mul( v1, f ) { return { x: v1.x * f, y: v1.y * f, z: v1.z * f }; }
function lent( v ) {
	return Math.sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}
function norm( v ) {
	var l = lent(v);
	if( l > 0 ) {
		return { x: v.x / l, y: v.y / l, z: v.z / l };
	}
	return v;
}

Game.onLoad = function()
{
	this.menu = new Menu(require('./scripts/games/fortress/menu.js').root);
	this.menu.position = [20, 30];
	
	// Create a drone.
	this.drones = [];
	for( var i = 0; i < 20; i++ )
	{
		var d = this.newDrone();
		d.overridePosition({ x: Math.random() * 100, y: 140, z: Math.random() * 100 });
	}
	
	// Create a test entity
	this.anvil = world.createEntity('anvil');
	this.anvil.create();
}

Game.build = function( b ) 
{
	var rs = world.fireRay( this.player.getEyeCast() );
	
	if( rs.hit )
	{
		var p = { x: rs.worldHit.x + (rs.normal.x /2), y: rs.worldHit.y - (rs.normal.y /2), z: rs.worldHit.z + (rs.normal.z /2) };
		var structure = {
			'structure': b,
			'position': p
		};
		
		// Tell the nearest drone to do something about it.
		this.getNearestDrone( p ).construct(structure);
	}
}

Game.clear = function()
{
	var rs = world.fireRay( this.player.getEyeCast() );
	
	if( rs.hit )
	{
		var p = { x: rs.worldHit.x - (rs.normal.x /2), y: rs.worldHit.y - (rs.normal.y /2), z: rs.worldHit.z - (rs.normal.z /2) };
		
		if( this.clearStart == undefined ) 
		{
			this.clearStart = p;
		}
		else 
		{
			this.clearArea( this.clearStart, p );
			this.clearStart = undefined;
		}
	}
}

Game.clearArea = function( min, max )
{
	// Tell the nearest drone to do something about it.
	this.getNearestDrone( min ).clearArea( min, max );
}

Game.createStructure = function( s ) {
	Structure.create( s.structure, s.position );
}

Game.newDrone = function() {
	var d = new Drone(this);
	this.drones.push(d);
	return d;
};

Game.createDrone = function() {
	var rs = world.fireRay( this.player.getEyeCast() );
	
	if( rs.hit )
	{
		var drone = this.newDrone();
		drone.position = { x: rs.worldHit.x + (rs.normal.x /2), y: rs.worldHit.y - (rs.normal.y /2), z: rs.worldHit.z + (rs.normal.z /2) };
	}
}

Game.getNearestDrone = function( center ) {
	var drone, ndrone;
	var nearest = 100000;
	for( var x = 0; x < this.drones.length; x++ )
	{
		drone = this.drones[x];
		var d = { x: center.x - drone.position.x, y: center.y - drone.position.y, z: center.z - drone.position.z };
		var lenth = lent( d );
		if( lenth < nearest ) {
			nearest = lenth;
			ndrone = drone;
		};
	}
	return ndrone;
}

Game.onSpawn = function( p )
{
	// Hack.
	this.player = p;
	this.player.setPosition( { x: 10, y: 130, z: 10 } );
	this.player.moveTarget = { x: 0, y: 0, z: 0 };
	this.player.moveSpeed = { x: 0, y: 0, z: 0 };
	this.player.turnAngle = 180;
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
		case Events.Keys.X:
			this.player.moveTarget.y = 10;
			break;
		case Events.Keys.Z:
			this.player.moveTarget.y = -10;
			break;
		case Events.Keys.Up: 
			this.menu.move(-1);
			break;
		case Events.Keys.Down:
			this.menu.move( 1);
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
			this.player.moveTarget.z = 0;
			break;
		case Events.Keys.X:
		case Events.Keys.Z:
			this.player.moveTarget.y = 0;
			break;
		case Events.Keys.Enter:
			this.menu.activate(this);
			break;
		case Events.Keys.Backspace:
			this.menu.back();
			break;
			
	}
}

Game.onPrimary = function( player )
{
	this.build( 'woodcutter' );
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
	var accel = 2.5;
	for( var d in this.player.moveTarget )
	{
		var dif = this.player.moveTarget[d] - this.player.moveSpeed[d];
		this.player.moveSpeed[d] += (dif*accel) * dt;
	}
	if( this.player.moveSpeed.x != 0 || this.player.moveSpeed.y != 0 || this.player.moveSpeed.z != 0 )
	{
		this.player.move( this.player.moveSpeed.z * this.player.movementSpeed * dt, this.player.moveSpeed.y * this.player.movementSpeed * dt, this.player.moveSpeed.x * this.player.movementSpeed * dt );
	}
	
	this.player.setYaw( this.player.turnAngle += (this.player.turnSpeed * dt) );
	this.player.setPitch( this.player.lookAngle );
	
	for( var i = 0; i < this.drones.length; i++ ) {
		this.drones[i].tick(dt);
	}
}

Game.draw = function()
{
	this.menu.draw();
}