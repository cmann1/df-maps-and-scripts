#include "../common/sprite_group.cpp"
#include "../common/Fx.cpp"

enum WitchesState
{
	
	WS_IDLE,
	WS_OPENING_DIALOG,
	WS_WAITING,
	WS_ACCEPT_DIALOG,
	WS_ACCEPT_ITEM,
	WS_CLOSING_DIALOG,
	WS_END,
	
}

class Witches : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	array<WitchDialog> dialog_list = {
		WitchDialog(WS_OPENING_DIALOG, 0, 30, "Look sisters! A visitor!"),
		WitchDialog(WS_OPENING_DIALOG, 2, 0, "Indeed, a visitor!"),
		WitchDialog(WS_OPENING_DIALOG, 1, 30, "How fortuitous!"),
		WitchDialog(WS_OPENING_DIALOG, 1, 30, ""),
		WitchDialog(WS_OPENING_DIALOG, 0, 0, "If you're interested"),
		WitchDialog(WS_OPENING_DIALOG, 2, 0, "We may be able to help each other."),
		WitchDialog(WS_OPENING_DIALOG, 1, 30, ""),
		WitchDialog(WS_OPENING_DIALOG, 0, 0, "We are in need of some items"),
		WitchDialog(WS_OPENING_DIALOG, 1, 0, "And we know what it is you seek"),
		WitchDialog(WS_OPENING_DIALOG, 2, 0, "For our eyes see far"),
		WitchDialog(WS_OPENING_DIALOG, 0, 0, "And little escapes our gaze."),
		WitchDialog(WS_OPENING_DIALOG, 1, 30, ""),
		WitchDialog(WS_OPENING_DIALOG, 0, 0, "Three items we seek to complete our spell:"),
		WitchDialog(WS_OPENING_DIALOG, 1, 0, "The feather of a crow."),
		WitchDialog(WS_OPENING_DIALOG, 2, 0, "The tears of a ghost."),
		WitchDialog(WS_OPENING_DIALOG, 0, 0, "And the head of a monster."),
		WitchDialog(WS_OPENING_DIALOG, 1, 30, ""),
		WitchDialog(WS_OPENING_DIALOG, 2, 30, "Help us"),
		WitchDialog(WS_OPENING_DIALOG, 1, 0, "And in return we will help you..."),
		
		WitchDialog(WS_ACCEPT_DIALOG, 1, 30, "", "feather"),
		WitchDialog(WS_ACCEPT_DIALOG, 1, 0, "A crow's feather."),
		WitchDialog(WS_ACCEPT_DIALOG, 2, 0, "In excellent condition.", "", true),
		
		WitchDialog(WS_ACCEPT_DIALOG, 1, 30, "", "pumpkin"),
		WitchDialog(WS_ACCEPT_DIALOG, 1, 0, "A monster's head."),
		WitchDialog(WS_ACCEPT_DIALOG, 0, 0, "Yes... This will do nicely.", "", true),
		
		WitchDialog(WS_ACCEPT_DIALOG, 2, 30, "", "tear"),
		WitchDialog(WS_ACCEPT_DIALOG, 2, 15, "A ghost's tears."),
		WitchDialog(WS_ACCEPT_DIALOG, 0, 0, "Absolutely beautiful!", "", true),
		
