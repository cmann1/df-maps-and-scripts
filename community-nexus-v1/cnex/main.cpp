#include '../common/AnimatedSpriteBatch.cpp'
#include '../common/SpriteBatch.cpp'
#include '../common/date.cpp'
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