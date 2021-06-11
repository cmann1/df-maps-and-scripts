#include "../common/utils.cpp"
#include "../common/SpriteRenderer.cpp"
#include "HangingSkeleton.cpp"
#include "Pumpkin.cpp"
#include "Caustics.cpp"
#include "Bat.cpp"
#include "Crow.cpp"
#include "Ghost.cpp"
#include "Clouds.cpp"
#include "PropsWind.cpp"
#include "EntityOutliner.cpp"
#include "SecretTrigger.cpp"
#include "SoundRepeater.cpp"

//#include "Jack.cpp"
//#include "Jack2.cpp"
#include "Graveyard.cpp"
#include "Witches.cpp"

const float MIN_SOUND_DISTANCE = 48 * 14;
const float MAX_SOUND_DISTANCE = 48 * 24;

class script : callback_base
{
	
	scene@ g;
	array<controllable@> players;
	array<bool> players_enabled;
	int num_players;
	
	Clouds clouds;
	PropsWind props_wind;
	EntityOutliner entity_outliner;
	
	dictionary persist_vars;
	dictionary persist_vars_checkpoint;
	
	textfield@ onscreen_tf;
	string onscreen_text = "";
	int onscreen_text_timer_max = 340;
	int onscreen_text_fade_time = 40;
	int onscreen_text_timer;
	
	script()
	{
		@g = get_scene();
		g.override_stream_sizes(12, 8);
		
		@onscreen_tf = create_textfield();
		onscreen_tf.set_font("ProximaNovaReg", 36);
		onscreen_tf.align_horizontal(0);
		onscreen_tf.align_vertical(0);
		
		num_players = num_cameras();
		players.resize(num_players);
		players_enabled.resize(num_players);
				
		for(int i = 0; i < num_players; i++)
		{
			players_enabled[i] = true;
		}
		
		add_broadcast_receiver("activate_item", this, "on_activate_item");
		add_broadcast_receiver("play_sound_2d", this, "on_play_sound_2d");
	}
	
	void on_level_start()
	{
		audio@ a = g.play_script_stream("ambience", 2, 3785, 1, true, 1.0);
	}
	
	void checkpoint_load()
	{
		for(int i = 0; i < num_players; i++)
		{
			@players[i] = null;
		}
		
		persist_vars = persist_vars_checkpoint;
		props_wind.checkpoint_load();
//		broadcast_message("checkpoint_load", create_message());
	}
	
	void checkpoint_save()
	{
		persist_vars_checkpoint = persist_vars;
	}
	
	void display_text(string txt)
	{
		onscreen_tf.text(onscreen_text = txt);
		onscreen_text_timer = onscreen_text_timer_max;
	}
	
	void persist_bool(string name, bool value){ persist_vars[name] = value; }
	bool persist_bool(string name){ return persist_vars.exists(name) ? bool(persist_vars[name]) : false; }
	
	void on_activate_item(string id, message@ msg)
	{
		activate_item(msg.get_string("item_name"));
	}
	
	void activate_item(string name)
	{
		persist_bool("witem_active_" + name, true);
		message@ msg = create_message();
		msg.set_string("item_name", name);
		broadcast_message("activate_witches_item", msg);
	}
	
	void step(int entities)
	{
		controllable@ player;
		for(int i = 0; i < num_players; i++)
		{
			if((@player = players[i]) != null)
			{
				props_wind.player(player);
				
				if(!players_enabled[i])
				{
					player.jump_intent(0);
					player.heavy_intent(0);
					player.light_intent(0);
					player.dash_intent(0);
					player.taunt_intent(0);
					player.y_intent(0);
					player.x_intent(0);
				}
				else
				{
					if(player.taunt_intent() != 0)
					{
						broadcast_message("player_taunt", create_message());
					}
				}
			}
			else
			{
				entity@ e = controller_entity(i);
				@players[i] = @player = (@e != null ? e.as_controllable() : null);
				if(@player != null)
				{
					dustman@ d = player.as_dustman();
				}
			}
		}
		
		entity_outliner.step(entities);
		clouds.step();
		props_wind.step();
		
		if(onscreen_text_timer > 0)
		{
			float alpha = 1;
			if(onscreen_text_timer <= onscreen_text_fade_time)
				alpha = onscreen_text_timer / float(onscreen_text_fade_time);
			else if(onscreen_text_timer >= onscreen_text_timer_max - onscreen_text_fade_time)
				alpha = (onscreen_text_timer_max - onscreen_text_timer) / float(onscreen_text_fade_time);
			uint calpha = uint(alpha * float(0xFF)) << 24;
//			shadowed_text_hud(onscreen_tf, 10, 10, 0, SCREEN_TOP + 10, 1, 1, 0, 0xFF000000, 2, 2);
			onscreen_tf.colour(calpha + 0xfc971f);
			shadowed_text_hud(onscreen_tf, 10, 10, 0, -100, 1, 1, 0, calpha + 0x000000, 2, 2);
			
			onscreen_text_timer--;
		}
	}
	
	void draw(float sub_frame)
	{
		entity_outliner.draw(sub_frame);
		clouds.draw(sub_frame);
	}
	
	void editor_step()
	{
		clouds.step();
	}
	
	void editor_draw(float sub_frame)
	{
		clouds.draw(sub_frame);
	}
	
}