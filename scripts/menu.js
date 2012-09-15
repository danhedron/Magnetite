(function(data){
	var menu = data;
	var currentMenu = menu;
	var lastMenus = [];
	
	this.position = [10, 10];
	this.spacing = 20;
	
	this.breadcrumbs = [ currentMenu.text ];
	
	this.current = 0;
	
	this.draw = function() {
		console.drawText( this.position[0]-5, this.position[1] - this.spacing, this.breadcrumbs.join(" > ") );
		for( var i = 0; i < currentMenu.children.length; i++ )
		{
			var item = currentMenu.children[i];
			console.drawText( this.position[0] + ( this.current == i ? 5 : 0 ), this.position[1] + (i * this.spacing), item.text );
		}
	};
	
	this.move = function( d ) {
		this.current = Math.min(Math.max( this.current + d, 0 ), currentMenu.children.length-1 );
	}
	
	this.activate = function(ctx) {
		var mi = currentMenu.children[this.current];
		if( mi.children != undefined && mi.children.length > 0 ) {
			lastMenus.push(currentMenu);
			currentMenu = mi;
			this.breadcrumbs.push( currentMenu.text );
		}
		else if( mi.action != undefined ) {
			mi.action.call( ctx );
		}
	}
	
	this.back = function() {
		if( lastMenus.length > 0 )
		{
			currentMenu = lastMenus.pop();
			this.breadcrumbs.pop();
		}
	}
});
