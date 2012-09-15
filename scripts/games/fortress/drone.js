(function() {
	this.entity = world.createEntity();
	
	this.vis = this.entity.addComponent('renderable');
	this.vis.setModel('drone.obj');
	this.vis.setPosition( {x:20, y:150, z:20}  );
	
	this.targetY = 150;
	
	this.position = { x: 30, y: 150, z: 30 };
	this.downNormal = { x: 0, y: -1, z: 0 };
	
	this.t = 0;
	// Hack until custom components are implemented.
	this.tick = function( dt ) {
		var p = { x: this.position.x, y: this.position.y, z: this.position.z };
		
		this.t += dt;
		
		p.x += Math.sin(this.t) * 3;
		p.z += Math.cos(this.t) * 3;
		
		var r = new world.createRay();
		r.direction = this.downNormal;
		r.origin = p;
		r = world.fireRay( r );
		if( r.hit ) {
			this.targetY = r.worldHit.y + 3;
		}
		
		this.position.y += (2 * (this.targetY - this.position.y)) * dt;
		this.vis.setPosition(p);
	};
});