		WitchDialog(WS_CLOSING_DIALOG, 0, 40, "Finally!", "closing_dlg"),
		WitchDialog(WS_CLOSING_DIALOG, 2, 30, "It's done."),
		WitchDialog(WS_CLOSING_DIALOG, 0, 50, ""),
		WitchDialog(WS_CLOSING_DIALOG, 1, 0, "As agreed upon"),
		WitchDialog(WS_CLOSING_DIALOG, 2, 30, "The item you you so desperately desire..."),
		WitchDialog(WS_CLOSING_DIALOG, 0, 0, "") // << REQUIRED. Seems like there's a bug in angelscript which messes up the list item.
	};
	dictionary dialog_ids;
	
	[hidden]
	array<string> item_names = {"tear", "pumpkin", "feather"};
	dictionary item_sprites = {
		{"tear", "tear"}, {"pumpkin", "pumpkin_small"}, {"feather", "feather"}
	};
	dictionary item_glowing = {
		{"tear", true}, {"pumpkin", false}, {"feather", false}
	};
	
	[text] int layer = 17;
	[text] int sub_layer = 5;
	[text] int glow_sub_layer = 4;
	[text] float scale = 1.25;
	
	[position,layer:19,y:witch1_y] float witch1_x = 0;
	[hidden] float witch1_y = 0;
	[position,layer:19,y:witch2_y] float witch2_x = 0;
	[hidden] float witch2_y = 0;
	[position,layer:19,y:witch3_y] float witch3_x = 0;
	[hidden] float witch3_y = 0;
	[angle] float witch1_rotation = 0;
	[angle] float witch2_rotation = 0;
	[angle] float witch3_rotation = 0;
	
	[position,layer:19,y:witch1_dlg_y] float witch1_dlg_x = 0;
	[hidden] float witch1_dlg_y = 0;
	[position,layer:19,y:witch2_dlg_y] float witch2_dlg_x = 0;
	[hidden] float witch2_dlg_y = 0;
	[position,layer:19,y:witch3_dlg_y] float witch3_dlg_x = 0;
	[hidden] float witch3_dlg_y = 0;
	
	[entity] int apple_id;
	[position,layer:19,y:apple_drop_y] float apple_drop_x = 0;
	[hidden] float apple_drop_y = 0;
	
	[hidden] int state = WS_IDLE;
	[hidden] int dialog_index = -1;
	[hidden] int dialog_timer = 0;
	[hidden] bool dialog_complete = false;
	WitchDialog@ current_dialog = null;
	
	array<float> hat_xyr = {
		10, -82, -12,
		8,  -83, -10,
		4,  -81, -10,
		5,  -81, -10,
		8,  -83, -12,
		11, -81, -15,
		11, -81, -14
	};
	
	float t = 0;
	bool found_player = false;
	
	int end_laugh_timer = 75;
	
	sprites@ spr;
	sprite_group hat_spr;
	textfield@ dialog_txt;
	
	Witches()
	{
		@g = get_scene();
		@spr = create_sprites();
		
		@dialog_txt = create_textfield();
		dialog_txt.set_font("ProximaNovaReg", 26);
		dialog_txt.colour(0xFFb794ff);
		dialog_txt.align_horizontal(0);
		dialog_txt.align_vertical(0);
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		spr.add_sprite_set("maid");
		
		hat_spr.clear();
		hat_spr.add_sprite("props2", "boulders_6",
			0.5, 0.5,
			7, -25, 120,
			0.15, 0.15);
		hat_spr.add_sprite("props4", "backdrops_1",
			0.75, 0.5,
			0, 0, 120,
			0.8, 0.8);
		
		dialog_index = -1;
		dialog_complete = false;
		add_broadcast_receiver("player_taunt", this, "on_skip_dialog");
		
		for(int i = int(dialog_list.length() - 1); i >= 0; i--)
		{
			WitchDialog@ dlg = dialog_list[i];
			if(dlg.dlg_id != "")
			{
				dialog_ids[dlg.dlg_id] = i;
			}
		}
		
		add_broadcast_receiver("wi_drop_complete", this, "on_drop_complete");
	}
	
	void on_add()
	{
		if(!dialog_complete)
		{
			set_dialog(dialog_index);
		}
	}
	
	void on_skip_dialog(string id, message@ msg)
	{
		if(@current_dialog != null and current_dialog.state == state)
		{
			dialog_timer = current_dialog.time;
		}
	}
	
	void set_dialog(int index)
	{
		if(index >= 0 and index < int(dialog_list.length()))
		{
			dialog_index = index;
			@current_dialog = dialog_list[index];
			dialog_complete = false;
			dialog_txt.text(current_dialog.text);
		}
		else
		{
			dialog_index = -1;
			@current_dialog = null;
			dialog_complete = true;
			dialog_txt.text("");
		}
		
		dialog_timer = 0;
	}
	
	void on_drop_complete(string id, message@ msg)
	{
		if(item_names.length() == 0)
		{
			state = WS_CLOSING_DIALOG;
			set_dialog(int(dialog_ids["closing_dlg"]));
		}
		else
		{
			state = WS_WAITING;
		}
	}
	
	void activate(controllable@ e)
	{
		for(uint i = 0; i < num_cameras(); i++)
		{
			entity@ player = controller_entity(i);
			if(@player != null and e.is_same(player))
			{
				found_player = true;

				return;
			}
		}
	}
	
	void step()
	{
		if(found_player)
		{
			if(state == WS_IDLE)
			{
				state = WS_OPENING_DIALOG;
				set_dialog(0);
			}
			else if(state == WS_OPENING_DIALOG)
			{
				if(dialog_complete)
				{
					state = WS_WAITING;
				}
			}
			else if(state == WS_WAITING)
			{
				for(int i = int(item_names.length() - 1); i >= 0; i--)
				{
					const string name = item_names[i];
					if(script.persist_bool("witem_" +  name))
					{
						state = WS_ACCEPT_DIALOG;
						item_names.removeAt(i);
						set_dialog(int(dialog_ids[name]));
						
						message@ msg = create_message();
						msg.set_string("sprite", string(item_sprites[name]));
						msg.set_int("glowing", bool(item_glowing[name]) ? 1 : 0);
						broadcast_message("wi_init_drop", msg);
						
						break;
					}
				}
			}
			else if(state == WS_ACCEPT_DIALOG)
			{
				if(dialog_complete)
				{
					state = WS_ACCEPT_ITEM;
					broadcast_message("wi_start_drop", create_message());
					
				}
			}
			else if(state == WS_CLOSING_DIALOG)
			{
				if(dialog_complete)
				{
					state = WS_END;
					entity@ apple = entity_by_id(apple_id);
					if(@apple != null)
					{
						apple.set_xy(apple_drop_x, apple_drop_y);
						Fx@ fx = spawn_fx(apple_drop_x, apple_drop_y + 63, "editor", "respawnteam1");
						fx.set_layer(18, 6);
						g.play_sound("sfx_respawn", apple_drop_x, apple_drop_y, 1.0, false, true);
					}
				}
			}
			else if(state == WS_END)
			{
				if(end_laugh_timer-- == 0)
				{
					audio@ a = g.play_script_stream("witch_laugh", 2, self.x(), self.y(), false, 0.75);
					a.positional(true);
				}
			}
			
			if(!dialog_complete and @current_dialog != null)
			{
				if(current_dialog.state == state and dialog_timer++ >= current_dialog.time)
				{
					const bool force_complete = current_dialog.force_complete;
					set_dialog(++dialog_index);
					if(force_complete or @current_dialog == null or current_dialog.state != state)
					{
						dialog_complete = true;
					}
				}
			}
		}
		
		found_player = false;
		t++;
	}
	
	void editor_step()
	{
		t++;
	}
	
	void draw(float sub_frame)
	{
		const int palette = 0;
		const float glow_offx = -2;
		const float glow_offy = 1.5;
		
		for(int i = 0; i < 3; i++)
		{
			const int frame = int((t * 0.05 + i * 100) % 7);
			float x;
			float y;
			float rotation;
			
			if(i == 0)
			{
				x = witch1_x;
				y = witch1_y;
				rotation = witch1_rotation;
			}
			else if(i == 1)
			{
				x = witch2_x;
				y = witch2_y;
				rotation = witch2_rotation;
			}
			else
			{
				x = witch3_x;
				y = witch3_y;
				rotation = witch3_rotation;
			}
			
			spr.draw_world(layer, glow_sub_layer, "cidle", frame, palette, x + glow_offx, y + glow_offy, rotation, -scale, scale, 0xFFFFFFFF);
			spr.draw_world(layer, sub_layer, "cidle", frame, palette, x, y, rotation, -scale, scale, 0xFFFFFFFF);
			
			int ix = int(frame * 3);
			float hat_x = hat_xyr[ix] * -scale;
			float hat_y = hat_xyr[ix + 1] * scale;
			float hat_r = hat_xyr[ix + 2];
			rotate(hat_x, hat_y, rotation * DEG2RAD, hat_x, hat_y);
			hat_spr.draw(layer, glow_sub_layer, x + hat_x + glow_offx, y + hat_y + glow_offy, rotation + hat_r, 1);
			hat_spr.draw(layer, sub_layer, x + hat_x, y + hat_y, rotation + hat_r, 1);
		}
		
		if(!dialog_complete and @current_dialog != null)
		{
			const int witch_index = current_dialog.witch_index;
			float x;
			float y;
			
			if(witch_index == 0)
			{
				x = witch1_dlg_x;
				y = witch1_dlg_y;
			}
			else if(witch_index == 1)
			{
				x = witch2_dlg_x;
				y = witch2_dlg_y;
			}
			else
			{
				x = witch3_dlg_x;
				y = witch3_dlg_y;
			}
			
			shadowed_text_world(dialog_txt, 21, 19,
				x, y,
				1, 1, 0, 0xFF000000, 2, 2);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		draw_rect(g, witch1_dlg_x, witch1_dlg_y, 5, 5, 0, 21, 19, 0xFFFF0000);
		draw_rect(g, witch2_dlg_x, witch2_dlg_y, 5, 5, 0, 21, 19, 0xFF00FF00);
		draw_rect(g, witch3_dlg_x, witch3_dlg_y, 5, 5, 0, 21, 19, 0xFF0000FF);
		
		draw_rect(g, apple_drop_x, apple_drop_y, 10, 3, 0, 21, 19, 0xFF00FFFF);
		draw_rect(g, apple_drop_x, apple_drop_y, 3, 10, 0, 21, 19, 0xFF00FFFF);
	}
	
}

