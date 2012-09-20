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
							this.clear();
						}
					}
				]
			}
		]
	}
});
