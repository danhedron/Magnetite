(function() {
	this.entity = world.createEntity();
	
	this.vis = this.entity.addComponent('renderable');
	this.vis.setModel('drone.obj');
	this.vis.setPosition( {x:20, y:130, z:20}  );
	
	this.targetY = 130;
	
	this.position = { x: 30, y: 130, z: 30 };
	
	this.t = 0;
	// Hack until custom components are implemented.
	this.tick = function( dt ) {
		var p = { x: 30, y: this.position.y, z: 30 };
		
		this.t += dt;
		
		p.x += Math.sin(this.t) * 10;
		p.z += Math.cos(this.t) * 10;
		
		var r = new world.createRay();
		r.direction = {x: 0, y: -1, z: 0};
		r.origin = p;
		var rs = world.fireRay( r );
		if( rs.hit ) {
			this.targetY = rs.worldHit.y + 3;
		}
		
		var deltaY = this.targetY - this.position.y;
		this.position.y += (2 * deltaY) * dt;
		
		console.log( JSON.stringify( this.position ) );
		
		this.vis.setPosition(p);
	};
});