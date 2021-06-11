enum MineSuperintendentState
{
	Idle,
	QuestActive,
	TaskCompleted,
	GiveReward,
	End
}

class MineSuperintendent : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	[position,mode:world,layer:19,y:door_y1] float door_x1;
	[hidden] float door_y1;
	[position,mode:world,layer:19,y:door_y2] float door_x2;
	[hidden] float door_y2;
	
	[entity] int idle_text_id;
	[entity] int completed_text_id;
	[entity] int other_text_id;
	[entity] int completed_other_text_id;
	
	[position,mode:world,layer:19,y:player_key_y] float player_key_x;
	[hidden] float player_key_y;
	
	[text] float key_scale = 0.25;
	
	float fade_t = -150;
	float fade_time = 50;
	
	[hidden] int quest_state = MineSuperintendentState::Idle;
	
	float t = 0;
	
	MineSuperintendent()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		script.vars.listen('mine_quest_state', this, 'on_quest_update');
		int new_quest_state = script.vars.get_int('mine_quest_state');
		if(new_quest_state != quest_state)
			on_quest_update('', null);
	}
	
	void activate(controllable@ e)
	{
		if(e.player_index() == -1)
			return;
		
		switch(quest_state)
		{
			case MineSuperintendentState::Idle:
				script.vars.set_int('mine_quest_state', MineSuperintendentState::QuestActive);
				break;
			case MineSuperintendentState::TaskCompleted:
				script.vars.set_int('mine_quest_state', MineSuperintendentState::GiveReward);
				break;
		}
	}
	
	void on_quest_update(string id, message@ msg)
	{
		const int new_quest_state = script.vars.get_int('mine_quest_state');
		if(quest_state == new_quest_state) return;
		
		switch(new_quest_state)
		{
			case MineSuperintendentState::QuestActive:
				open_mine_door();
				break;
			case MineSuperintendentState::TaskCompleted:
			{
				entity@ idle_text = entity_by_id(idle_text_id);
				entity@ completed_text = entity_by_id(completed_text_id);
				entity@ other_text = entity_by_id(other_text_id);
				entity@ completed_other_text = entity_by_id(completed_other_text_id);
				if(@completed_text != null && @idle_text != null)
					completed_text.set_xy(idle_text.x(), idle_text.y());
				if(@completed_other_text != null && @other_text != null)
					completed_other_text.set_xy(other_text.x(), other_text.y());
				
				if(@idle_text != null)
					g.remove_entity(idle_text);
				if(@other_text != null)
					g.remove_entity(other_text);
			}
				break;
			case MineSuperintendentState::GiveReward:
			{
				script.vars.set_bool('has_machine_key', true);
				
				entity@ emitter = create_emitter(KingZone, player_key_x, player_key_y, 96, 96, 18, 12);
				g.add_entity(emitter);
				remove_timer(emitter, 10);
			}
				break;
		}
		
		quest_state = new_quest_state;
	}
	
	void open_mine_door()
	{
		const int tx1 = int(floor(door_x1 / 48));
		const int ty1 = int(floor(door_y1 / 48));
		const int tx2 = int(floor(door_x2 / 48));
		const int ty2 = int(floor(door_y2 / 48));
		
		for(int x = tx1; x <= tx2; x++)
		{
			for(int y = ty1; y <= ty2; y++)
			{
				g.set_tile(x, y, 19, false, 0, 0, 0, 0);
			}
		}
	}
	
	void step()
	{
		t++;
		
		if(quest_state == MineSuperintendentState::GiveReward)
		{
			fade_t++;
			
			if(fade_t >= fade_time)
			{
				g.remove_entity(self.as_entity());
			}
		}
	}
	
	void draw(float sub_frame)
	{
		if(quest_state == MineSuperintendentState::GiveReward)
		{
//			if(fade_t >= 0)
//				key_spr.set_colour((uint((1 - fade_t / fade_time) * 255) << 24) | 0xFFFFFF);
			script.music_lock_key_spr.draw(18, 1, player_key_x, player_key_y, 0, (key_scale + sin(t * 0.1) * 0.05) * (1 - max(0, fade_t) / fade_time));
		}
	}
	
	void draw_coll()
	{
		const float tx1 = floor(door_x1 / 48) * 48;
		const float ty1 = floor(door_y1 / 48) * 48;
		const float tx2 = floor(door_x2 / 48) * 48 + 48;
		const float ty2 = floor(door_y2 / 48) * 48 + 48;
		g.draw_rectangle_world(22, 22,
			tx1, ty1, tx2, ty2,
			0, 0x4400FF00);
		outline_rect(g, tx1, ty1, tx2, ty2, 22, 22, 1, 0x8800FF00);
	}
	
	void editor_draw(float sub_frame)
	{
		draw_coll();
		
		entity@ text = entity_by_id(idle_text_id);
		if(@text != null)
			draw_arrow(g, 22, 22, self.x(), self.y(), text.x(), text.y(), 4, 20, 0.5, 0x99FF0000);
		
		@text = entity_by_id(completed_text_id);
		if(@text != null)
			draw_arrow(g, 22, 22, self.x(), self.y(), text.x(), text.y(), 4, 20, 0.5, 0x990000FF);
		
		@text = entity_by_id(other_text_id);
		if(@text != null)
			draw_arrow(g, 22, 22, self.x(), self.y(), text.x(), text.y(), 4, 20, 0.5, 0x99FF0000);
		
		@text = entity_by_id(completed_other_text_id);
		if(@text != null)
			draw_arrow(g, 22, 22, self.x(), self.y(), text.x(), text.y(), 4, 20, 0.5, 0x990000FF);
	}
	
}