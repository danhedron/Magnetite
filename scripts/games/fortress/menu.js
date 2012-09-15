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
				]
			},
			{
				text: 'Units',
				chidren: [
					{ 
						text: 'Drone',
						action: function() {}
					}
				]
			}
		]
	}
});
