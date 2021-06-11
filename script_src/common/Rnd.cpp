class Rnd
{
	
	protected uint _seed = 0;
	protected uint _currentSeed = 0;

	Rnd(uint seed=1) {
		_seed = _currentSeed = seed;
	}
	
	uint get_seed() const
	{
		return _seed;
	}
	void set_seed(uint value)
	{
		_seed = _currentSeed = value;
	}
	
	uint get_currentSeed() const
	{
		return _currentSeed;
	}
	uint set_currentSeed(uint value) const
	{
		return _currentSeed;
	}

	// random(); // returns a number between 0-1 exclusive.
	float random()
	{
		return float(_currentSeed = (_currentSeed * 16807) % 2147483647)/float(0x7FFFFFFF)+0.000000000233;
	}
	
	// float(50); // returns a number between 0-50 exclusive
	// float(20,50); // returns a number between 20-50 exclusive
	float range(float min, float max)
	{
		return random() * (max - min) + min;
	}
	
	// boolean(); // returns true or false (50% chance of true)
	// boolean(0.8); // returns true or false (80% chance of true)
	bool boolean(float chance=0.5)
	{
		return (random() < chance);
	}
	
	// sign(); // returns 1 or -1 (50% chance of 1)
	// sign(0.8); // returns 1 or -1 (80% chance of 1)
	int sign(float chance=0.5)
	{
		return (random() < chance) ? 1 : -1;
	}
	
	// bit(); // returns 1 or 0 (50% chance of 1)
	// bit(0.8); // returns 1 or 0 (80% chance of 1)
	int bit(float chance=0.5)
	{
		return (random() < chance) ? 1 : 0;
	}
	
	// integer(50); // returns an integer between 0-49 inclusive
	// integer(20,50); // returns an integer between 20-49 inclusive
	int integer(int min, int max)
	{
		// Need to use floor instead of bit shift to work properly with negative values:
		return int( floor(random() * (max - min) + min) );
	}
	
	// reset(); // resets the number series, retaining the same seed
	void reset()
	{
		_seed = _currentSeed;
	}
	
}