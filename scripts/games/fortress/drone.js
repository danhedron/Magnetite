(function() {
	this.entity = world.createEntity();
	
	this.vis = this.entity.addComponent('renderable');
	this.vis.setModel('drone.obj');
	this.vis.setPosition( {x:20, y:130, z:20}  );
	
	debug.log(this.vis.getType());
	
	this.t = 0;
	// Hack until custom components are implemented.
	this.tick = function( dt ) {
		var p = { x: 30, y: 125, z:30 };
		
		this.t += dt;
		
		p.x += Math.sin(this.t) * 5;
		p.z += Math.cos(this.t) * 5;
		
		this.vis.setPosition(p);
	};
});