class WitchItemDropper : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	sprites@ spr;
	sprite_group glow_spr;
	float glow_alpha = 0;
	
	float t = 0;
	
	[text] int layer = 17;
	[text] int black_sub_layer = 0;
	[text] int glow_sub_layer = 1;
	
	[position,layer:19,y:drop_y] float drop_x = 0;
	[hidden] float drop_y = 0;
	
	[text] float gravity = 0.2;
	[text] float max_speed = 3;
	
	[hidden] float alpha = 0;
	[hidden] bool active = false;
	[hidden] bool dropping = false;
	[hidden] string sprite_name;
	[hidden] bool glowing;
	[hidden] float item_x = 0;
	[hidden] float item_y = 0;
	[hidden] float item_speed = 0;
	[hidden] float floating = 1;
	
	WitchItemDropper()
	{
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		spr.add_sprite_set("script");
		
		add_broadcast_receiver("wi_init_drop", this, "on_init_drop");
		add_broadcast_receiver("wi_start_drop", this, "on_start_drop");
		
		glow_spr.add_sprite("props3", "backdrops_3",
			0.5, 0.5,
			0, -50, 90,
			0.25, 0.25);
		glow_spr.add_sprite("props3", "backdrops_4",
			0.5, 1,
			6, 25, -17,
			1, 1);
		glow_spr.add_sprite("props3", "backdrops_4",
			0.5, 1,
			16, 5, -3,
			1, 1);
		glow_spr.add_sprite("props3", "backdrops_4",
			0.5, 1,
			20, 15, 15,
			1, 1);
	}
	
	void on_init_drop(string id, message@ msg)
	{
		sprite_name = msg.get_string("sprite");
		glowing = msg.get_int("glowing") != 0;
		active = true;
		item_x = self.x();
		item_y = self.y();
		alpha = 0;
		floating = 1;
		item_speed = 0;
		dropping = false;
	}
	
	void on_start_drop(string id, message@ msg)
	{
		dropping = true;
	}
	
	void step()
	{
		if(active)
		{
			if(alpha < 1)
			{
				alpha = min(1, alpha + DT);
			}
			
			if(dropping)
			{
				floating *= 0.95;
				item_speed = min(max_speed, item_speed + gravity);
				item_y += item_speed;
				
				if(item_y >= drop_y)
				{
					active = false;
					broadcast_message("wi_drop_complete", create_message());
					string sound1 = "sfx_slime_light_" + (rand() % 2 + 1);
					string sound2 = "sfx_slime_light_overlay_" + (rand() % 6 + 1);
					g.play_sound(sound2, drop_x, drop_y, 1.0, false, true);
					g.play_sound(sound2, drop_x, drop_y, 1.0, false, true);
					g.play_sound(sound2, drop_x, drop_y, 1.0, false, true);
					g.play_sound(sound1, drop_x, drop_y, 1.0, false, true);
					g.play_sound(sound1, drop_x, drop_y, 1.0, false, true);
					g.play_sound(sound1, drop_x, drop_y, 1.0, false, true);
					glow_alpha = 2;
				}
			}
		}
		
		if(glow_alpha != 0)
		{
			glow_alpha -= DT * 2;
			if(glow_alpha < 0) glow_alpha = 0;
			
			float alpha = glow_alpha;
			if(alpha > 1) alpha = 2 - alpha;
			for(int i = int(glow_spr.sprite_transforms.length() - 1); i >= 0; i--)
			{
				simple_transform@ tr = @glow_spr.sprite_transforms[i];
				tr.colour = (uint(alpha * float(0xFF)) << 24) + 0xFFFFFF;
			}
		}
		
		t += DT * 2;
	}
	
	void draw(float sub_frame)
	{
		if(active)
		{
			const int frame = 0;
			const int palette = 0;
			const float s = sin(t);
			spr.draw_world(layer, glowing ? glow_sub_layer : black_sub_layer, sprite_name, frame, palette,
				item_x,
				item_y - s * 14 * floating,
				0, 1, 1, (uint(alpha * float(0xFF)) << 24) + 0xFFFFFF);
		}
		
		if(glow_alpha != 0)
		{
			glow_spr.draw(layer, glow_sub_layer, drop_x, drop_y, 0, 1);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw_rect(g, drop_x, drop_y, 5, 5, 0, 21, 19, 0xFFFF0000);
	}
	
}

