interface IOpenable
{
	
	bool is_closed { get const; }
	
	bool is_alive { get const; }
	
	void open(const bool open=true);
	
	void toggle();
	
}
