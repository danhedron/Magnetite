(function() {
	return new (function() {
		this.create = function() {
			console.log('Anvil created!');
		};
	})();
})();
