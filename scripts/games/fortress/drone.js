(function(game) {
	this.entity = world.createEntity();
	
	this.vis = this.entity.addComponent('renderable');
	this.vis.setModel('drone.obj');
	this.vis.setPosition( {x:20, y:130, z:20}  );
	
	this.targetPosition = { x: 30, y: 130, z: 30 };
	this.movementSpeed = 2.5;
	
	this.position = { x: 30, y: 130, z: 30 };
	this.downNormal = { x: 0, y: -1, z: 0 };
	
	this.constructionDistance = 3.5;
	this.constructionQueue = [];
	
	this.t = Math.random() * Math.PI*2;
	
	this.overridePosition = function( p ) {
		this.position = JSON.parse(JSON.stringify(p));
		this.targetPosition = JSON.parse(JSON.stringify(p));
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
			this.targetPosition.y = r.worldHit.y + 2 + (Math.sin(this.t*2)*0.25);
		}
		
		var axes = ['x', 'y', 'z'];
		for( var d in axes )
		{
			d = axes[d];
			this.position[d] += Math.min((this.targetPosition[d] - this.position[d]), this.movementSpeed) * dt;
		}
		
		this.vis.setPosition(p);
		
		if( this.constructionQueue.length > 0 )
		{
			var structure = this.constructionQueue[0];
			var direction = norm(subt( this.position, structure.position ));
			this.targetPosition = add( structure.position, mul(direction, 2 ) );
			var dist = lent( subt( structure.position, this.position ) );
			if( dist < this.constructionDistance )
			{
				game.createStructure( structure );
				this.constructionQueue = this.constructionQueue.slice(1, this.constructionQueue.length);
			}
		}
	};
});