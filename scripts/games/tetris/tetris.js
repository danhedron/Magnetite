Game.name = "Tetris World";

function createTetrisField( w, h )
{
	var field = [];
	for( var x = 0; x < w; x+= 1 )
	{
		for( var y = 0; y < h; y+= 1 )
		{
			field[ (x*h) + y ] = { colour: 0, active: false };
		}
	}
	return field;
}

Game.width = 12;
Game.height = 20;

Game.platformX = 90 - Math.ceil(Game.width / 2);
Game.platformY = 122;
Game.platformZ = 40;
Game.platformBlock = 'cobble';

Game.cameraX = 84;
Game.cameraY = 132;
Game.cameraZ = 55;

Game.timer = 0;

Game.colours = [
	'cobble',
	'dirt',
	'grass',
	'mossycobble',
	'sand'
];

Game.next = [];
Game.blocks = [
	{ 
		colour: 1,
		blocks: [
			[ 1, 2, 1, 1 ],
			[ 0, 0, 0, 0 ],
			[ 0, 0, 0, 0 ],
			[ 0, 0, 0, 0 ]
		]
	},
	{ 
		colour: 2,
		blocks: [
			[ 0, 1, 0, 0 ],
			[ 1, 2, 1, 0 ],
			[ 0, 0, 0, 0 ],
			[ 0, 0, 0, 0 ]
		]
	},
	{ 
		colour: 3,
		blocks: [
			[ 1, 1, 2, 0 ],
			[ 0, 0, 1, 0 ],
			[ 0, 0, 0, 0 ],
			[ 0, 0, 0, 0 ]
		]
	},
	{
		colour: 3,
		blocks: [
			[ 0, 0, 1, 0 ],
			[ 1, 1, 2, 0 ],
			[ 0, 0, 0, 0 ],
			[ 0, 0, 0, 0 ]
		]
	},
	{ 
		colour: 4,
		blocks: [
			[ 0, 0, 0, 0 ],
			[ 0, 1, 1, 0 ],
			[ 0, 2, 1, 0 ],
			[ 0, 0, 0, 0 ]
		]
	},
	{
		colour: 5,
		blocks: [
			[ 0, 1, 0, 0 ],
			[ 0, 1, 1, 0 ],
			[ 0, 0, 1, 0 ],
			[ 0, 0, 0, 0 ]
		]
	}
];

/**
 * Called when the game mode is first initalized
 */
Game.onStart = function()
{
}

/**
 * Called once the world has finished loading
 */
Game.onLoad = function()
{
	this.reset();
	this.createPlatform();
}

/**
 * Creates the platform on which the tetris game is played
 */
Game.createPlatform = function()
{
	for( var x = 0; x < this.width+2; x += 1 )
	{
		world.createBlock( this.platformBlock, this.platformX + x, this.platformY + this.height + 1, this.platformZ + 1 );
		for( var z = 0; z < 2; z += 1 )
		{
			for( var y = 0; y < 5; y += 1 )
			{
				world.createBlock( this.platformBlock, this.platformX + x, this.platformY - y, this.platformZ + z );
			}
		}
	}
	for( var y = 0; y < this.height+1; y += 1 )
	{
		world.createBlock( this.platformBlock, this.platformX, this.platformY + y, this.platformZ + 1 );
		world.createBlock( this.platformBlock, this.platformX + this.width + 1, this.platformY + y, this.platformZ + 1 );
		for( var x = 0; x < this.width; x+= 1 )
		{
			world.createBlock( 'stone', this.platformX + x + 1, this.platformY + y, this.platformZ );
		}
	}
	for( var x = -15; x < 0; x += 1 )
	{
		for( var y = -5; y < this.height+2; y += 1 )
		{
			//world.createBlock( 'stone', this.platformX + x + 1, this.platformY + y, this.platformZ );
			if( ( x > -15 && x < 0 && y < this.height + 1 && y > this.height - 7 ) ) continue;
			world.createBlock( this.platformBlock, this.platformX + x, this.platformY + y, this.platformZ + 1 );
			
		}
	}
}

Game.reset = function()
{
	Game.field = createTetrisField( Game.width, Game.height );
	Game.score = 0;
	this.newBlock();
}

/**
 * Called when a player joins the game
 */
Game.onJoin = function( player )
{
	console.log( player.getPosition() );
}

