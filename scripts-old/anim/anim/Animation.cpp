#include 'Track.cpp'

class Animation
{
	
	Model@ model;
	
	string name = '';
	int length = 0;
	float fps = 30;
	
	bool loop = true;
	bool skip_last_frame = false;
	
	array<Track@> tracks;
	array<Track@> track_list;
	int num_tracks = 0;
	
	Animation(string name, Model@ model)
	{
		this.name = name;
		@this.model = model;
		tracks.resize(model.num_nodes);
	}
	
	Track@ addTrack(Node@ node)
	{
		return addTrack(node.id);
	}
	
	Track@ addTrack(int id)
	{
		if(id < 0 or id >= model.num_nodes)
		{
			return null;
		}
		
		Track@ track = Track(id);
		track_list.insertLast(track);
		num_tracks++;
		return @tracks[id] = track;
	}
	
	/////////
	
	void save(ByteArray@ &in data, StringTable@ &in string_table)
	{
		data.writeEncodedInt(string_table.get_string_id(name));
		data.writeEncodedInt(length);
		data.writeFloat(fps);
		data.writeByte(loop ? 1 : 0);
		data.writeByte(skip_last_frame ? 1 : 0);
		data.writeEncodedInt(num_tracks);
		
		for(int i = 0; i < num_tracks; i++)
		{
			track_list[i].save(data, string_table);
		}
	}
	
}