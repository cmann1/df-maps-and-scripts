#include "../common/math.cpp"
#include "../common/drawing_utils.cpp"
#include "Vehicle.cpp"

class Bat : trigger_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	Vehicle vehicle;
	
	sprites@ spr;
	audio@ flapping_snd = null;
	
	float t = 0;
	
	[text] float anim_speed = 0.5;
	[text] float speed = 1;
	[text] float acc = 0.01;
	[text] int layer = 17;
	[text] int sub_layer = 19;
	[slider,min:0,max:200] float radius = 100;
	[angle] float rotation = 0;
	[text] bool flying = true;
	[position,layer:19,y:fly_y] float fly_x = 0;
	[hidden] float fly_y = 0;
	
	float p = 0;
	float p_max;
	float px = 0;
	float py = 0;
	
	Bat()
	{
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		spr.add_sprite_set("script");
		vehicle.loc_x = self.x();
		vehicle.loc_y = self.y();
		vehicle.maxspeed = speed;
		vehicle.maxforce = acc;
		choose_location();
		t = rand() % 1000;
		
		self.editor_show_radius(!flying);
		self.editor_handle_size(4);
		
		self.editor_colour_inactive(0x88818181);
		self.editor_colour_active(0x88afafaf);
		self.editor_colour_circle(0x55818181);
	}
	
	void choose_location()
	{
		px = fly_x + frand() * radius;
		py = fly_y + frand() * radius;
	}
	
	void activate(controllable@ e)
	{
		if(flying) return;
		
		for(uint i = 0; i < num_cameras(); i++)
		{
			entity@ player = controller_entity(i);
			if(@player != null and e.is_same(player))
			{
				vehicle.loc_x = self.x();
				vehicle.loc_y = self.y() + 30;
				flying = true;
//				if(frand() >= 0.4) g.play_script_stream("bat_0" + (rand() % 2 + 1), 2, self.x(), self.y(), false, 1.0);
				@flapping_snd = g.play_script_stream("bird_flapping_01", 2, self.x(), self.y(), false, 0.4);
				flapping_snd.positional(true);
				return;
			}
		}
	}
	
	void step()
	{
		if(flying)
		{
			t++;
			
			if(abs(vehicle.loc_x - px) < 1 and abs(vehicle.loc_y - py) < 1)
			{
				choose_location();
			}
			else
			{
				vehicle.arrive(px, py);
			}
			
			if(@flapping_snd != null)
			{
				if(!flapping_snd.is_playing()) @flapping_snd = null;
				else flapping_snd.set_position(vehicle.loc_x, vehicle.loc_y);
			}
			
			vehicle.update();
		}
	}
	
	void editor_step()
	{
		step();
	}
	
	void draw(float sub_frame)
	{
		const float x = flying ? lerp(vehicle.prev_x, vehicle.loc_x, sub_frame) : self.x();
		const float y = flying ? lerp(vehicle.prev_y, vehicle.loc_y, sub_frame) : self.y();
		const int frame = 0;
		const int palette = 0;
		const float s = sin(t * anim_speed);
		const float v = sgn(s);
		spr.draw_world(layer, sub_layer, 
			!flying ? "bat_hanging" : (v < 0 ? "bat_01" : "bat_02"),
			frame, palette, x, flying ? y + sin(t * anim_speed * 0.2) * 8 : y, flying ? rotation : 0, 1, 1, 0xFFFFFFFF);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		if(self.editor_selected())
		{
			g.draw_line(21, 10, self.x(), self.y(), fly_x, fly_y, 1, 0x88FFFFFF);
			g.draw_rectangle_world(22, 10, fly_x - 5, fly_y - 5, fly_x + 5, fly_y + 5, 0, 0xAAFF0000);
			outline_rect(g, fly_x - radius, fly_y - radius, fly_x + radius, fly_y + radius, 22, 10, 2, 0xAAFF0000);
		}
	}
	
}


















