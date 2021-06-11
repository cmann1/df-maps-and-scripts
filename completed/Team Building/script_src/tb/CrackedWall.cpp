class CrackedWall : trigger_base, callback_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	[text] string portal = "default";
	[text] int apple_count = 5;
	[entity] int text_id;
	
	CrackedWall()
	{
		@g = get_scene();
		add_broadcast_receiver("portal_apple_count", this, "portal_apple_count");
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		
		check_apple_count(s.ge_apple_count(portal));
	}
	
	void portal_apple_count(string id, message@ msg)
	{
		check_apple_count(msg.get_int("count"));
	}

	void check_apple_count(int count)
	{
		if(count == apple_count)
		{
			const int tx = int(floor(self.x() / 48));
			const int ty = int(floor(self.y() / 48));
			
			tileinfo@ tile = g.get_tile(tx, ty);
			tile.type(4);
			g.set_tile(tx, ty, 19, tile, true);
			@tile = g.get_tile(tx, ty - 1);
			tile.type(3);
			g.set_tile(tx, ty - 1, 19, tile, true);
			
			g.remove_entity(self.as_entity());
			g.remove_entity(entity_by_id(text_id));
		}
	}
	
	void editor_draw(float sub_frame)
	{
		const float tx = floor(self.x() / 48) * 48;
		const float ty = floor(self.y() / 48) * 48;
		g.draw_rectangle_world(21, 21, tx, ty - 48, tx + 48, ty + 48, 0, 0x77FFFFFF);
	}
	
}