({
	root: {
		text: 'Menu',
		children: [
			{
				text: 'Construction',
				children: [
					{
						text: 'Woodcutter\'s Hut',
						action: function() {
							this.build('woodcutter');
						}
					},
					{
						text: 'Forge',
						action: function() {
							this.build('forge');
						}
					},
					{
						text: 'Guard Tower',
						action: function() {
							this.build('tower');
						}
					},
					{
						text: 'Foundations',
						action: function() {
							this.build('foundation');
						}
					},
					{
						text: 'Stockpile',
						action: function() {
							this.build('stockpile');
						}
					},
					{
						text: 'Wall',
						children: [
							{ 
								text: 'Wooden',
								action: function() {
									this.build('wwall');
								}
							},
							{ 
								text: 'Log',
								action: function() {
									this.build('wall');
								}
							},
							{ 
								text: 'Stone',
								action: function() {
									this.build('swall');
								}
							},
						]
					}
				]
			},
			{
				text: 'Units',
				children: [
					{ 
						text: 'Drone',
						action: function() {
							this.createDrone();
						}
					}
				]
			},
			{
				text: 'Terraforming',
				children: [
					{ 
						text: 'Clear area',
						action: function() {
							this.selectArea( this.clearArea );
						}
					},
					{ 
						text: 'Fill area',
						children: [
							{
								text: 'Dirt',
								action: function() {
									this.selectArea( function( min, max ) { this.fillArea( 'dirt', min, max ) } );
								}
							},
							{
								text: 'Grass',
								action: function() {
									this.selectArea( function( min, max ) { this.fillArea( 'grass', min, max ) } );
								}
							}
						]
						
					}
				]
			}
		]
	}
});
