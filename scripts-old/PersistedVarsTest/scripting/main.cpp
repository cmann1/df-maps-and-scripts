#include '../lib/std.cpp';
#include '../lib/PersistedVars.cpp';
#include '../lib/drawing/SpriteGroup.cpp';
#include '../lib/drawing/common.cpp';

class script
{
	
	PersistedVars vars;
	
	script()
	{
		get_scene().override_stream_sizes(12, 8);
	}
	
	void on_level_start()
	{
		vars.active = true;
	}
	
	void checkpoint_save()
	{
		vars.checkpoint_save();
	}
	void checkpoint_load()
	{
		vars.checkpoint_load();
	}
	
}

class LightTrigger : trigger_base, PersistedVarListener
{
	
	scene@ g;
	script@ script;
	scripttrigger@ self;
	
	[text] bool powered_on = false;
	[text] float size = 20;
	[text] int index = 0;
	[colour,alpha] uint off_colour = 0xEEAAAA99;
	[colour,alpha] uint on_colour = 0xFFFFFFDD;
	
	[text] float brightness = 0;
	
	float rotation = 0;
	
	SpriteGroup spr;
	
	LightTrigger()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@script = s;
		@this.self = self;
		
		script.vars.register('light_power', this, powered_on, index);
		script.vars.register('light_size', this, size);
		
		self.editor_handle_size(5);
		
		spr.add('props2', 'backdrops_4', 0, 1, 0.51, 0.49);
		spr.add('props3', 'backdrops_4', 0, 0, 0.15, 1, 0, 0, 0);
		spr.add('props3', 'backdrops_4', 0, 0, 0.15, 1, 0, 0, 90);
		spr.add('props3', 'backdrops_4', 0, 0, 0.15, 1, 0, 0, 180);
		spr.add('props3', 'backdrops_4', 0, 0, 0.15, 1, 0, 0, 270);
	}
	
	void on_remove()
	{
		script.vars.unregister('light_power', this);
	}
	
	void on_var_change(string name, int index)
	{
		if(name == 'light_power')
		{
			if(index == this.index)
				powered_on = script.vars.get_bool(name, index);
		}
		else if(name == 'light_size')
		{
			size = script.vars.get_float(name);
		}
	}
	
	void step()
	{
		if(powered_on)
		{
			if(brightness < 1)
			{
				brightness += 0.05;
				if(brightness > 1) brightness = 1;
			}
			rotation++;
		}
	}
	
	void draw(float sub_frame)
	{
		const float x = self.x();
		const float y = self.y();
		g.draw_rectangle_world(17, 19, x-size, y-size, x+size, y+size, 45, powered_on ? on_colour : off_colour);
		if(powered_on)
		{
			spr.draw(self.x(), self.y(), rotation, size / 20.0, 16, 18, brightness);
//			SpriteRectangle r = spr.get_rectangle(rotation, size / 20.0);
//			outline_rect(g, x + r.left, y + r.top, x + r.right, y + r.bottom, 17, 19, 2);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}