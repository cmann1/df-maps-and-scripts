#include '../common-old/AnimatedSpriteBatch.cpp'
#include '../common-old/SpriteBatch.cpp'
#include '../common-old/date.cpp'
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