class WitchDialog
{
	
	string dlg_id;
	WitchesState state;
	int witch_index;
	int time;
	string text;
	bool force_complete;
	
	WitchDialog(){}
	
	WitchDialog(WitchesState state, int witch_index, int time, string text, string dlg_id="", bool force_complete=false)
	{
		this.dlg_id = dlg_id;
		this.state = state;
		this.witch_index = witch_index;
		this.time = text.length() * 8 + time;
		this.text = text;
		this.force_complete = force_complete;
	}
	
}

enum WitchesItemState
{
	WI_HIDDEN,
	WI_WATING, // Will start falling when activated
	WI_FALLING,
	WI_READY // Can be collected
}

class WitchesItem : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	[text] string sprite = "feather";
	[text] string name = "feather";
	[text] string display_text = "a feather";
	[text] int layer = 17;
	[text] int sub_layer = 19;
	[text] string glow_sprite = "";
	[text] int glow_layer = -1;
	[text] int glow_sub_layer = -1;
	[text] WitchesItemState state = WI_READY;
	[text] bool can_fall = false;
	[text] float fade_in = 0;
	[text] int wait_radius = 134;
	[text] int radius = 48;
	
	[position,layer:19,y:land_y] float land_x = 0;
	[hidden] float alpha = 1;
	[hidden] float land_y = 0;
	[hidden] float float_factor = 1;
	[hidden] float vel_y = 0;
	[text] float gravity = 5;
	[text] float vel_max = 5;
	[text] float sway_rotation = 0;
	[text] float sway_rotation_offset = 0;
	[text] float sway_x = 0;
	[text] float sway_y = 14;
	
	float t = 0;
	bool requires_init = true;
	
	sprites@ spr;
	
	WitchesItem()
	{
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		spr.add_sprite_set("script");
		self.vars().get_var("width").set_int32(state == WI_WATING ? wait_radius : radius);
		
		add_broadcast_receiver("activate_witches_item", this, "on_activate");
		
		self.editor_handle_size(4);
	}
	
	void on_activate(string id, message@ msg)
	{
		if(msg.get_string("item_name") == name)
		{
			set_ready();
			script.persist_bool("witem_active_" + name, true);
		}
	}
	
	void set_ready()
	{
		state = can_fall ? WI_FALLING : WI_READY;
		if(fade_in > 0) alpha = 0;
	}
	
	void activate(controllable@ e)
	{
		entity@ player = null;
		for(uint i = 0; i < num_cameras(); i++)
		{
			@player = controller_entity(i);
			if(@player == null or !e.is_same(player))
			{
				@player = null;
			}
		}
		
		if(state == WI_WATING)
		{
			if(can_fall)
			{
				set_ready();
				self.vars().get_var("width").set_int32(radius);
			}
		}
		else if(state == WI_READY)
		{
			script.display_text("You found " + display_text + ".");
			script.persist_bool("witem_" + name, true);
			g.remove_entity(self.as_entity());
		}
	}
	
	void step()
	{
		if(requires_init)
		{
			if(@script != null and script.persist_bool("witem_active_" + name))
			{
				set_ready();
			}
			
			requires_init = false;
		}
		
		if(state == WI_FALLING)
		{
			vel_y += gravity * DT;
			if(vel_y > vel_max) vel_y = vel_max;
			
			self.y(self.y() + vel_y);
			
			if(self.y() >= land_y - 5)
			{
				self.y(land_y);
				state = WI_READY;
			}
			if(fade_in > 0) alpha = min(1, alpha + DT * fade_in);
		}
		else if(state == WI_READY)
		{
			float_factor *= 0.95;
			alpha = 1;
		}
		
		t += DT * 2;
	}
	
	void draw_sprite(int layer, int sub_layer)
	{
		const int frame = 0;
		const int palette = 0;
		const float s = sin(t);
		const float x = self.x() + cos(t * 1.5) * sway_x * float_factor;
		const float y = self.y() - (s * sway_y) * (1 - float_factor);
		const float rotation = s * sway_rotation * float_factor + sway_rotation_offset * float_factor;
		const uint colour = (uint(alpha * float(0xFF)) << 24) + 0xFFFFFF;
		
		if(glow_layer != -1)
		{
			spr.draw_world(glow_layer, glow_sub_layer, glow_sprite, frame, palette,
				x, y, rotation,
				1, 1, colour);
		}
		
		spr.draw_world(layer, sub_layer, sprite, frame, palette,
			x, y, rotation,
			1, 1, colour);
	}
	
	void draw(float sub_frame)
	{
		if(state != WI_HIDDEN and state != WI_WATING)
		{
			draw_sprite(layer, sub_layer);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw_sprite(layer, sub_layer);
		
		if(can_fall)
		{
			g.draw_rectangle_world(22, 10, land_x - 5, land_y - 5, land_x + 5, land_y + 5, 0, 0xAAFF0000);
		}
	}
	
}

class HangingSkulls : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	sprites@ spr;
	
	[text] int count = 2;
	
	float t = 0;
	
	HangingSkulls()
	{
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		spr.add_sprite_set("script");
		self.editor_handle_size(4);
	}
	
	void step()
	{
		t++;
	}
	void editor_step()
	{
		t++;
	}
	
	void draw(float sub_frame)
	{
		const int frame = 0;
		const int palette = 0;
		float x = self.x();
		float y = self.y();
		float rotation = (x * 50 + y * 50) % 360;
		
		const float wind_speed = 0.05;
		const float wind_direction = 1;
		const float wind_strength = 7;
		
		for(int i = 0; i < count; i++)
		{
			const float pr = (sin(x + y + i) * 0.25 + 1) * wind_speed;
			const float s = sin(t * pr + (x + y + i) * 0.1f);
			const float wind = s * wind_direction * wind_strength;
			
			spr.draw_world(15, 19, "skull", frame, palette, x + s * 1, y, rotation + wind, 1, 1, 0xFFFFFFFF);
			y += 36 + (y % 16);
			rotation += ((x * y) % 180 - 90);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}
















