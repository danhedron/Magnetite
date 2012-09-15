(function(game) {
	this.entity = world.createEntity();
	
	this.vis = this.entity.addComponent('renderable');
	this.vis.setModel('drone.obj');
	this.vis.setPosition( {x:20, y:130, z:20}  );
	
	this.targetPosition = { x: 30, y: 130, z: 30 };
	this.movementSpeed = 2.5;
	
	this.hoverHeight = 1.5;
	
	this.position = { x: 30, y: 130, z: 30 };
	this.downNormal = { x: 0, y: -1, z: 0 };
	
	this.constructionDistance = 3.5;
	
	this.actionQueue = [];
	
	this.t = Math.random() * Math.PI*2;
	
	this.overridePosition = function( p ) {
		this.position = JSON.parse(JSON.stringify(p));
		this.targetPosition = JSON.parse(JSON.stringify(p));
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
	
	// Hack until custom components are implemented.
	this.tick = function( dt ) {
		var p = { x: this.position.x, y: this.position.y, z: this.position.z };
		
		this.t += dt;
		
		var r = new world.createRay();
		r.direction = this.downNormal;
		r.origin = p;
		r = world.fireRay( r );
		if( r.hit ) {
			this.targetPosition.y = r.worldHit.y + this.hoverHeight + (Math.sin(this.t*2)*0.25);
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
						game.createStructure( structure );
						this.actionQueue = this.actionQueue.slice(1);
					}
					break;
				case 'clear':
					// Find the first non-empty block in the area
					var block = false;
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
									this.targetPosition = add( {x: x, y: y, z: z}, mul(norm(direction), 2 ) );
									var dist = lent( direction );
									if( dist < this.constructionDistance )
									{
										world.removeBlock( x, y, z );
									}
								}
							}
						}
					}
					if( block == false )
					{
						this.actionQueue = this.actionQueue.slice(1);
						console.log( JSON.stringify( this.actionQueue ) );
					}
					break;
			}
		}
	};
});