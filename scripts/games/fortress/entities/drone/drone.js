(new (function() {
	this.create = function() {
		this.vis = this.addComponent('renderable');
		
		this.vis.setModel('drone.obj');
		this.vis.setPosition( this.position  );
	};
	
	this.t = Math.random() * 3.14;

	this.targetPosition = { x: 30, y: 130, z: 30 };
	this.movementSpeed = 2.5;
	
	this.hoverHeight = 1.5;
	
	this.position = { x: 30, y: 130, z: 30 };
	this.downNormal = { x: 0, y: -1, z: 0 };
	
	this.constructionDistance = 3.5;
	
	this.actionQueue = [];
	
	// Temp: Stores blocks the drone has collected.
	this.blocks = [];
	this.maxBlocks = 10;
	
	this.overridePosition = function( p ) {
		this.position = JSON.parse(JSON.stringify(p));
		this.targetPosition = JSON.parse(JSON.stringify(p));
	}
	
	this.dump = function( stockpile ) {
		this.actionQueue.unshift({
			type: 'dump',
			stockpile: stockpile
		});
	}
	
	this.construct = function( s ) {
		this.actionQueue.push({
			type: 'construction',
			structure: s
		});
	}
	
	this.clearArea = function( min, max ) {
		this.actionQueue.push({
			type: 'clear',
			min: min,
			max: max
		});
	}
	
	this.fillArea = function( type, min, max ) {
		this.actionQueue.push({
			type: 'fill',
			min: min,
			max: max,
			block: type
		});
	}
	this.storeBlock = function( b ) {
		this.blocks.push( b );
		if( this.blocks.length > this.maxBlocks )
		{
			var pile = Game.getBestStockpile( this.position );
			if( pile )
			{
				this.dump( pile );
			}
		}
	}
	
	// Hack until custom components are implemented.
	this.think = function( dt ) {
		var p = { x: this.position.x, y: this.position.y, z: this.position.z };
		
		this.t += dt;
		
		var r = new world.createRay();
		r.direction = this.downNormal;
		r.origin = add( p, { x: 0, y: 5, z: 0 } );
		r = world.fireRay( r );
		if( r.hit ) {
			// Let drones fly if they need to in order to reach their target.
			if( this.actionQueue.length == 0 ) {
				this.targetPosition.y = r.worldHit.y + this.hoverHeight + (Math.sin(this.t*2)*0.25);
			}
			else {
				this.targetPosition.y = Math.max(r.worldHit.y + this.hoverHeight + (Math.sin(this.t*2)*0.25), this.targetPosition.y);
			}
		}
		
		var axes = ['x', 'y', 'z'];
		for( var d in axes )
		{
			d = axes[d];
			this.position[d] += Math.min((this.targetPosition[d] - this.position[d]), this.movementSpeed) * dt;
		}
		
		this.vis.setPosition(p);
		
		if( this.actionQueue.length > 0 )
		{
			var action = this.actionQueue[0];
			switch( action.type ) {
				case 'construction':
					var structure = action.structure;
					var direction = norm(subt( this.position, structure.position ));
					this.targetPosition = add( structure.position, mul(direction, 2 ) );
					var dist = lent( subt( structure.position, this.position ) );
					if( dist < this.constructionDistance )
					{
						this.actionQueue = this.actionQueue.slice(1);
						Game.createStructure( structure );
					}
					break;
				case 'clear':
					// Find the first non-empty block in the area
					var block = false;
					var closest = 1000;
					for( var x = action.min.x; x <= action.max.x; x++ )
					{
						for( var z = action.min.z; z <= action.max.z; z++ )
						{
							for( var y = action.min.y; y <= action.max.y; y++ )
							{
								var b = world.getBlock( x, y, z );
								if( b != undefined ) {
									block = true; // we have at least one block.
									var direction = subt( this.position, {x: x, y: y, z: z} );
									var dist = lent( direction );
									if( dist < closest ) {
										this.targetPosition = {x: x, y: y, z: z};
										closest = dist;
									}
									if( dist < this.constructionDistance )
									{
										this.storeBlock(b.type);
										world.removeBlock( x, y, z );
									}
								}
							}
						}
					}
					if( block == false )
					{
						this.actionQueue = this.actionQueue.slice(1);
					}
					break;
				case 'fill':
					var blocks = 0;
					for( var y = action.min.y; y <= action.max.y; y++ )
					{
						for( var x = action.min.x; x <= action.max.x; x++ )
						{
							for( var z = action.min.z; z <= action.max.z; z++ )
							{
								var b = world.getBlock( x, y, z );
								if( b == undefined ) {
									block = true; // we have at least one block.
									if( blocks == 0 ) {
										var direction = subt( this.position, {x: x, y: y, z: z} );
										this.targetPosition = add( {x: x, y: y, z: z}, mul(norm(direction), 2 ) );
										var dist = lent( direction );
										if( dist < this.constructionDistance )
										{
											world.createBlock( action.block, x, y, z );
										}
									}
									blocks++;
								}
							}
						}
					}
					if( blocks == 0 )
					{
						this.actionQueue = this.actionQueue.slice(1);
					}
					break;
				case 'dump':
						this.targetPosition = add( action.stockpile.position, { x: 0, y: 0, z: 0 } );
						var direction = subt( this.position, action.stockpile.position );
						var dist = lent( direction );
						if( dist < this.constructionDistance )
						{
							for( var i = 0; i < this.blocks.length; ) 
							{
								if( !action.stockpile.isAvailable() )
								{
									// Suddenly, the stockpile is unavailable :c
									this.storeBlock( this.blocks[i] ); break;
								}
								else if( action.stockpile.canStore( this.blocks[i] ) )
								{
									action.stockpile.store( this.blocks[i] );
									this.blocks.splice( i, 1 );
								}
							}
							this.actionQueue = this.actionQueue.slice(1);
						}
					break;
			}
		}
	};

})());