/**
 * Called when a player spawns
 */
Game.onSpawn = function( player )
{
	player.setPosition( { x: this.cameraX, y: this.cameraY, z: this.cameraZ } );
}

/**
 * Makes all of the active blocks not so
 */
Game.stopAllActive = function()
{
	for( var x = 0; x < this.width; x+=1 )
	{
		for( var y = 0; y < this.height; y+=1 )
		{
			this.field[(x*this.height) + y].active = 0;
		}
	}
}

/**
 * Starts a new block on it's journey
 */
Game.newBlock = function()
{
	while( this.next.length < 4 )
	{
		var data = this.blocks[ Math.floor(Math.random()*this.blocks.length) ];
		this.next.push( data );
	}
	
	var data = this.next.splice(0, 1);
	var block = data[0].blocks;
	for( var y = 0; y < block.length; y += 1 )
	{
		var l = block[y];
		for( var x = 0; x < l.length; x += 1 )
		{
			var v = l[x];
			var p = v > 0;
			this.field[ ((x+(this.width/2)-2)*this.height) + y ] = { colour: p ? data.colour : 0, active: p ? true : false, pivot: (v == 2) ? true : false };
		}
	}
	
}

Game.keyDown = function( key )
{
	// 81 79
	if( key == 79 )
	{
		for( var y = this.height - 1; y >= 0; y-=1 )
		{
			for( var x = 0; x < this.width; x+=1 )
			{
				var b = this.field[(x*this.height) + y];
				if( b.active )
				{
					this.field[((x-1)*this.height) + y] = b;
					this.field[(x*this.height) + y] = { 'colour':0, 'active':0 };
				}
			}
		}
		Game.updateWorld();
	}
	if( key == 80 )
	{
		Game.rotateBlock();
		Game.updateWorld();
	}
	if( key == 81 )
	{
		for( var y = this.height - 1; y >= 0; y-=1 )
		{
			for( var x = this.width - 1; x >= 0; x-=1 )
			{
				var b = this.field[(x*this.height) + y];
				if( b.active )
				{
					this.field[((x+1)*this.height) + y] = b;
					this.field[(x*this.height) + y] = { 'colour':0, 'active':0 };
				}
			}
		}
		this.updateWorld();
	}
	if( key == 77 )
	{
		this.updateBlocks();
	}
	if( key == 57 )
	{
		Game.reset();
	}
}

/**
 * A single tick of the game
 */
Game.updateBlocks = function()
{
	var b;
	var below;
	var stoped = false;
	for( var x = 0; x < this.width; x+=1 )
	{
		for( var y = 0; y < this.height; y+=1 )
		{
			var b = this.field[(x*this.height) + y];
			if( b !== undefined && b.active )
			{
				if( y == this.height - 1 ) 
				{
					stoped = true;
				}
				else
				{
					below = this.field[(x*this.height) + y + 1];
					if( below.active == 0 && below.colour > 0 )
					{
						stoped = true;
					}
				}
			}
		}
	}
	
	if( stoped == true )
	{
		this.stopAllActive();
		this.clearLines();
		this.newBlock();
	}
	
	for( var x = 0; x < this.width; x+=1 )
	{
		for( var y = this.height - 1; y >= 0; y-=1 )
		{
			var b = this.field[(x*this.height) + y];
			if( b !== undefined && b.active )
			{
				this.field[(x*this.height) + y + 1] = b;
				this.field[(x*this.height) + y] = { 'colour':0, 'active':0 };
			}
		}
	}
	
	this.updateWorld();
}

/**
/* Rotate all active blocks around the pivot 
 */
