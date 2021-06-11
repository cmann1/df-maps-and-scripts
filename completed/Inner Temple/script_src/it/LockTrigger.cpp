#include '../common/Sprite.cpp'
#include '../common/SpriteBatch.cpp'

class LockTrigger : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	Sprite dot_spr('props2', 'backdrops_4');
	sprites@ icon_spr;
	array<string> icons = {'sun', 'moon', 'river'};
	
	SpriteBatch holo_glow(
		array<string>={'props3','backdrops_3','props3','backdrops_3',},
		array<int>={15,19,15,19,},
		array<float>={-304,-95,118,67,},
		array<float>={1,1,1,1,},
		array<float>={309.996,229.999,},
		array<uint>={0,0,});
	
	controllable@ player = null;
	float alpha = 0;
	float fade_speed = 1.0 / 2;
	float t = 0;
	
	entity@ emitter = null;
	int emitter_life = 10;
	
	Rnd rnd;
	
	int prev_jump_intent = 0;
	int prev_light_intent = 0;
	
	int byte_count = 3;
	int bit_count = byte_count * 8;
	[hidden] array<int> bits;
//	array<int> bits;
	array<uint> bytes = {154, 36, 216};
	
	[hidden] bool locked = true;
	[hidden] int pointer_index = 0;
	
	[position,layer:19,y:my] float mx = 0;
	[hidden] float my = 0;
	float mx_offset = 48 + 24;
	[text] float spacing = 20;
	[text] float size = 10;
	[text] int layer = 17;
	[text] int sub_layer = 24;
	[colour] uint colour = 0x9FF2FF;
	[position,layer:19,y:icons_y] float icons_x = 0;
	[hidden] float icons_y = 0;
	
	[text] float icon_width = 80;
	[text] float icon_spacing = 10;
	
	LockTrigger()
	{
		@g = get_scene();
		bits.resize(bit_count);
		@icon_spr = create_sprites();
		icon_spr.add_sprite_set('script');
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = @self;
//		bits = array<int> = {0,1,1,2,2,1,2,1, 1,1,2,1,1,2,1,1, 2,2,1,2,2,1,1,1};
		bits = array<int> = {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0};
	}
	
	void activate(controllable@ e)
	{
		if(!locked) return;
		
		for(int i = int(num_cameras()) - 1; i >= 0; i--)
		{
			controllable@ c = controller_controllable(i);
			if(c !is null and c.is_same(e))
			{
				@player = e;
				return;
			}
		}
	}
	
	void step()
	{
		if(player !is null)
		{
			if(alpha > 0.75)
			{
				bool has_changed = false;
				
				if(player.dash_intent() != 0)
				{
					if(++pointer_index >= bit_count) pointer_index = 0;
				}
				else if(player.jump_intent() != 0)
				{
					if(prev_jump_intent == 0)
					{
						if(bits[pointer_index] < 2)
						{
							bits[pointer_index]++;
							has_changed = true;
						}
					}
				}
				else if(player.light_intent() != 0)
				{
					if(prev_light_intent == 0)
					{
						if(bits[pointer_index] > 1)
						{
							bits[pointer_index]--;
							has_changed = true;
						}
					}
				}
				
				if(has_changed)
				{
					bool valid = true;
					int matched = 0;
					for(int byte = 0; byte < 3; byte++)
					{
						const int byte_index = byte * 8;
						uint byte_value = 0;
						for(int i = 0; i < 8; i++)
						{
							int bit  = bits[byte_index + i] - 1;
							if(bit < 0)
							{
								valid = false;
								break;
							}
							
							byte_value |= bit << (7 - i);
						}
						
						if(!valid) break;
						
						if(byte_value == bytes[byte])
							matched++;
					}
					
					if(matched == 3)
					{
						broadcast_message('UNLOCK', create_message());
						
						if(@emitter == null)
						{
							float x = mx;
							float y = my + mx_offset;
							const float width = bit_count * spacing;
							@emitter = create_emitter(126, x, y, int(width), int(size), 15, 12);
							g.add_entity(emitter, false);
						}
						
						locked = false;
						fade_speed *= 2;
					}
				}
			}
			
			prev_light_intent = player.light_intent();
			prev_jump_intent = player.jump_intent();
			
			@player = null;
			
			alpha += fade_speed * DT;
			if(alpha > 1) alpha = 1;
		}
		else
		{
			alpha -= fade_speed * DT;
			
			if(@emitter != null and emitter_life-- <= 0)
			{
				g.remove_entity(emitter);
				@emitter = null;
			}
			
			if(alpha < 0)
			{
				alpha = 0;
				if(!locked) g.remove_entity(self.as_entity());
			}
		}
		
		if(alpha > 0)
		{
			t++;
		}
	}
	
	void draw(float sub_frame)
	{
		const uint spr_colour = (uint(alpha * 0xFF) << 24) + colour;
		
		float icons_width = (icon_width + icon_spacing) * 2;
		float icon_px = icons_x - icons_width * 0.5;
		for(int i = 0; i < 3; i++)
		{
			if(locked)
				icon_spr.draw_world(layer, sub_layer - 1, icons[i], 0, 0, icon_px - 1, icons_y - 1, 0, 1, 1, 0xFFFFFFFF);
			
			if(alpha > 0)
				icon_spr.draw_world(layer, sub_layer, icons[i], 0, 0, icon_px, icons_y, 0, 1, 1, spr_colour);
			
			icon_px += icon_width + icon_spacing;
		}
		
		if(alpha > 0)
		{
			rnd.seed = uint(1000523 + t / 4);
			rnd.random();
			
			const uint glow_colour = (uint(alpha * 0x55) << 24) + colour;
			const float glow_width = 15;
			const float base_spr_scale = 0.075;
			
			const float width = bit_count * spacing;
			float x = mx - width * 0.5 +24;
			float y = my + mx_offset;
			
			for(int i = 0; i < bit_count; i++)
			{
				const int bit = bits[i];
				
				const float spr_scale = (i == pointer_index ? base_spr_scale * 1.45 : base_spr_scale);
				const float size = (i == pointer_index ? this.size * 1.45 : this.size);
				
				if(bit == 1)
				{
					const float x1 = x - size + rnd.range(-1, 1);
					const float y1 = y - size + rnd.range(-1, 1);
					const float x2 = x + size + rnd.range(-1, 1);
					const float y2 = y - size + rnd.range(-1, 1);
					const float x3 = x + size + rnd.range(-1, 1);
					const float y3 = y + size + rnd.range(-1, 1);
					const float x4 = x - size + rnd.range(-1, 1);
					const float y4 = y + size + rnd.range(-1, 1);
					dot_spr.draw_world(layer, sub_layer, 0, 0, x1, y1, 0, spr_scale, spr_scale, spr_colour);
					dot_spr.draw_world(layer, sub_layer, 0, 0, x2, y2, 0, spr_scale, spr_scale, spr_colour);
					dot_spr.draw_world(layer, sub_layer, 0, 0, x3, y3, 0, spr_scale, spr_scale, spr_colour);
					dot_spr.draw_world(layer, sub_layer, 0, 0, x4, y4, 0, spr_scale, spr_scale, spr_colour);
					draw_line_glowing(g, layer, sub_layer, x1, y1, x2, y2, 2, glow_width, glow_colour, spr_colour);
					draw_line_glowing(g, layer, sub_layer, x2, y2, x3, y3, 2, glow_width, glow_colour, spr_colour);
					draw_line_glowing(g, layer, sub_layer, x3, y3, x4, y4, 2, glow_width, glow_colour, spr_colour);
					draw_line_glowing(g, layer, sub_layer, x4, y4, x1, y1, 2, glow_width, glow_colour, spr_colour);
				}
				else if(bit == 2)
				{
					const float x1 = x + rnd.range(-1, 1);
					const float y1 = y - size + rnd.range(-1, 1);
					const float y2 = y + size + rnd.range(-1, 1);
					dot_spr.draw_world(layer, sub_layer, 0, 0, x1, y1, 0, spr_scale, spr_scale, spr_colour);
					dot_spr.draw_world(layer, sub_layer, 0, 0, x1, y2, 0, spr_scale, spr_scale, spr_colour);
					draw_line_glowing(g, layer, sub_layer, x1, y1, x1, y2, 2, glow_width, glow_colour, spr_colour);
				}
				else
					dot_spr.draw_world(layer, sub_layer, 0, 0, x + rnd.range(-1, 1), y + rnd.range(-1, 1), 0, spr_scale * 1.25, spr_scale * 1.25, spr_colour);
				
				if(i == pointer_index)
				{
					dot_spr.draw_world(layer, sub_layer, 0, 0, x + rnd.range(-1, 1), y + 24 + size + rnd.range(-1, 1), 0, spr_scale, spr_scale, spr_colour);
				}
				
				x += spacing;
			}
			
			holo_glow.draw_colour(mx, my, spr_colour);
		}
	}
	
//	void editor_step() { t++; }
//	void editor_draw(float sub_frame) { alpha = 1; draw(sub_frame); }
	
}