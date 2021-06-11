#include '../common/utils.cpp'
#include '../common/embeds.cpp'
#include 'embed_sprites.cpp'
#include 'embed_audio.cpp'

class script
{
	
	void build_sprites(message@ msg)
	{
		embed_build_sprites(msg);
	}
	
	void build_sounds(message@ msg)
	{
		embed_build_sounds(msg);
	}
	
}