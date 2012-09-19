(new (function() {
	this.create = function() {
		
		this.vis = this.getEntity().addComponent('renderable');
		
		this.vis.setModel('anvil.obj');
		
		this.vis.setPosition( { x: 30, y: 130, z: 30 } );
	};
})());
