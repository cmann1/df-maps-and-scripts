class script
{
	
	scene@ g;
	
	script()
	{
		@g = get_scene();
		g.override_stream_sizes(16, 8);
	}
	
}
