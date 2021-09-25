#include '../lib/std.cpp';
#include '../lib/drawing/common.cpp';
#include '../lib/drawing/Sprite.cpp';
#include '../lib/emitters/common.cpp';
#include '../lib/math/math.cpp';

#include 'IInitHandler.cpp';
#include 'IPreStepHandler.cpp';
#include 'BurstEmitter.cpp';
#include 'Messages.cpp';
#include 'Player.cpp';

class BaseScript
{
	
	scene@ g;
	/// Global reusable raycast result
	raycast@ ray;
	camera@ cam;
	camera@ active_cam;
	
	int num_players;
	array<Player> players;
	
	int collision_layer = 19;
	bool is_playing;
	int frame;
	
	float ed_zoom;
	float ed_view_x;
	float ed_view_y;
	float ed_view_x1, ed_view_y1;
	float ed_view_x2, ed_view_y2;
	
	array<BurstEmitter@> burst_emitters;
	
	private int size_init_listeners = 8;
	private int num_init_listeners;
	private array<IInitHandler@> init_listeners(size_init_listeners);
	
	private int size_pre_step_listeners = 8;
	private int num_pre_step_listeners;
	private array<IPreStepHandler@> pre_step_listeners(size_pre_step_listeners);
	
	Messages messages;
	
	BaseScript()
	{
		@g = get_scene();
		@ray = g.ray_cast_tiles_ex(0, 0, 1, 0, 19, ray);
		collision_layer = g.default_collision_layer();
		@cam = get_active_camera();
		
		num_players = num_cameras();
		players.resize(num_players);
		
		for(int i = 0; i < num_players; i++)
		{
			players[i].init(this, i);
		}
	}
	
	void on_level_start()
	{
		is_playing = true;
	}
	
	void checkpoint_load()
	{
		num_pre_step_listeners = 0;
		burst_emitters.resize(0);
		
		messages.clear();
	}
	
	void step(int num_entities)
	{
		ed_zoom = 1 / (1080 / cam.screen_height());
		
		for(int i = int(burst_emitters.length - 1); i >= 0; i--)
		{
			if(!burst_emitters[i].step(this))
			{
				burst_emitters.removeAt(i);
			}
		}
		
		frame++;
		
		if(is_playing)
		{
			for(int i = num_pre_step_listeners - 1; i >= 0; i--)
			{
				pre_step_listeners[i].pre_step();
			}
		}
	}
	
	void step_post(int num_entities)
	{
		run_init();
		
		for(int i = 0; i < num_players; i++)
		{
			players[i].step_post();
		}
	}
	
	void editor_step()
	{
		run_init();
		
		step(0);
		
		ed_view_x = cam.x();
		ed_view_y = cam.y();
		ed_zoom = 1 / cam.editor_zoom();
		
		float view1_x, view1_y, view1_w, view1_h;
		float view2_x, view2_y, view2_w, view2_h;
		cam.get_layer_draw_rect(0, 19, view1_x, view1_y, view1_w, view1_h);
		cam.get_layer_draw_rect(1, 19, view2_x, view2_y, view2_w, view2_h);
		ed_view_x1 = min(view1_x, view2_x);
		ed_view_y1 = min(view1_y, view2_y);
		ed_view_x2 = max(view1_x + view1_w, view2_x + view2_w);
		ed_view_y2 = max(view1_y + view1_h, view2_y + view2_h);
		
		players[0].step_post();
	}
	
	//void draw(float sub_frame)
	//{
	//	Player@ p = players[active_player];
	//	p.draw(sub_frame);
	//}
	
	//void editor_draw(float sub_frame)
	//{
	//	Player@ p = players[active_player];
	//	p.draw(sub_frame);
	//}
	
	bool in_view(float x1, float y1, float x2, float y2, const float padding=100)
	{
		x1 -= padding;
		y1 -= padding;
		x2 += padding;
		y2 += padding;
		
		for(int i = 0; i < num_players; i++)
		{
			Player@ p = players[i];
			if(x1 <= p.x2 && x2 >= p.x1 && y1 <= p.y2 && y2 >= p.y1)
				return true;
		}
		
		return false;
	}
	
	bool should_draw(const float x1, const float y1, const float x2, const float y2)
	{
		Player@ p = players[active_player];
		return x1 <= p.x2 && x2 >= p.x1 && y1 <= p.y2 && y2 >= p.y1;
	}
	
	/**
	 * Takes a position in **from_layer** coordinates and returns the matching
	 * position in **to_layer** based on the view/camera position.
	 */
	void layer_position(float x, float y, int from_layer, int to_layer, float &out out_x, float &out out_y)
	{
		const float scale = g.layer_scale(from_layer) / g.layer_scale(to_layer);
		
		float dx = (x - ed_view_x) * scale;
		float dy = (y - ed_view_y) * scale;
		
		out_x = ed_view_x + dx;
		out_y = ed_view_y + dy;
	}
	
	BurstEmitter@ add_emitter_burst(
		const int emitter_id,
		const int layer, const int sub_layer,
		const float x, const float y,
		const float size_x, const float size_y, float rotation, const float time=0.25, const int density=1)
	{
		BurstEmitter@ burst = BurstEmitter(time);
		burst_emitters.insertLast(burst);
		
		for(int i = 0; i < density; i++)
		{
			entity@ e = create_emitter(emitter_id,
				x, y,
				int(size_x), int(size_y),
				layer, sub_layer, int(rotation)
			);
			burst.emitters.insertLast(e);
			g.add_entity(e, false);
		}
		
		return burst;
	}
	
	void add_screen_shake(float x, float y, float dir, float force)
	{
		for(int i = 0; i < num_players; i++)
		{
			camera@ cam = get_camera(i);
			cam.add_screen_shake(x, y, dir, force);
		}
	}
	
	void pre_step_subscribe(IPreStepHandler@ listener)
	{
		if(num_pre_step_listeners + 1 >= size_pre_step_listeners)
		{
			pre_step_listeners.resize(size_pre_step_listeners *= 2);
		}
		
		@pre_step_listeners[num_pre_step_listeners++] = listener;
	}
	
	void pre_step_unsubscribe(IPreStepHandler@ listener)
	{
		int index = pre_step_listeners.findByRef(listener);
				
		if(index >= 0 && index < num_pre_step_listeners)
		{
			@pre_step_listeners[index] = @pre_step_listeners[--num_pre_step_listeners];
		}
	}
	
	void init_subscribe(IInitHandler@ listener)
	{
		if(num_init_listeners + 1 >= size_init_listeners)
		{
			init_listeners.resize(size_init_listeners *= 2);
		}
		
		@init_listeners[num_init_listeners++] = listener;
	}
	
	private void run_init()
	{
		if(num_init_listeners <= 0)
			return;
		
		for(int i = 0; i < num_init_listeners; i++)
		{
			init_listeners[i].init();
		}
		
		num_init_listeners = 0;
	}
	
}
