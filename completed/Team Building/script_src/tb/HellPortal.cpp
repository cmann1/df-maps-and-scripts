#include "../common/Sprite.cpp"

class HellPortal : trigger_base, callback_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	sprites@ spr;
	Sprite glow1_spr("props3", "backdrops_3", 0.5, 0.5);
	Sprite glow2_spr("props3", "backdrops_3", 0.25, 0.5);
	
	[text] string name = "default";
	[text] int apple_count = 5;
	[position,layer:19,y:hell_y] float hell_x = 0;
	[hidden] float hell_y = 0;
	[position,layer:19,y:hell_cam_y] float hell_cam_x = 0;
	[hidden] float hell_cam_y = 0;
	[position,layer:19,y:interm_y] float interm_x = 0;
	[hidden] float interm_y = 0;
	[position,layer:19,y:interm2_y] float interm2_x = 0;
	[hidden] float interm2_y = 0;
	
	[text] float scale = 1.5;
	[text] int layer = 16;
	[text] int sub_layer = 6;
	[text] int active_layer = 14;
	[text] int active_sub_layer = 7;
	[text] int glow_sub_layer = 5;
	
	[text] float pulse_speed = 0.25;
	[text] float pulse_size = 12;
	
	[text] float act_duration = 20;
	[hidden] float act_time = 0;
	
	[text] float rim_radius = 180;
	[text] int rim_e_count = 26;
	[text] int rim_e_size = 30;
	[text] int rim_e_layer = 16;
	[text] int rim_e_sub_layer = 12;
	
	[colour] uint fade_colour = 0x00FFFFFF;
	[text] float fade_time = 60;
	
	[text] bool active = false;
	
	bool has_emitters = false;
	array<entity@> emitters;
	array<entity@> rim_emitters;
	
	array<controllable@> teleport_queue;
	array<float> teleport_queue_fade;
	
	float t = 0;
	
	float act_emitter_timer;
	entity@ act_emitter_1 = null;
	entity@ act_emitter_2 = null;
	entity@ act_emitter_3 = null;
	entity@ act_emitter_4 = null;
	
	HellPortal()
	{
		@g = get_scene();
		@spr = create_sprites();
		spr.add_sprite_set("script");
		
		add_broadcast_receiver("portal_apple_count", this, "portal_apple_count");
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.self = @self;
		self.editor_handle_size(6);
		
		check_apple_count(script.ge_apple_count(name));
		teleport_queue.resize(num_cameras());
		teleport_queue_fade.resize(num_cameras());
	}
	
	void portal_apple_count(string id, message@ msg)
	{
		if(msg.get_string("portal") == name)
		{
			check_apple_count(msg.get_int("count"));
		}
	}
	
	void check_apple_count(int count)
	{
		active = count == apple_count;
	}
	
	void activate(controllable@ e)
	{
		if(active)
		{
			for(int i = int(num_cameras()) - 1; i >= 0; i--)
			{
				if(controller_controllable(i).is_same(e))
				{
					if(@teleport_queue[i] == null)
					{
						e.time_warp(0.1);
						teleport_queue_fade[i] = fade_time;
						@teleport_queue[i] = e;
					}
					return;
				}
			}
		}
	}
	
	void step()
	{
		t += pulse_speed;
		
		if(active and !has_emitters)
		{
			array<float> epos = {-96, -96, 96, -96, 96, 96, -96, 96, 0, 0};
			for(int i = 0; i < 5; i++)
			{
				entity@ emitter = create_emitter(4,
					self.x() + epos[i * 2], self.y() + epos[i * 2 + 1],
					120, 120, 16, 12);
				g.add_entity(emitter, false);
				emitters.insertLast(emitter);
			}
			
			float angle = 0;
			const float da = PI * 2 / rim_e_count;
			for(int i = 0; i < rim_e_count; i++)
			{
				const float x = self.x() + cos(angle) * rim_radius;
				const float y = self.y() + sin(angle) * rim_radius;
				
				for(int j = 0; j < 2; j++)
				{
					entity@ emitter = create_emitter(125, x, y, rim_e_size / (j + 1), rim_e_size / (j + 1), rim_e_layer, rim_e_sub_layer + j);
					g.add_entity(emitter, false);
					rim_emitters.insertLast(emitter);
				}
				
				angle += da;
			}
			
			if(act_emitter_1 is null)
			{
				@act_emitter_1 = create_emitter(122, self.x() - rim_radius * 0.75, self.y(), 50, int(rim_radius), rim_e_layer, rim_e_sub_layer);
				g.add_entity(act_emitter_1, false);
				@act_emitter_2 = create_emitter(122, self.x() + rim_radius * 0.75, self.y(), 50, int(rim_radius), rim_e_layer, rim_e_sub_layer);
				g.add_entity(act_emitter_2, false);
				@act_emitter_3 = create_emitter(122, self.x(), self.y() - rim_radius * 0.75, int(rim_radius), 50, rim_e_layer, rim_e_sub_layer);
				g.add_entity(act_emitter_3, false);
				@act_emitter_4 = create_emitter(122, self.x(), self.y() + rim_radius * 0.75, int(rim_radius), 50, rim_e_layer, rim_e_sub_layer);
				g.add_entity(act_emitter_4, false);
			}
			act_emitter_timer = t + 10;
			
			has_emitters = true;
		}
		else if(!active and has_emitters)
		{
			for(int i = 0; i < 5; i++)
			{
				g.remove_entity(@emitters[i]);
			}
			for(int i = int(rim_emitters.length()) - 1; i >= 0; i--)
			{
				g.remove_entity(@rim_emitters[i]);
			}
			
			emitters.resize(0);
			rim_emitters.resize(0);
			has_emitters = false;
		}
		
		if(active)
		{
			for(int i = int(num_cameras()) - 1; i >= 0; i--)
			{
				controllable@ e = teleport_queue[i];
				if(@e != null)
				{
					const float fade = teleport_queue_fade[i]--;
					
					// Finally start moving
					if(fade < -20)
					{
						e.set_speed_xy(0, 0);
						e.state(0);
						e.attack_state(0);
						
						// Don't move the player isntantly otherwise ztriggers don't activate
						PlayerMover@ pm = PlayerMover();
						pm.set(e, i, hell_x, hell_y, hell_cam_x, hell_cam_y);
						scripttrigger@ st = create_scripttrigger(pm);
						st.set_xy(e.x(), e.y());
						g.add_entity(st.as_entity(), false);
						
						@teleport_queue[i] = null;
					}
					else if(fade == 0)
					{
						// First move the player down to activate the fog trigger
						if(fade == 0)
						{
							e.set_xy(interm_x, interm_y);
							reset_camera(i);
							e.set_speed_xy(0, 0);
						}
					}
					else if(fade == -10)
					{
						// Then move the player down to detach from the camera
						e.set_xy(interm2_x, interm2_y);
					}
					
					e.light_intent(0);
					e.heavy_intent(0);
					e.jump_intent(0);
					e.dash_intent(0);
					e.x_intent(0);
					e.y_intent(0);
					e.taunt_intent(0);
					e.fall_intent(0);
				}
			}
			
			if(act_time < act_duration) act_time++;
		}
		else
		{
			if(act_time > 0) act_time--;
		}
		
		if(@act_emitter_1 != null and t >= act_emitter_timer)
		{
			g.remove_entity(act_emitter_1);
			g.remove_entity(act_emitter_2);
			g.remove_entity(act_emitter_3);
			g.remove_entity(act_emitter_4);
			@act_emitter_1 = null;
			@act_emitter_2 = null;
			@act_emitter_3 = null;
			@act_emitter_4 = null;
		}
	}
	
	void editor_step()
	{
		t += pulse_speed;
	}
	
	void draw(float sub_frame)
	{
		const float x = self.x();
		const float y = self.y();
		
		const float act_t = act_time / act_duration;
		const uint act_alpha = uint(act_t * 255);
		const uint act_colour = (act_alpha << 24) + 0xFFFFFF;
		
		if(active) spr.draw_world(active_layer, glow_sub_layer, "pent_glow", 0, 0, x, y, 0, scale, scale, (act_alpha << 24) + 0xFFFFFF);
		
		if(act_t < 1) spr.draw_world(layer, sub_layer, "pentagram", 0, 0, x, y, 0, scale, scale, ((255 - act_alpha) << 24) + 0xFFFFFF);
		if(act_t > 0)
		{
			
		}
		
		if(active)
		{
			const float s = scale + sin(t * 0.035) * 0.1;
			spr.draw_world(active_layer, active_sub_layer, "pentagram", 0, 0, x, y, 0, s, s, act_colour);
			
			const uint jitter_col = (uint(act_t * 0x44) << 24) + 0xFFFFFF;
		
			glow1_spr.draw_world(active_layer, glow_sub_layer, 0, 0, x, y, 45, 1, 1, act_colour);
			glow1_spr.draw_world(active_layer, glow_sub_layer, 0, 0, x, y, -45, 1, 1, act_colour);
			glow2_spr.draw_world(active_layer, glow_sub_layer, 0, 0, x, y, 90, 1, 1, act_colour);
			
			spr.draw_world(active_layer, active_sub_layer, "pentagram", 0, 0, x + round(sin(t)) * pulse_size, y + round(sin(t * 2)) * pulse_size, 0, s, s, jitter_col);
			spr.draw_world(active_layer, active_sub_layer, "pentagram", 0, 0, x + round(sin(t * 2 + 50)) * pulse_size, y + round(sin(t + 4)) * pulse_size, 0, s, s, jitter_col);
			
			// Player fade
			const int player_index = get_active_player();
			controllable@ e = teleport_queue[player_index];
			if(@e != null)
			{
				const uint alpha = uint(255 - max(0, teleport_queue_fade[player_index] / fade_time) * 255) << 24;
				g.draw_rectangle_hud(10, 10, -900, -550, 900, 550, 0, alpha + (fade_colour & 0x00FFFFFF));
			}
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		g.draw_rectangle_world(21, 19, hell_x - 4, hell_y - 4, hell_x + 4, hell_y + 4, 0, 0xFFFF0000);
		g.draw_rectangle_world(21, 19, hell_cam_x - 4, hell_cam_y - 4, hell_cam_x + 4, hell_cam_y + 4, 0, 0xFFFFFF00);
		g.draw_rectangle_world(21, 19, interm_x - 4, interm_y - 4, interm_x + 4, interm_y + 4, 0, 0xFFFF0000);
		g.draw_rectangle_world(21, 19, interm2_x - 4, interm2_y - 4, interm2_x + 4, interm2_y + 4, 0, 0xFF00FF00);
	}
	
}