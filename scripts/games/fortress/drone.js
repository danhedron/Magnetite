(function() {
	this.entity = world.createEntity();
	
	this.vis = this.entity.addComponent('renderable');
	debug.log(this.vis.getType());
});