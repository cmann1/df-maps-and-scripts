class FxManager{
	
	array<Fx@> effects_current = {};
	array<Fx@> effects_checkpoint = {};
	uint effects_current_count = 0;
	uint effects_checkpoint_count = 0;
	
	Fx@ add_fx(Fx@ fx)
	{
		if(effects_current_count >= effects_current.length())
		{
			effects_current.resize(effects_current_count + 10);
		}
		
		@effects_current[effects_current_count++] = fx;
		
		return fx;
	}
	
	void checkpoint_save()
	{
		effects_checkpoint = effects_current;
		effects_checkpoint_count = effects_current_count;
	}

	void checkpoint_load()
	{
		effects_current = effects_checkpoint;
		effects_current_count = effects_checkpoint_count;
	}
	
	void step()
	{
		for(uint i = 0; i < effects_current_count; i++)
		{
			Fx@ fx = effects_current[i];
			fx.frame += fx.frame_rate;
		}
	}
	
	void draw()
	{
		for(uint i = 0; i < effects_current_count; i++)
		{
			Fx@ fx = effects_current[i];
			if(!effects_current[i].draw())
			{
				@effects_current[i] = @effects_current[effects_current_count - 1];
				@effects_current[--effects_current_count] = null;
			}
		}
	}
	
}

class Fx{
	sprites@ sprite;
	string sprite_name;
	uint palette;
	
	float x;
	float y;
	float rotation;
	float scale_x = 1;
	float scale_y = 1;
	uint colour;
	
	int layer = 19;
	int sublayer = 19;
	
	float start_frame;
	float end_frame;
	float frame = 0;
	float frame_rate;
	bool loop = false;
	
	Fx(float x, float y, string sprite_set, string sprite_name, uint palette=0, uint fps=15, float rotation=0, float scale_x=1, float scale_y=1, uint colour=0xFFFFFFFF)
	{
		this.x = x;
		this.y = y;
		this.rotation = rotation;
		this.scale_x = scale_x;
		this.scale_y = scale_y;
		this.colour = colour;
		
		this.sprite_name = sprite_name;
		@sprite = create_sprites();
		sprite.add_sprite_set(sprite_set);
		
		start_frame = 0;
		end_frame = sprite.get_animation_length(sprite_name);
		
		frame_rate = fps / 60.0;
		this.palette = palette;
	}
	
	bool draw()
	{
		sprite.draw_world(layer, sublayer, sprite_name,
				uint(frame), palette, x, y, rotation,
				scale_x, scale_y, colour);
		
		
		
		if(frame > end_frame)
		{
			if(loop)
			{
				frame -= end_frame - start_frame;
			}
			else
			{
				return false;
			}
		}
		
		return true;
	}
}