console.log('hello world');

var someBlock = world.getBlock( 10, 10, 10 );
if( someBlock != undefined )
{
	console.log(someBlock.type);
}

for( var i = 0; i < 140; i++ )
{
	world.removeBlock( 10, i, 10 );
	world.createBlock( 'cobble', 10, i, 10 );
}