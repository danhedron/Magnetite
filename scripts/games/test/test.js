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