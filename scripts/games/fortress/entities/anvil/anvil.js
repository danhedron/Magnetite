(new (function() {
	this.create = function() {
		this.vis = this.getEntity().addComponent('renderable');
		
		this.vis.setModel('anvil.obj');
	};
})());
