#include "../common/Sprite.cpp"
#include "../common/Fx.cpp"
#include "Jack.cpp"

const float MIN_SOUND_DISTANCE = 48 * 14;
const float MAX_SOUND_DISTANCE = 48 * 24;

class script{}

enum JackState
{
	Sleep,
	WakeUp,
	OpenDialog,
	Idle,
}

class Jack2 : Jack, callback_base
{
	
	Sprite head_glow_spr("props1", "backdrops_4", 0.5, 0.68);
	sprites@ head_spr;
	
	[hidden] JackState state = Sleep;
	[hidden] int sacriface_count = -1;
	
	array<JackDialog@> opening_dialog = {
		JackDialog(4.5, "It would seem I've caught an intruder;\nnot many would dare enter my lair uninvited."),
		JackDialog(0.5, "Hmm"),
		JackDialog(0.5, "Hmm."),
		JackDialog(0.5, "Hmm.."),
		JackDialog(0.5, "Hmm..."),
		JackDialog(0.5, "Hmm...."),
		JackDialog(0.5, "Hmm....."),
		JackDialog(4, "Perhaps we can come to some sort of agreement."),
		JackDialog(6.0, "Bring me three sacrifices to satiate my hunger,\nand in exchange I will let you live.")
	};
	array<JackDialog@> closing_dialog = {
		JackDialog(6, "You've done very well - I'm feeling generous tonight so\nI'll give you a reward for your hard work."),
		JackDialog(9, "Atop the tallest building in the cemetery you'll find a very\nvaluable item. It may be of some use to you in the future.")
	};
	textfield@ dialog_txt;
	[hidden] array<JackDialog@>@ current_dialog = null;
	[hidden] int current_dialog_index = -1;
	[hidden] int dialog_stage = -1;
	[hidden] int dialog_timer = 0;
	
	[position,layer:19,y:dialog_loc_y] float dialog_loc_x = 0;
	[hidden] float dialog_loc_y = 0;
	
	bool found_player = false;
	
	Jack2()
	{
		super();
		@head_spr = create_sprites();
		@dialog_txt = create_textfield();
		dialog_txt.set_font("ProximaNovaReg", 26);
		dialog_txt.colour(0xFFffe1bd);
		dialog_txt.align_horizontal(-1);
		dialog_txt.align_vertical(-1);
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		Jack::init(script, self);
		
		head_spr.add_sprite_set("script");
		@event_callback = EventCallback(this.on_anim_event);
		
		if(state == Sleep)
		{
			set_animation("Sleep");
		}
		else if(state == WakeUp)
		{
			set_animation("WakeUp");
		}
		else if(state == OpenDialog or state == Idle)
		{
			set_animation("Idle");
		}
		
		if(current_dialog_index == 0)
			@current_dialog = @opening_dialog;
		else if(current_dialog_index == 1)
			@current_dialog = @closing_dialog;
		
		add_broadcast_receiver("jack_sacrifice", this, "on_sacrifice");
		add_broadcast_receiver("player_taunt", this, "on_skip_dialog");
	}
	
	void on_skip_dialog(string id, message@ msg)
	{
		if(@current_dialog != null and dialog_stage != -1)
		{
			dialog_timer = current_dialog[dialog_stage].time;
		}
	}
	
	void on_sacrifice(string id, message@ msg)
	{
		if(sacriface_count == -1) return;
		
		entity@ e = msg.get_entity("apple");
		Fx@ fx = spawn_fx(e.x(), e.y() + 63, "editor", "respawnteam1");
		fx.set_layer(18, 6);
		g.play_sound("sfx_damage_spikes", e.x(), e.y(), 1.0, false, true);
		g.remove_entity(e);
		
		if(--sacriface_count <= 0)
		{
			dialog_stage = 0;
			@current_dialog = @closing_dialog;
			current_dialog_index = 1;
			
			message@ msg1 = create_message();
			msg1.set_string("item_name", "pumpkin");
			broadcast_message("activate_item", msg1);
		}
	}
	
	void step()
	{
		Jack::step();
		
		if(found_player)
		{
			if(state == Sleep)
			{
				self.vars().get_var("width").set_int32(820);
				state = WakeUp;
				set_animation("WakeUp");
				
				message@ msg = create_message();
				msg.set_string("sound", "monster_snarl");
				msg.set_float("x", self.x());
				msg.set_float("y", self.y());
				msg.set_int("loop", 1);
				msg.set_float("volume", 0.65);
				msg.set_float("min" ,MIN_SOUND_DISTANCE);
				msg.set_float("max", MAX_SOUND_DISTANCE);
				broadcast_message("play_sound_2d", msg);
				
//				on_anim_event("wake_up_complete");
			}
			
			if(dialog_stage != -1)
			{
//				if(dialog_timer++ > 10)
				if(dialog_timer++ >= current_dialog[dialog_stage].time)
				{
					dialog_timer = 0;
					if(++dialog_stage >= int(current_dialog.length()))
					{
						@current_dialog = null;
						current_dialog_index = -1;
						dialog_stage = -1;
						
						if(state == OpenDialog)
						{
							sacriface_count = 3;
							state = Idle;
						}
					}
				}
			}
		}
		
		found_player = false;
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
	
	void on_anim_event(string event)
	{
		if(event == "wake_up_complete")
		{
			set_animation("Idle");
			state = OpenDialog;
			
			dialog_stage = 0;
			@current_dialog = @opening_dialog;
			current_dialog_index = 0;
		}
		
		else if(event == "footstep")
		{
			const string sfx = "sfx_footstep_dirt_" + (rand() % 3 + 1);
			g.play_sound(sfx, self.x(), self.y(), 1, false, true);
			g.play_sound(sfx, self.x(), self.y(), 1, false, true);
		}
	}
	
	void draw(float sub_frame)
	{
		Jack::draw(sub_frame);
		const int frame = 0;
		const int palette = 0;
		const int layer = 17;
		const int sub_layer = 19;
		float head_x;
		float head_y;
		float head_rot;
		get_anchor_pos("head", head_x, head_y);
		get_anchor_rot("head", head_rot);
		head_x += self.x();
		head_y += self.y();
		head_rot *= RAD2DEG;
		head_glow_spr.draw_world(layer, 18, frame, palette, head_x, head_y, head_rot);
		head_spr.draw_world(layer, sub_layer, "pumpkin", frame, palette, head_x, head_y, head_rot, 1, 1, 0xFFFFFFFF);
		
		if(dialog_stage != -1)
		{
			dialog_txt.text(current_dialog[dialog_stage].text);
			dialog_txt.draw_world(21, 19, dialog_loc_x, dialog_loc_y, 1, 1, 0);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		g.draw_rectangle_world(22, 10, dialog_loc_x - 5, dialog_loc_y - 5, dialog_loc_x + 5, dialog_loc_y + 5, 0, 0xAA0000FF);
	}
	
}

class JackDialog
{
	int time;
	string text;
	
	JackDialog(float seconds, string text)
	{
		this.time = int(seconds * 60.0);
		this.text = text;
	}
	
}

class JackSacrifice : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	JackSacrifice()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
	}
	
	void activate(controllable@ e)
	{
		if(e.type_name() == "hittable_apple")
		{
			message@ msg = create_message();
			msg.set_entity("apple", e.as_entity());
			broadcast_message("jack_sacrifice", msg);
		}
	}
}