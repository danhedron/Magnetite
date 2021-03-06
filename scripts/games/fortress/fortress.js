var Events = require('./scripts/events.js');
var Menu = require('./scripts/menu.js');
var Drone = require('./scripts/games/fortress/drone.js');
var Structure = require('./scripts/games/fortress/structure.js');

Game.name = 'Fortress';

// Hack I guess.
Game.stockpiles = [];

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
// Returns the smallest posible vector from the input.
function minv( v1, v2 ) { return { x : Math.min( v1.x, v2.x ), y: Math.min( v1.y, v2.y ), z: Math.min( v1.z, v2.z ) }; }
function maxv( v1, v2 ) { return { x : Math.max( v1.x, v2.x ), y: Math.max( v1.y, v2.y ), z: Math.max( v1.z, v2.z ) }; }

Game.onLoad = function()
{
	this.menu = new Menu(require('./scripts/games/fortress/menu.js').root);
	this.menu.position = [20, 30];
	this.drawMenu = false;
	
	// Create a drone.
	this.drones = [];
	for( var i = 0; i < 10; i++ )
	{
		var d = this.newDrone();
		d.overridePosition({ x: Math.random() * 100, y: 140, z: Math.random() * 100 });
		d.create();
	}
}

Game.build = function( t ) 
{
	var rs = world.fireRay( this.player.getEyeCast() );
	
	if( rs.hit )
	{
		var p = { x: rs.worldHit.x + (rs.normal.x /2), y: rs.worldHit.y + (rs.normal.y /2), z: rs.worldHit.z + (rs.normal.z /2) };
		// Try and get a request
		var sr = Structure.create( t, p );
		if( sr != undefined ) {
			// Tell the nearest drone to do something about it.
			this.getNearestDrone( p ).construct(sr);
		}
	}
}

Game.selectArea = function( callback )
{
	var rs = world.fireRay( this.player.getEyeCast() );
	
	if( rs.hit )
	{
		var p = { x: rs.worldHit.x - (rs.normal.x /2), y: rs.worldHit.y - (rs.normal.y /2), z: rs.worldHit.z - (rs.normal.z /2) };
		
		if( this.areaStart == undefined ) 
		{
			this.areaStart = p;
		}
		else 
		{
			var min = {
				x: Math.min( this.areaStart.x, p.x ),
				y: Math.min( this.areaStart.y, p.y ),
				z: Math.min( this.areaStart.z, p.z )
			}, max = {
				x: Math.max( this.areaStart.x, p.x ),
				y: Math.max( this.areaStart.y, p.y ),
				z: Math.max( this.areaStart.z, p.z )
			}
			callback.call( this, min, max );
			this.areaStart = undefined;
		}
	}
}

Game.fillArea = function( type, min, max )
{
	// Tell the nearest drone to do something about it.
	this.getNearestDrone( min ).fillArea( type, min, max );
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
	var d = world.createEntity('drone');
	this.drones.push(d);
	return d;
};

Game.createDrone = function() {
	var rs = world.fireRay( this.player.getEyeCast() );
	
	if( rs.hit )
	{
		var drone = this.newDrone();
		drone.overridePosition( { x: rs.worldHit.x + (rs.normal.x /2), y: rs.worldHit.y - (rs.normal.y /2), z: rs.worldHit.z + (rs.normal.z /2) } );
		drone.create();
	}
}

Game.getNearestDrone = function( center ) {
	
	var drone = world.findEntity({
		'type': 'drone',
		'center': center
	});
	
	return drone;
}

Game.getBestStockpile = function( center, type ) {
	// Returns the closest stockpile that can accomodate the given type.
	var s;
	var c;
	for( var i = 0; i < this.stockpiles.length; i++ )
	{
		var pile = this.stockpiles[i];
		var d = lent( subt( center, pile.position ) );
		if( (c == undefined || d < c) && (pile.isAvailable() && ( type == undefined || pile.canStore(type) )) )
		{
			c = d;
			s = pile;
		}
	}
	return s;
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
		case Events.Keys.W:
			this.player.moveTarget.x = -10;
			break;
		case Events.Keys.A:
			this.player.moveTarget.z = -10;
			break;
		case Events.Keys.S:
			this.player.moveTarget.x = 10;
			break;
		case Events.Keys.D:
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
		case Events.Keys.Space:
			this.drawMenu = !this.drawMenu;
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
}

Game.draw = function()
{
	if( this.drawMenu )
		this.menu.draw();
}