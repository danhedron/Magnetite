(function(data){
	var menu = data;
	
	this.position = [10, 10];
	this.spacing = 20;
	
	this.breadcrumbs = [];
	
	this.draw = function() {
		console.drawText( this.position[0]-5, this.position[1] - this.spacing, this.breadcrumbs.join(" > ") );
		for( var i = 0; i < menu.items.length; i++ )
		{
			var item = menu.items[i];
			console.drawText( this.position[0], this.position[1] + (i * this.spacing), item.text );
		}
	};
});
