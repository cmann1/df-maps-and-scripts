#include '../common/utils.cpp'
#include 'Torch.cpp'
#include 'Key.cpp'
#include 'Bridge.cpp'
#include 'Dragon.cpp'

class script
{
	
	[hidden] array<int> keys(32, 0);
	[hidden] int key_count = 0;
	
	void set_key(int key_id)
	{
		keys[key_id] = 1;
		key_count++;
	}
	
	bool has_key(int key_id)
	{
		if(key_id < 0 or key_id > int(keys.length()))
			return false;
		
		return keys[key_id] == 1;
	}
	
}