Game.rotateBlock = function()
{
	var top = { x: 1000, y: 1000 };
	var center = { x: 0, y: 0 };
	var bottom = { x: -1000, y: -1000 };
	for( var x = 0; x < this.width; x+=1 )
	{
		for( var y = 0; y < this.height; y+=1 )
		{
			var b = this.field[(x*this.height) + y];
			if( b.active == true && b.colour > 0 )
			{
				top.x = Math.min( x, top.x );
				top.y = Math.min( y, top.y );
				bottom.x = Math.max( x+1, bottom.x );
				bottom.y = Math.max( y+1, bottom.y );
			}
			if( b.pivot == true )
			{
				center.x = x;
				center.y = y;
			}
		}
	}
	var rotated = [];
	var width = bottom.x - top.x;
	var height = bottom.y - top.y;
	for( var y = top.y; y <= bottom.y; y += 1 )
	{
		for( var x = top.x; x <= bottom.x; x += 1 )
		{
			var b = this.field[(x*this.height)+y];
			if( b.active && b.colour > 0 )
			{
				rotated[((x-top.x)*height) + (y - top.y)] = b;
				this.field[(x*this.height)+y] = {colour: 0, active: 0};
			}
		}
	}
	for( var x = 0; x < width; x += 1 )
	{
		for( var y = 0; y < height; y+= 1 )
		{
			var b = rotated[(x*height)+y];
			if( b !== undefined )
			{
				this.field[((top.x+(height-y-1))*this.height) + top.y + x] = b;
			}
		}
	}
}

/**
 * Callback every time the engine updates
 */
Game.think = function( dt )
{
	this.timer += dt;
	while( this.timer > 0.5 )
	{
		this.updateBlocks();
		this.timer -= 0.5;
	}
}

Game.pushDown = function( layer )
{
	for( var y = layer; y >= 0; y -= 1 )
	{
		for( var x = 0; x < this.width; x += 1 )
		{
			if( y == 0 )
			{
				this.field[(x*this.height) + y] = { 'colour':0, 'active':0 };
			}
			else
			{
				var b = this.field[(x*this.height) + y - 1];
				this.field[(x*this.height) + y] = b;
				this.field[(x*this.height) + y - 1] = { 'colour':0, 'active':0 };
			}
		}
	}
}

/**
 * Clear any lines in the grid
 */
Game.clearLines = function()
{
	for( var y = this.height - 1; y >= 0; )
	{
		var fill = true;
		for( var x = 0; x < this.width; x += 1 )
		{
			var b = this.field[(x*this.height) + y];
			if( b.colour == 0 || b.active != false )
			{
				fill = false;
				break;
			}
		}
		if( fill )
		{
			this.score += 100;
			this.pushDown( y );
		}
		else
		{
			y -= 1;
		}
	}
}

/**
 * Draw the grid to the console
 */
Game.drawField = function()
{
	var s = '';
	for( var y = 0; y < this.height; y+=1 )
	{
		var l = '';
		l += '|';
		for( var x = 0; x < this.width; x+=1 )
		{
			b = this.field[(x*this.height) + y];
			if( b === undefined )
			{
				l += 'U';
			}
			else if( !b.active && b.colour == 0 ) {
				l += ' ';
			}
			else
			{
				if( b.pivot !== undefined && b.pivot == true )
				{
					l += 'X';
				}
				else
				{
					l += '#';
				}
			}
			l += '';
		}
		s += l + '|\n';
		//console.log(l);
	}
	s += '\\';
	for( var x = 0; x < this.width; x += 1 )
	{
		s += '=';
	}
	s += '//';
	return s;
}

Game.updateWorld = function()
{
	for( var y = 0; y < this.height; y+=1 )
	{
 		for( var x = 0; x < this.width; x+=1 )
		{
			b = this.field[(x*this.height) + y];
			var block = world.getBlock( this.platformX + x + 1, this.platformY + (this.height - y), this.platformZ + 1 );
			if( block !== undefined && block.type != this.colours[b.colour - 1] )
			{
				world.removeBlock( this.platformX + x + 1, this.platformY + (this.height - y), this.platformZ + 1 );
			}
			if( b.colour != 0 )
			{
				world.createBlock( this.colours[b.colour - 1], this.platformX + x + 1 , this.platformY + (this.height - y), this.platformZ + 1 );
			}
 		}
	}
	
	for( var i = 0; i < this.next.length; i += 1 )
	{
		//5
		var block = this.next[i].blocks;
		for( var y = 0; y < block.length; y += 1 )
		{
			var l = block[y];
			for( var x = 0; x < l.length; x += 1 )
			{
				var v = l[x];
				var p = v > 0;
				var type = this.colours[this.next[i].colour-1];
				if( p )
				{
					world.createBlock( type, this.platformX + ( -5 * i ) - 4 + x, this.platformY + this.height - 1 - y, this.platformZ + 1 );
				}
			}
		}
	}
}

/**
 * Called every time the world is drawn
 */
Game.draw = function( dt )
{
	debug.drawText( 20, 100, 'Score: ' + this.score);
}