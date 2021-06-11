#include '../lib/drawing/SpriteBatch.cpp';
#include '../lib/utils/colour.cpp';

class Gravestone : trigger_base
{
	
	scene@ g;
	script@ script;
	scripttrigger@ self;
	
	[text] int layer = 11;
	[text] int sublayer = 17;
	[text] int fg_layer = -1;
	[text] int fg_sublayer = -1;
	[colour,alpha] uint text_clr = 0;
	[colour,alpha] uint light_clr = 0;
	[colour,alpha] uint dark_clr = 0;
	[text] float glow_oy;
	
	textfield@ text;
	float text_scale = 1;
	float text_rotation = 0;
	float text_x, text_y;
	SpriteBatch glow_spr(
		array<string>={'props2','backdrops_4','props3','backdrops_3',},
		array<int>={11,17,11,18,},
		array<float>={6.20703,263.317,49.207,59.3168,},
		array<float>={1,1,0.442637,0.442637,},
		array<float>={0,92.7795,},
		array<uint>={0,0,});
	SpriteBatch fg_flow_spr(
		array<string>={'props3','backdrops_3',},
		array<int>={11,2,},
		array<float>={56.2197,87.3741,},
		array<float>={0.442637,0.442637,},
		array<float>={92.6093,},
		array<uint>={0,});
	
	int activated;
	float alpha;
	
	void init(script@ s, scripttrigger@ self)
	{
		@g = get_scene();
		@this.script = s;
		@this.self = self;
//		puts(self.radius());
//		self.radius(80);
		
		int i = g.get_entity_collision(
			self.y() - self.radius() - 24, self.y() + self.radius() + 24,
			self.x() - self.radius() - 24, self.x() + self.radius() + 24, 16);
		
		while(i-- > 0)
		{
			entity@ e = g.get_entity_collision_index(i);
			
			if(e.type_name() != 'z_text_prop_trigger')
				continue;
			
			varstruct@ vars = e.vars();
			
			if(vars.get_var('layer').get_int32() != layer)
				continue;
			
			@text = create_textfield();
			text.align_horizontal(-1);
			text.align_vertical(-1);
			text.set_font(vars.get_var('font').get_string(), vars.get_var('font_size').get_int32());
			text.text(vars.get_var('text').get_string());
			text_scale = vars.get_var('text_scale').get_float();
			text_rotation = vars.get_var('text_rotation').get_int32();
			text_x = e.x();
			text_y = e.y();
			break;
		}
		
		for(uint j = 0; j < glow_spr.sprite_list.length(); j++)
		{
			glow_spr.sprite_list[j].layer = layer;
		}
		
		if(fg_layer != -1)
		{
			for(uint j = 0; j < fg_flow_spr.sprite_list.length(); j++)
			{
				fg_flow_spr.sprite_list[j].layer = fg_layer;
				fg_flow_spr.sprite_list[j].sub_layer = fg_sublayer;
			}
		}
	}
	
	void activate(controllable@ c)
	{
		if(c.player_index() == -1)
			return;
		
		activated = 4;
	}
	
	void step()
	{
		if(activated > 0)
		{
			activated--;
			
			if(alpha < 1)
			{
				alpha = min(1.0, alpha + 0.025);
			}
		}
		else if(alpha > 0)
		{
			alpha = max(0.0, alpha - 0.025);
		}
	}
	
	void draw(float sub_frame)
	{
		if(@text == null || alpha == 0)
			return;
		
		text.colour(multiply_alpha(light_clr == 0 ? script.light_clr : light_clr, alpha));
		text.draw_world(layer, sublayer, text_x, text_y - 1, text_scale, text_scale, text_rotation);
		text.colour(multiply_alpha(dark_clr == 0 ? script.dark_clr : dark_clr, alpha));
		text.draw_world(layer, sublayer, text_x, text_y + 1, text_scale, text_scale, text_rotation);
		text.colour(multiply_alpha(text_clr == 0 ? script.text_clr : text_clr, alpha));
		text.draw_world(layer, sublayer, text_x, text_y, text_scale, text_scale, text_rotation);
		
		glow_spr.draw(self.x(), self.y() + glow_oy, multiply_alpha(0xffffffff, alpha));
		
		if(fg_layer != -1)
		{
			fg_flow_spr.draw(self.x(), self.y() + glow_oy, multiply_alpha(0xffffffff, alpha));
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}