#include '../lib/easing/cubic.cpp';
#include '../lib/enums/AttackType.cpp';
#include '../lib/enums/EntityState.cpp';

namespace SnakeIntro
{
	
	const int StateIdle					= 0;
	const int StateWaitingPlayerIdle	= 1;
	const int StateEnter				= 2;
	const int StateIntro				= 3;
	const int StateIntroResponse		= 4;
	const int StateIntroRejected		= 5;
	const int StateIntroAccepted		= 6;
	const int StateIntroAcceptedExit	= 7;
	
	const int StatePendingUnlock		= 8;
	const int StateUnlocked				= 9;
	const int StateUnlockedWaiting		= 10;
	const int StateUnlockedDialog		= 11;
	const int StateUnlockedExit			= 12;
	const int StateOutroPending			= 13;
	const int StateOutroInit			= 14;
	const int StateOutroEnter			= 15;
	const int StateOutro				= 16;
	const int StateOutroExit			= 17;
	
}

class SnakeIntro  : trigger_base, SpeechBubble::Listener, SpeechBubble::CustomContent, SnakeListener
{
	
	[entity] uint snake_id;
	[position,mode:world,layer:19,y:player_y] float player_x;
	[hidden] float player_y;
	[position,mode:world,layer:19,y:turn_y] float turn_x;
	[hidden] float turn_y;
	[position,mode:world,layer:19,y:stop_y] float stop_x;
	[hidden] float stop_y;
	[position,mode:world,layer:19,y:hole_y] float hole_x;
	[hidden] float hole_y;
	
	[position,mode:world,layer:19,y:unlock_turn_y] float unlock_turn_x;
	[hidden] float unlock_turn_y;
	[position,mode:world,layer:19,y:unlock_stop_y] float unlock_stop_x;
	[hidden] float unlock_stop_y;
	
	[position,mode:world,layer:19,y:outro_y] float outro_x;
	[hidden] float outro_y;
	[position,mode:world,layer:19,y:outro_snake_y] float outro_snake_x;
	[hidden] float outro_snake_y;
	
	[persist] float curve_t = 0.7;
	[persist] array<Bezier> curves;
	
	private DLScript@ script;
	private scripttrigger@ self;
	
	[hidden]
	private int state = SnakeIntro::StateIdle;
	[hidden]
	private int player_index = -1;
	private PlayerData@ player;
	private Snake@ snake;
	
	private float snake_lock_x, snake_lock_y;
	private bool snake_lock;
	
	private int intro_dir;
	private int intro_exit_dir;
	private float start_x;
	
	private float curve_base_x1, curve_base_y1;
	private float curve_base_x2, curve_base_y2;
	private float sway_t;
	
	private bool is_dragging;
	private float drag_ox, drag_oy;
	
	private const array<string>@ dialog;
	private int dialog_index = -1;
	private int dialog_count;
	private bool dialog_completed;
	
	private const array<string>@ responses_txt = @Snake::Dialog[Snake::DlgResponses];
	
	private SpeechBubble@ snake_speech_bubble;
	private SpeechBubble player_speech_bubble;
	
	private textfield@ text_field = player_speech_bubble.create_text_field();
	
	private float response_w, response_h;
	private float response_line_spacing = 25;
	private float response_line_height = 0;
	private int response_index = 0;
	
	private float response_cursor_spacing = 15;
	private float response_cursor_scale = 0.37;
	private Sprite response_cursor('props4', 'machinery_19');
	private float response_cursor_w = response_cursor.sprite_width * response_cursor_scale;
	
	private bool allow_player_advance = true;
	
	private MessageHandler@ on_security_updated_cb;
	private MessageHandler@ on_west_wing_activate_cb;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.editor_colour_inactive(0xffff2222);
		self.editor_colour_circle(0xffff2222);
		self.editor_colour_active(0xffff6666);
		
