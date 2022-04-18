#include 'lib/AnimatedSpriteBatch.cpp'
#include 'lib/SpriteBatch.cpp'
#include 'lib/date.cpp'
#include 'MainTrigger.cpp'

class script
{
	
	bool in_game = false;
	
	script()
	{
	}
	
	void on_level_start()
	{
		in_game = true;
	}
	
}