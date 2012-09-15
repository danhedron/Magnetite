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
					}
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
