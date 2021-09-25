#include '../BaseScript.cpp';
#include 'Coil.cpp';

class script : BaseScript
{
	
	script()
	{
		super();
		g.override_stream_sizes(14, 8);
	}
	
}
