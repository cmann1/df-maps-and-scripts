#include "../common/ColType.cpp"
#include "Vehicle.cpp"

enum GhostMaidState
{
	GM_WAITING,
	GM_READY,
	GM_TALKING,
	GM_MOVING,
	GM_FADING
}

class GhostMaid : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	[hidden] Vehicle vehicle;
	
	[text] int layer = 17;
	[text] int sub_layer = 19;
	[slider,min:0,max:1] float alpha = 0.5;
	[text] float facing = 1;
	
	[entity] int text_id;
	entity@ text_trigger = null;
	
	[text] float speed = 1;
	[text] float acc = 0.01;
	
	[hidden] GhostMaidState state = GM_WAITING;
	[hidden] int talk_timer = 360;
	[hidden] float master_alpha = 1;
	[hidden] float fade_time = 180;
	[hidden] float fade = fade_time;
	[hidden] float grave_x = 0;
	[hidden] float grave_y = 0;
	
	float t = 0;
	
	sprites@ spr;
	
	GhostMaid()
	{
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		spr.add_sprite_set("maid");
		spr.add_sprite_set("props3");
		add_broadcast_receiver("husband_grave_complete", this, "on_grave_dig_complete");
		
		vehicle.loc_x = self.x();
		vehicle.loc_y = self.y();
		vehicle.maxspeed = speed;
		vehicle.maxforce = acc;
		self.editor_colour_inactive(0xFF57cca5);
		self.editor_colour_active(0xFF6bf3c6);
		self.editor_colour_circle(0xFF57cca5);
	}
	
	void on_add()
	{
		@text_trigger = entity_by_id(text_id);
	}
	
	void on_grave_dig_complete(string id, message@ msg)
	{
		state = GM_READY;
		grave_x = msg.get_float("grave_x");
		grave_y = msg.get_float("grave_y");
	}
	
	void activate(controllable@ e)
	{
		if(state != GM_READY) return;
		
		for(uint i = 0; i < num_cameras(); i++)
		{
			entity@ player = controller_entity(i);
			if(@player != null and e.is_same(player))
			{
				state = GM_TALKING;
				@text_trigger = update_text_trigger(g, text_trigger, "Oh thank you!");
				text_id = text_trigger.id();
				for(int j = 0; j < 2; j++)
				{
					audio@ a = g.play_script_stream("crying", 2, self.x(), self.y(), false, 1);
					a.positional(true);
				}
				return;
			}
		}
	}
	
	void step()
	{
		if(state == GM_TALKING)
		{
			if(talk_timer-- <= 0)
			{
				@text_trigger = entity_by_id(text_id);
				if(@text_trigger != null) g.remove_entity(text_trigger);
				facing = sgn(grave_x - self.x());
				state = GM_MOVING;
				
				script.activate_item("tear");
			}
		}
		if(state == GM_MOVING)
		{
			vehicle.arrive(grave_x, grave_y);
			vehicle.update();
			self.set_xy(vehicle.loc_x, vehicle.loc_y);
			
			if(abs(vehicle.loc_x - grave_x) < 1 and abs(vehicle.loc_y - grave_y) < 1)
			{
				state = GM_FADING;
				grave_y += 48 * 2;
			}
		}
		else if(state == GM_FADING)
		{
			if(--fade <= 0)
			{
				g.remove_entity(self.as_entity());
				return;
			}
			
			master_alpha = fade / fade_time;
			
			vehicle.vel_x *= 0.75;
			vehicle.arrive(grave_x, grave_y);
			vehicle.update();
			self.set_xy(vehicle.loc_x, vehicle.loc_y);
		}
		
		t++;
	}
	
	void editor_step()
	{
		t++;
	}
	
	void draw(float sub_frame)
	{
		const float x = self.x();
		const float y = self.y() - sin(t * DT) * 10;
		const int frame = int((t * DT * 2) % 7);
		const int palette = 0;
		float s = round(sin(t * DT * 0.5) * 3) / 3;
		float c = round(sin(t * DT * 1.5) * 3) / 3;
		if(s == 0) s = 1;
		if(c == 0) c = 1;
		const uint alpha1 = (uint(min(1, this.alpha * master_alpha * 1.4) * float(0xFF)) << 24) + 0xFFFFFF;
		const uint alpha2 = (uint(this.alpha * master_alpha * float(0xFF)) << 24) + 0xFFFFFF;
		const float p = 3;
		
		spr.draw_world(layer, sub_layer, "backdrops_3", 0, palette, x + 130, y, 90, 1, 1, (uint(master_alpha * float(0xFF)) << 24) + 0xFFFFFF);
		spr.draw_world(layer, sub_layer, "cidle", frame, palette, x, y, 0, facing, 1, alpha1);
		spr.draw_world(layer, sub_layer, "cidle", frame, palette, x - s * p, y - c * p, 0, facing, 1, alpha2);
		spr.draw_world(layer, sub_layer, "cidle", frame, palette, x + c * p, y + s * p, 0, facing, 1, alpha2);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}

class Grave : enemy_base, callback_base
{
	
	script@ script;
	scene@ g;
	scriptenemy@ self;
	
	[position,layer:19,y:tile_y] float tile_x = 0;
	[hidden] float tile_y = 0;
	[text] int width = 5;
	[text] int height = 5;
	[text] string event = "";
	
	Grave()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scriptenemy@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.on_hurt_callback(this, "on_hurt", 0);
		self.auto_physics(false);
		update_collision();
	}
	
	void update_collision(float sy=0)
	{
		const float tx = floor(tile_x / 48) * 48;
		const float ty = floor(tile_y / 48) * 48;
		const float x = tx - self.x();
		const float y = ty - self.y();
		self.base_rectangle(
			y + sy, y + height * 48,
			x, x + width * 48);
		self.hit_rectangle(
			y + sy, y + height * 48,
			x, x + width * 48);
	}
	
	void on_hurt(controllable@ attacked, controllable@ attacker, hitbox@ attack_hitbox, int arg)
	{
		const int tx = int(floor(tile_x / 48));
		const int ty = int(floor(tile_y / 48));
		bool complete = false;
		
		for(int iy = 0; iy < height; iy++)
		{
			for(int ix = 0; ix < width; ix++)
			{
				const int x = tx + ix;
				const int y = ty + iy;
				tileinfo@ tile = g.get_tile(x, y);
				if(tile.solid())
				{
					tile.solid(false);
					g.set_tile(x, y, 19, tile, true);
					
					if(ix == width - 1)
					{
						update_collision(iy * 48 + 48);
//						self.y(floor(tile_y / 48 + iy) * 48 + 48);
						
						if(iy == height - 1)
						{
							complete = true;
						}
					}
					
					iy = height;
					break;
				}
			}
		}
		
		if(complete)
		{
			message@ msg = create_message();
			msg.set_float("grave_x", self.x() + (width * 48) / 2);
			msg.set_float("grave_y", tile_y - 56);
			broadcast_message(event, msg);
			g.remove_entity(self.as_entity());
		}
	}
	
//	void draw(float sub_frame)
//	{
//		editor_draw(sub_frame);
//	}
	
	void editor_draw(float sub_frame)
	{
		const float tx = floor(tile_x / 48) * 48;
		const float ty = floor(tile_y / 48) * 48;
		g.draw_rectangle_world(21, 10, tx, ty, tx + width * 48, ty + height * 48, 0, 0x77FFFFFF);
		g.draw_rectangle_world(21, 11, tx - 5, ty - 5, tx + 5, ty + 5, 0, 0xAAFF0000);
	}
	
}



