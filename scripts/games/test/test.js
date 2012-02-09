Game.name = "Test Game Type"; 
Game.someValue = 210;

/**
 * Called when the game mode is first initalized
 */
Game.onStart = function()
{
	console.log('Javascript game type loaded');
	console.log('someValue: ' + this.someValue);
}

/**
 * Called once the world has finished loading
 */
Game.onLoad = function()
{
	// Create some fancy pillar to demonstrate something
	for( var i = 0; i < 140; i++ )
	{
		world.removeBlock( 10, i, 10 );
		world.createBlock( 'cobble', 10, i, 10 );
	}
}

/**
 * Called when a player joins the game
 */
Game.onJoin = function( player )
{
	console.log('Player joined');
}

/**
 * Called when a player spawns
 */
Game.onSpawn = function( player )
{
	console.log('Player spawned');
}

/**
 * Callback every time the engine updates
 */
Game.think = function( dt )
{
	
}

/**
 * Called every time the world is drawn
 */
Game.draw = function( dt )
{
	debug.drawText( 100, 100, 'Javascript says hello');
}