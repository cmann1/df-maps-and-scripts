class AnimatedSpriteBatch
{
	
	sprites@ spr;
	string sprite_name;
	array<AnimatedSpriteBatchSprite> sprite_list;
	int num_sprites = 0;
	int animation_length;
	
	float t = 0;
	float t_step = 0;
	
	AnimatedSpriteBatch(const string &in sprite_set_name, const string &in sprite_name, float fps=12)
	{
		@spr = create_sprites();
		spr.add_sprite_set(sprite_set_name);
		this.sprite_name = sprite_name;
		animation_length = spr.get_animation_length(sprite_name);
		t_step = fps / 60;
	}
	
	AnimatedSpriteBatch(
		const string &in sprite_set_name, const string &in sprite_name, const array<int> &in layer_sub_layer,
		const array<float> &in x_y, const array<float> &in scale, const array<float> &in rotation, const array<uint> &in palette, float fps=12)
	{
		@spr = create_sprites();
		spr.add_sprite_set(sprite_set_name);
		this.sprite_name = sprite_name;
		animation_length = spr.get_animation_length(sprite_name);
		t_step = fps / 60;
		
		num_sprites = int(palette.length());
		sprite_list.resize(num_sprites);
		int i2 = 0;
		for(int i = 0; i < num_sprites; i++, i2 += 2)
		{
			const int i3 = i2 + 1;
			AnimatedSpriteBatchSprite@ batch_spr = @sprite_list[i];
			
			batch_spr.layer = layer_sub_layer[i2];
			batch_spr.sub_layer = layer_sub_layer[i3];
			batch_spr.x = x_y[i2];
			batch_spr.y = x_y[i3];
			batch_spr.scale_x = scale[i2];
			batch_spr.scale_y = scale[i3];
			batch_spr.rotation = rotation[i];
			batch_spr.palette = palette[i];
		}
	}
	
	void add(int layer, int sub_layer, float x, float y, float scale_x=1, float scale_y=1, float rotation=0, uint palette=0)
	{
		sprite_list.insertLast(
			AnimatedSpriteBatchSprite(layer, sub_layer, x, y, scale_x, scale_y, rotation, palette)
		);
		num_sprites++;
	}
	
	void add(prop@ p)
	{
		sprite_list.insertLast(
			AnimatedSpriteBatchSprite(p.layer(), p.sub_layer(), p.x(), p.y(), p.scale_x(), p.scale_y(), p.rotation(), p.palette())
		);
		num_sprites++;
	}
	
	void step()
	{
		t += t_step;
	}
	
	void draw(float x, float y)
	{
		for(int i = 0; i < num_sprites; i++)
		{
			AnimatedSpriteBatchSprite@ s = sprite_list[i];
			spr.draw_world(s.layer, s.sub_layer, sprite_name, int(t + i * 2) % animation_length, s.palette, x + s.x, y + s.y, s.rotation, s.scale_x, s.scale_y, 0xFFFFFFFF);
		}
	}
	
	void draw_colour(float x, float y, uint colour)
	{
		for(int i = 0; i < num_sprites; i++)
		{
			AnimatedSpriteBatchSprite@ s = sprite_list[i];
			spr.draw_world(s.layer, s.sub_layer, sprite_name, int(t + i * 2) % animation_length, s.palette, x + s.x, y + s.y, s.rotation, s.scale_x, s.scale_y, colour);
		}
	}
	
}

class AnimatedSpriteBatchSprite
{
	string sprite_name;
	int layer, sub_layer;
	float x, y;
	float scale_x, scale_y;
	float rotation;
	uint palette;
	
	AnimatedSpriteBatchSprite(){}
	
	AnimatedSpriteBatchSprite(int layer, int sub_layer, float x, float y, float scale_x=1, float scale_y=1, float rotation=0, uint palette=0)
	{
		this.layer = layer;
		this.sub_layer = sub_layer;
		this.x = x;
		this.y = y;
		this.scale_x = scale_x;
		this.scale_y = scale_y;
		this.rotation = rotation;
		this.palette = palette;
	}
	
}