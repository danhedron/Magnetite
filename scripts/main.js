console.log('hello world');

var someBlock = world.getBlock( 10, 10, 10 );
if( someBlock != undefined )
{
	console.log(someBlock.type);
}

console.log('Available Types: ');
var types = meta.blocks.availableTypes;
for( var x = 0; x < types.length; x += 1 )
{
	console.log( '\t' + meta.blocks.availableTypes[x] );
}