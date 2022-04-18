const array<string> PROP_GROUP_NAMES = {
	'books', 'buildingblocks', 'chains', 'decoration', 'facade', 'foliage', 'furniture', 'gazebo',
	'lighting', '', 'statues', 'storage', 'study', 'fencing', '', '',
	'', '', 'backleaves', 'leaves', 'trunks', 'boulders', 'backdrops', 'temple',
	'npc', 'symbol', 'cars', 'sidewalk', 'machinery'
};

void sprite_from_prop(prop@ p, string &out sprite_set, string &out sprite_name)
{
	sprite_set = 'props' + p.prop_set();
	sprite_name = PROP_GROUP_NAMES[p.prop_group()] + '_' + p.prop_index();
}
void sprite_from_prop(uint prop_set, uint prop_group, uint prop_index, string &out sprite_set, string &out sprite_name)
{
	sprite_set = 'props' + prop_set;
	sprite_name = PROP_GROUP_NAMES[prop_group] + '_' + prop_index;
}