		if(script.is_playing)
		{
			@player_speech_bubble.script = script;
			player_speech_bubble.min_width = 0;
		}
		else
		{
			for(uint i = 0; i < curves.length; i++)
			{
				curves[i].update();
			}
		}
	}
	
	void editor_var_changed(var_info@ info)
	{
		if(info.get_name(0) == 'curves')
		{
			const int index = info.get_index(0);
			if(index != -1)
			{
				Bezier@ curve = @curves[index];
				curve.update();
				
				const string var = info.get_name();
				
				if((var == 'x1' || var == 'y1') && index > 0)
				{
					Bezier@ nc = @curves[index - 1];
					nc.x4 = curve.x1;
					nc.y4 = curve.y1;
					
				}
				else if((var == 'x4' || var == 'y4') && index < int(curves.length) - 1)
				{
					Bezier@ nc = @curves[index +1];
					nc.x1 = curve.x4;
					nc.y1 = curve.y4;
					
				}
			}
		}
	}
	
	void on_add()
	{
		if(!script.is_playing)
			return;
		
		if(state == SnakeIntro::StatePendingUnlock)
		{
			add_security_listener();
		}
		
		add_west_wing_activate_listener();
		
		if(player_index != -1)
		{
			@player = script.players[player_index];
			if(player.is_snake)
			{
				@snake = player.snake;
			}
			else
			{
				find_snake();
			}
		}
	}
	
	void on_remove()
	{
		if(!script.is_playing)
			return;
		
		remove_security_listener();
		remove_west_wing_activate_listener();
	}
	
	void activate(controllable@ c)
	{
		if(state == SnakeIntro::StateUnlockedWaiting)
		{
			dustman@ dm = pre_activate(c);
			if(@dm == null)
				return;
			
			state_unlocked_waiting_exit(dm);
			return;
		}
		else if(state == SnakeIntro::StateOutroInit)
		{
			dustman@ dm = pre_activate(c, false);
			if(@dm == null)
				return;
			
			state_outro_init(dm);
			return;
		}
		
		if(state != SnakeIntro::StateIdle)
			return;
		if(@player != null)
			return;
		dustman@ dm = pre_activate(c);
		if(@dm == null)
			return;
		
		start_x = snake.self.x();
		intro_dir = start_x < player_x ? 1 : -1;
		
		snake.set_speech_listener(this);
		@snake.listener = this;
		snake.lock_target(c);
		
		player_index = dm.player_index();
		@player = script.players[player_index];
		player.lock();
		state = SnakeIntro::StateWaitingPlayerIdle;
	}
	
	private dustman@ pre_activate(controllable@ c, const bool do_find_snake=true)
	{
		if(c.player_index() == -1)
			return null;
		
		dustman@ dm = c.as_dustman();
		if(@dm == null)
			return null;
		if(dm.dead() || dm.destroyed())
			return null;
		
		if(do_find_snake && !find_snake())
			return null;
		
		return dm;
	}
	
	private bool find_snake()
	{
		scriptenemy@ se = scriptenemy_by_id(snake_id);
		if(@se == null)
			return false;
		
		@snake = cast<Snake@>(se.get_object());
		return @snake != null;
	}
	
	void step()
	{
		if(@player == null)
			return;
		
		if(allow_player_advance && dialog_index != -1 && @snake_speech_bubble != null)
		{
			if(player.check_action())
			{
				if(snake_speech_bubble.is_completed || snake_speech_bubble.force_complete())
				{
					load_next_dialog_line();
				}
			}
		}
		
		switch(state)
		{
			case SnakeIntro::StateWaitingPlayerIdle: state_waiting_player_idle(); break;
			case SnakeIntro::StateEnter: state_enter(); break;
			case SnakeIntro::StateIntro: state_intro(); break;
			case SnakeIntro::StateIntroResponse: state_intro_response(); break;
			case SnakeIntro::StateIntroAccepted: state_intro_accepted(); break;
			case SnakeIntro::StateIntroAcceptedExit: state_intro_accepted_exit(); break;
			case SnakeIntro::StateIntroRejected: state_intro_rejected(); break;
			
			case SnakeIntro::StateUnlocked: state_unlocked(); break;
			case SnakeIntro::StateUnlockedWaiting: state_unlocked_waiting(); break;
			case SnakeIntro::StateUnlockedDialog: state_unlocked_dialog(); break;
			case SnakeIntro::StateUnlockedExit: state_unlocked_exit(); break;
			case SnakeIntro::StateOutroPending: state_outro_pending(); break;
			case SnakeIntro::StateOutroEnter: state_outro_enter(); break;
			case SnakeIntro::StateOutro: state_outro(); break;
			case SnakeIntro::StateOutroExit: state_outro_exit(); break;
		}
		
		if(player_speech_bubble.is_visible)
		{
			player_speech_bubble.step(script.time_scale);
		}
	}
	
	void editor_step()
	{
		if(curves.length == 0)
			return;
		if(!self.editor_selected())
			return;
		
		if(is_dragging && !script.input.key_check_gvb(GVB::MiddleClick))
		{
			is_dragging = false;
		}
			
		if(is_dragging)
		{
			Bezier@ base_curve = @curves[0];
			const float base_x = base_curve.x1;
			const float base_y = base_curve.y1;
			const float mx = script.input.mouse_x_world(19) + drag_ox;
			const float my = script.input.mouse_y_world(19) + drag_oy;
			for(uint i = 0; i < curves.length; i++)
			{
				Bezier@ curve = @curves[i];
				curve.x1 = mx + curve.x1 - base_x;
				curve.y1 = my + curve.y1 - base_y;
				curve.x2 = mx + curve.x2 - base_x;
				curve.y2 = my + curve.y2 - base_y;
				curve.x3 = mx + curve.x3 - base_x;
				curve.y3 = my + curve.y3 - base_y;
				curve.x4 = mx + curve.x4 - base_x;
				curve.y4 = my + curve.y4 - base_y;
			}
			return;
		}
		
		if(script.input.key_check_gvb(GVB::MiddleClick))
		{
			is_dragging = true;
			drag_ox = curves[0].x1 - script.input.mouse_x_world(19);
			drag_oy = curves[0].y1 - script.input.mouse_y_world(19);
		}
	}
	
	void draw(float sub_frame)
	{
		if(player_speech_bubble.is_visible)
		{
			rectangle@ r = player.c.base_rectangle();
			player_speech_bubble.draw(
				lerp(player.c.prev_x(), player.c.x(), sub_frame) + r.left() + r.width * 0.5 + 5 * player.c.face(),
				lerp(player.c.prev_y(), player.c.y(), sub_frame) + r.top() - 10,
				sub_frame);
		}
	}
	
	void editor_draw(float)
	{
		if(!script.debug_triggers)
			return;
		
		draw_x_limit(player_x, player_y, 0x88ff0000);
		
		draw_x_limit(turn_x, turn_y, 0x8800ff00);
		draw_x_limit(stop_x, stop_y, 0x880000ff);
		draw_x_limit(hole_x, hole_y, 0xff000000);
		
		draw_x_limit(unlock_turn_x, unlock_turn_y, 0xffffaaaa);
		draw_x_limit(unlock_stop_x, unlock_stop_y, 0xffaaffaa);
		
		draw_x_limit(outro_x, outro_y, 0xff00ffff);
		draw_x_limit(outro_snake_x, outro_snake_y, 0xff00ff00);
		
		if(curves.length > 0)
		{
			const float segment_length = 5;
			
			for(uint i = 0; i < curves.length; i++)
			{
				Bezier@ curve = @curves[i];
				
				const int num_segments = int(ceil(curve.length / 5));
				float x1 = curve.x1;
				float y1 = curve.y1;
				for(int j = 0; j < num_segments; j++)
				{
					const float dist = curve.length * (j / float(num_segments - 1));
					const float x2 = curve.mx(dist);
					const float y2 = curve.my(dist);
					script.g.draw_line_world(22, 22, x1, y1, x2, y2, 2, 0xff3333ff);
					
					x1 = x2;
					y1 = y2;
				}
				
				script.g.draw_line_world(22, 22, curve.x1, curve.y1, curve.x2, curve.y2, 2, 0xffff44ff);
				script.g.draw_line_world(22, 22, curve.x3, curve.y3, curve.x4, curve.y4, 2, 0xffff44ff);
				
				draw_dot(script.g, 22, 222, curve.x1, curve.y1, 5, 0xffee3333, 0);
				draw_dot(script.g, 22, 222, curve.x2, curve.y2, 4, 0xffee4488, 45);
				draw_dot(script.g, 22, 222, curve.x3, curve.y3, 4, 0xffff4444, 45);
				draw_dot(script.g, 22, 222, curve.x4, curve.y4, 5, 0xffff0000, 0);
			}
		}
	}
	
	private void draw_x_limit(const float x, const float y, const uint clr, const float top=-100, const float bottom=250)
	{
		draw_dot(script.g, 22, 22, x, y, 5, clr, 45);
		script.g.draw_line_world(22, 22, x, y + top, x, y + bottom, 2, clr);
	}
	
	// -- State
	
	private void state_waiting_player_idle()
	{
		if(!player.c.ground())
		{
			player.c.set_speed_xy(player.c.x_speed() * 0.9, player.c.y_speed());
			return;
		}
		
		if(intro_dir == 1 && player.c.x() > player_x)
		{
			player.c.x_intent(-1);
			return;
		}
		if(intro_dir == -1 && player.c.x() < player_x)
		{
			player.c.x_intent(1);
			return;
		}
		
		player.c.x_intent(0);
		
		if(player.c.state() != EntityState::Idle)
			return;
		if(player.c.attack_state() != AttackType::Idle)
			return;
		
		for(uint i = 0; i < curves.length; i++)
		{
			curves[i].update();
		}
		
		player.c.face(-intro_dir);
		
		state = SnakeIntro::StateEnter;
		snake.activate();
		
		@snake.curves = @curves;
		update_snake_curve_t();
		snake.lock_head_orientation(intro_dir);
		
		snake.play_hiss(Snake::HissMed);
		
		if(curves.length > 0)
		{
			Bezier@ curve = curves[0];
			curve_base_x1 = curve.x1;
			curve_base_y1 = curve.y1;
			curve_base_x2 = curve.x2;
			curve_base_y2 = curve.y2;
		}
	}
	
	private void state_enter()
	{
		sway();
		curve_t -= 0.5 * DT * script.time_scale;
		
		if(curves.length > 0)
		{
			update_snake_curve_t();
		}
		
		if(curve_t <= 0)
		{
			init_dialog(Snake::DlgIntro);
			curve_t = 0;
			
			state = SnakeIntro::StateIntro;
		}
	}
	
	private void update_snake_curve_t()
	{
		snake.curve_t = ease_in_out_cubic(clamp(curve_t, 0.0, 1.0));
		snake.position_along_curve();
	}
	
	private void state_intro()
	{
		sway_and_update();
		
		if(dialog_completed)
		{
			snake.close_dialog();
			dialog_index = -1;
			
			player_speech_bubble.draw_next_arrow = false;
			@player_speech_bubble.custom_content = this;
			player_speech_bubble.open();
			
			state = SnakeIntro::StateIntroResponse;
		}
	}
	
	private void state_intro_response()
	{
		sway_and_update();
		
		const int select_response = player.check_y_intent();
		if(select_response != 0)
		{
			const int response_index_prev = response_index;
			response_index += select_response;
			
			if(response_index == 0)
				response_index += select_response;
			else if(response_index < -1)
				response_index = -1;
			else if(response_index > 1)
				response_index = 1;
			
			if(response_index != response_index_prev)
			{
				script.g.play_sound(
					'sfx_footstep_carpet_' + rand_range(1, 3),
					player.cam.x(), player.cam.y(), 1, false, true);
			}
		}
		
		if(response_index != 0 && player.check_action())
		{
			const bool accepted = response_index == -1;
			player_speech_bubble.open(false);
			init_dialog(accepted ? Snake::DlgAccepted : Snake::DlgRejected);
			allow_player_advance = accepted;
			
			state = accepted ? SnakeIntro::StateIntroAccepted : SnakeIntro::StateIntroRejected;
		}
	}
	
	private void state_intro_accepted()
	{
		sway_and_update();
		
		if(dialog_completed)
		{
			snake.close_dialog();
			dialog_index = -1;
			@snake.curves = null;
			snake.unlock_head_orientation();
			snake.lock_target(null);
			
			state = SnakeIntro::StateIntroAcceptedExit;
			intro_exit_dir = 0;
		}
	}
	
	private void state_intro_accepted_exit()
	{
		player.set_lock_pos(snake.x, snake.y);
		
		if(intro_exit_dir == 0)
		{
			if(int(sign(snake.x - turn_x)) == intro_dir)
			{
				intro_exit_dir = 1;
			}
			else
			{
				snake.self.x_intent(intro_dir);
				snake.self.y_intent(get_move_y_intent(turn_y));
			}
			
			return;
		}
		else if(intro_exit_dir == 1)
		{
			snake.self.x_intent(-intro_dir);
			snake.self.y_intent(get_move_y_intent(stop_y));
		}
		
		if(int(sign(snake.x - stop_x)) == -intro_dir)
		{
			player.lock(false);
			player.control_snake(snake);
			@snake = player.snake;
			snake_id = snake.self.id();
			
			state = SnakeIntro::StatePendingUnlock;
			add_security_listener();
		}
	}
	
	private void state_intro_rejected()
	{
		if(dialog_completed)
		{
			if(dialog_index != -1)
			{
				snake.close_dialog();
				dialog_index = -1;
				@snake.curves = null;
				snake.self.team(Team::Filth);
			}
			
			if(snake.self.heavy_intent() != 11)
			{
				const int dir = snake.x < player.c.x() ? 1 : -1;
				snake.self.x_intent(dir);
				
				if(snake.y > player.c.y() - 96)
				{
					snake.self.y_intent(-1);
				}
				
				if(abs(snake.x - player.c.x()) <= 48 * 3.5)
				{
					snake.lock_attack_target(player.c);
					snake.self.y_intent(0);
					snake.self.heavy_intent(10);
				}
			}
			
			snake.collapse(hole_x, intro_dir);
			
			if(snake.is_collapsed)
			{
				script.g.remove_entity(snake.self.as_entity());
				script.g.remove_entity(self.as_entity());
			}
		}
		else
		{
			sway_and_update();
		}
	}
	
	private void state_unlocked()
	{
		if(intro_exit_dir == 0)
		{
			player.set_lock_pos(snake.x, snake.y);
			
			if(int(sign(snake.x - unlock_turn_x)) == intro_dir)
			{
				snake.self.x_intent(-intro_dir);
				snake.self.y_intent(get_move_y_intent(unlock_turn_y));
			}
			else
			{
				intro_exit_dir = 1;
				player.lock(false);
				player.set_cam_speed(0.2);
			}
			
			return;
		}
		
		snake.self.x_intent(intro_dir);
		snake.self.y_intent(get_move_y_intent(unlock_stop_y));
		
		if(int(sign(snake.x - unlock_stop_x)) == intro_dir)
		{
			state = SnakeIntro::StateUnlockedWaiting;
			
			snake.self.x_intent(0);
			snake.self.y_intent(0);
			self.radius(self.radius() + 100);
			self.set_xy(snake.x, snake.y);
		}
	}
	
	private void state_unlocked_waiting()
	{
		if(@snake == null)
		{
			find_snake();
		}
		
		intro_dir = unlock_stop_x >= unlock_turn_x ? 1 : -1;
		self.set_xy(snake.x + intro_dir * self.radius(), snake.y);
	}
	
	private void state_unlocked_waiting_exit(dustman@ dm)
	{
		player_index = dm.player_index();
		@player = script.players[player_index];
		player.lock();
		snake.lock_target(dm.as_controllable());
		
		@snake_speech_bubble = snake.set_speech_listener(this);
		snake_speech_bubble.draw_next_arrow = true;
		allow_player_advance = true;
		init_dialog(Snake::DlgUnlocked);
		
		state = SnakeIntro::StateUnlockedDialog;
	}
	
	private void state_unlocked_dialog()
	{
		if(dialog_completed)
		{
			snake.close_dialog();
			dialog_index = -1;
			
			snake.lock_target(null);
			self.radius(0);
			
			state = SnakeIntro::StateUnlockedExit;
		}
	}
	
	private void state_unlocked_exit()
	{
		player.set_lock_pos(lerp(player.c.x(), snake.x, 0.65), snake.y);
		
		self.set_xy(player.c.x(), player.c.y());
		
		intro_dir = unlock_stop_x >= unlock_turn_x ? 1 : -1;
		snake.self.x_intent(intro_dir);
		snake.collapse(hole_x, intro_dir);
		
		if(snake.is_collapsed)
		{
			player.lock(false);
			
			script.g.remove_entity(snake.self.as_entity());
			state = SnakeIntro::StateOutroPending;
			add_west_wing_activate_listener();
		}
	}
	
	private void state_outro_pending()
	{
		self.set_xy(player.c.x(), player.c.y());
	}
	
	private void state_outro_init(dustman@ dm)
	{
		player_index = dm.player_index();
		@player = script.players[player_index];
		
		player.lock();
		dm.face(1);
		
		@snake = Snake();
		snake.is_active = true;
		scriptenemy@ se = create_scriptenemy(snake);
		se.set_xy(outro_snake_x, outro_snake_y);
		se.prev_x(outro_snake_x);
		se.prev_y(outro_snake_y);
		se.face(-1);
		script.g.add_entity(se.as_entity(), true);
		
		snake.self.x_intent(-1);
		
		state = SnakeIntro::StateOutroEnter;
	}
	
	private void state_outro_enter()
	{
		if(!snake.is_ready)
			return;
		
		if(abs(snake.x - player.c.x()) < 48 * 8)
		{
			@snake_speech_bubble = snake.set_speech_listener(this);
			snake_speech_bubble.draw_next_arrow = true;
			allow_player_advance = true;
			init_dialog(Snake::DlgOutro);
			
			snake.self.x_intent(0);
			state = SnakeIntro::StateOutro;
		}
	}
	
	private void state_outro()
	{
		if(snake_lock)
		{
			snake.lock(snake_lock_x, snake_lock_y);
		}
		else if(abs(snake.speed_x) < 0.1)
		{
			snake_lock = true;
			snake_lock_x = snake.x;
			snake_lock_y = snake.y;
		}
		
		if(dialog_completed)
		{
			snake.close_dialog();
			snake.self.x_intent(-1);
			state = SnakeIntro::StateOutroExit;
		}
	}
	
	private void state_outro_exit()
	{
		player.set_lock_pos(lerp(player.c.x(), snake.x, 0.5), snake.y);
		snake.update_slide_volume_mult(snake.on_screen_percent());
		
		if(snake.is_offscreen())
		{
			player.lock(false);
			
			script.g.remove_entity(self.as_entity());
			script.g.remove_entity(snake.self.as_entity());
		}
	}
	
	//
	
	private int get_move_y_intent(const float height)
	{
		return snake.is_touching_wall() || snake.y < height ? 1 : -1;
	}
	
	private void sway()
	{
		if(curves.length == 0)
			return;
		
		sway_t += DT * script.time_scale;
		
		Bezier@ curve = curves[0];
		curve.x1 = curve_base_x1 + sin(sway_t * 1.0) * 10;
		curve.y1 = curve_base_y1 + cos(sway_t * 1.3) * 10;
		curve.x2 = curve_base_x2 + sin(sway_t * 0.7) * 5;
		curve.y2 = curve_base_y2 + cos(sway_t * 0.9) * 5;
		curve.update();
	}
	
	private void sway_and_update()
	{
		if(curves.length == 0)
			return;
		
		sway();
		snake.position_along_curve(true);
	}
	
	private bool load_next_dialog_line()
	{
		if(dialog_index >= dialog_count)
		{
			dialog_completed = true;
			return true;
		}
		
		snake.set_dialog(dialog[dialog_index++]);
		snake_speech_bubble.flash_next_arrow();
		return false;
	}
	
	private void init_dialog(const int index, const int start_line=0)
	{
		@dialog = @Snake::Dialog[index];
		dialog_count = dialog.length;
		dialog_completed = false;
		dialog_index = start_line >= 0
			? start_line % dialog_count
			: (((dialog_count + start_line) % dialog_count) + dialog_count) % dialog_count;
		snake.set_dialog(dialog[dialog_index++]);
	}
	
	private void draw_response_cursor(
		const int layer, const int sub_layer, const float tx, const float ty, const float w,
		const float sx, const float sy, const uint clr=0xffffffff)
	{
		for(int i = -1; i <= 1; i += 2)
		{
			response_cursor.draw(layer, sub_layer, 0, 0,
				tx + (w > 0 ? response_w * 0.5 - response_cursor_w * 0.5 : 0.0) * i,
				ty, 0,
				response_cursor_scale * sx, response_cursor_scale * sy, clr);
			
			if(w <= 0)
				break;
		}
	}
	
	private void add_security_listener()
	{
		if(@on_security_updated_cb != null)
			return;
		
		@on_security_updated_cb = MessageHandler(on_security_updated);
		script.messages.add_listener('security_updated', on_security_updated_cb);
	}
	
	private void remove_security_listener()
	{
		if(@on_security_updated_cb == null)
			return;
		
		script.messages.remove_listener('security_updated', on_security_updated_cb);
		@on_security_updated_cb = null;
	}
	
	private void add_west_wing_activate_listener()
	{
		if(state != SnakeIntro::StateOutroPending)
			return;
		if(@on_west_wing_activate_cb != null)
			return;
		
		@on_west_wing_activate_cb = MessageHandler(on_west_wing_activate);
		script.messages.add_listener('west_wing_activate', on_west_wing_activate_cb);
	}
	
	private void remove_west_wing_activate_listener()
	{
		if(@on_west_wing_activate_cb == null)
			return;
		
		script.messages.remove_listener('west_wing_activate', on_west_wing_activate_cb);
		@on_west_wing_activate_cb = null;
	}
	
	private void on_security_updated(const string &in event, message@ msg)
	{
		if(script.security_level != 1)
			return;
		
		@player = script.players[player_index];
		PlayerSpawner@ spawner = player.player_spawner;
		if(@spawner == null)
			return;
		
		state = SnakeIntro::StateUnlocked;
		intro_dir = unlock_stop_x >= unlock_turn_x ? 1 : -1;
		intro_exit_dir = 0;
		@snake = player.control_dustman();
		snake_id = snake.self.id();
		player.lock(true);
		
		remove_security_listener();
	}
	
	private void on_west_wing_activate(const string &in event, message@ msg)
	{
		const bool open = msg.get_int('open', 0) == 1;
		player.lock(!open);
		
		if(open)
		{
			raycast@ ray;
			@ray = script.g.ray_cast_tiles(outro_x, outro_y, outro_x, outro_y - 10000, ray);
			const float y1 = ray.hit_y();
			@ray = script.g.ray_cast_tiles(outro_x, outro_y, outro_x, outro_y + 10000, ray);
			const float y2 = ray.hit_y();
			
			self.radius(int((y2 - y1) * 0.5) + 24);
			self.set_xy(outro_x - self.radius(), (y1 + y2) * 0.5);
			
			state = SnakeIntro::StateOutroInit;
		}
	}
	
	// SpeechBubble::Listener
	
	void on_speech_bubble_init(SpeechBubble@ speech_bubble)
	{
		@this.snake_speech_bubble = speech_bubble;
	}
	
	void on_speech_bubble_progress(SpeechBubble@ speech_bubble, const int chr_index) {  }
	
	void on_speech_bubble_complete(SpeechBubble@ speech_bubble) { }
	
	void on_speech_bubble_advance(SpeechBubble@ speech_bubble)
	{
		if(speech_bubble.force_auto_advance)
		{
			load_next_dialog_line();
		}
	}
	
	// SpeechBubble::CustomContent
	
	void get_speech_bubble_content_final_size(float &out width, float &out height)
	{
		response_h = response_line_spacing;
		
		text_field.text(responses_txt[0]);
		response_line_height = text_field.text_height();
		response_w = text_field.text_width();
		response_h += response_line_height;
		
		text_field.text(responses_txt[1]);
		response_w = max(response_w, float(text_field.text_width()));
		response_h += response_line_height;
		
		response_w += (response_cursor_w + response_cursor_spacing) * 2;
		
		width = response_w;
		height = response_h;
	}
	
	void get_speech_bubble_content_size(float &out width, float &out height)
	{
		width = response_w * 0;
		height = response_h * 0;
	}
	
	void speech_bubble_draw(const int layer, const int sub_layer, const float x, const float y, const float w, const float h, const float sub_frame)
	{
		float sx, sy;
		player_speech_bubble.get_size_percent(sx, sy);
		
		const float by = y - 2 + player_speech_bubble.padding;
		
		float tx = x - 0.5 + w * 0.5;
		float ty = by;
		
		text_field.text(responses_txt[0]);
		text_field.draw_world(
			layer, sub_layer,
			tx, ty, sx, sy, 0);
		
		if(response_index == -1)
		{
			draw_response_cursor(layer, sub_layer, tx, ty + (response_line_height * 0.5 + 2) * sy, w, sx, sy);
		}
		
		ty += response_line_height * sy;
		float fy = lerp(by, ty, sy);
		
		script.g.draw_rectangle_world(layer, sub_layer,
			x + w * 0.5 - (w * 0.5 - player_speech_bubble.padding) * sx,
			fy + (response_line_spacing * 0.5 - 1 + 4) * sy,
			x + w * 0.5 + (w * 0.5 - player_speech_bubble.padding) * sx,
			fy + (response_line_spacing * 0.5 + 1 + 4) * sy,
			0, 0x227a7a7a);
		
		if(response_index == 0)
		{
			draw_response_cursor(layer, sub_layer, tx, fy + (response_line_spacing * 0.5 + 4) * sy, 0, sx, sy, 0x66ffffff);
		}
		
		ty += response_line_spacing * sy;
		fy = lerp(by, ty, sy);
		
		text_field.text(responses_txt[1]);
		text_field.draw_world(
			layer, sub_layer,
			tx, fy, sx, sy, 0);
		
		if(response_index == 1)
		{
			draw_response_cursor(layer, sub_layer, tx, fy + response_line_height * 0.5 + 2, w, sx, sy);
		}
	}
	
	// SnakeListener
	
	void on_snake_attack_hit(controllable@ attacked, hitbox@ hb)
	{
		if(attacked.is_same(player.c))
		{
			snake.lock_target(null);
			snake.lock_attack_target(null);
			player.kill();
			script.g.combo_break_count(script.g.combo_break_count() + 1);
		}
	}
	
}
