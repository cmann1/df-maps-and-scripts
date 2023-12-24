class TempEmitter
{

	entity@ e;
	float timer;

	TempEmitter() { }

	TempEmitter(entity@ e, const float timer=0.25)
	{
		@this.e = e;
		this.timer = timer;
	}

}
