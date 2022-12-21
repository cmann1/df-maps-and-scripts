#include '../lib/drawing/circle.cpp';
#include '../lib/drawing/SpriteGroup.cpp';
#include '../lib/easing/cubic.cpp';
#include '../lib/easing/sine.cpp';
#include '../lib/emitters/common.cpp';
#include '../lib/enums/ColType.cpp';
#include '../lib/enums/EmitterId.cpp';
#include '../lib/triggers/EnterExitTrigger.cpp';

const int ChargeIdle = 0;
const int ChargeActive = 1;
const int ChargeError = 2;

class Recharger : EntityOutlinerSource
{
	
	[persist] bool extra_charge;
	
	[hidden] int state = ChargeIdle;
	
	scene@ g;
	script@ script;
	
	float x, y;
	float radius = 30;
	entity@ emitter;
	int icon_sublayer = 9;
	
	float pulse = 0;
	int pulse_dir = 0;
	float base_rotation = 0;
	float rotation = 0;
	float rotation_t;
	int rotation_dir = 1;
	
	float x1, y1, x2, y2;
	
	RechargerActivation hitbox;
	
	/// Adding/remove emitters on the same frame can cause a crash.
	private bool pending_emitter_add;
	private bool emitter_added;
	
	void init(script@ s, scripttrigger@ self)
	{
		EntityOutlinerSource::init(s, self);
		@this.script = s;
		
		self.editor_colour_circle(0xcc82358d);
		self.editor_colour_inactive(0xff82358d);
		self.editor_colour_active(0xffB18BB6);
		
		x = self.x();
		y = self.y();
		
		@hitbox.recharger = this;
		
		
		const RechargerSprites@ spr = @script.sprites;
		const SpriteGroup@ charge_spr = extra_charge
			? (state == ChargeActive ? spr.ChargeDoubleActive : state == ChargeActive ? spr.ChargeDoubleError : spr.ChargeDouble)
			: (state == ChargeActive ? spr.ChargeSingleActive : state == ChargeActive ? spr.ChargeSingleError : spr.ChargeSingle);
		SpriteRectangle r1 = spr.ChargeBack.get_rectangle(0.0, 1.0);
		SpriteRectangle r2 = charge_spr.get_rectangle(0.0, 1.0);
		x1 = min(r1.left, r2.left);
		y1 = min(r1.top, r2.top);
		x2 = max(r1.right, r2.right);
		y2 = max(r1.bottom, r2.bottom);
		
		if(script.is_playing)
		{
			const int size = extra_charge ? 52 : 40;
			@emitter = create_emitter(
				EmitterId::Sparks, x, y, size, size, 17, 8, 0);
			update_emitter_layer();
		}
		else
		{
			self.editor_handle_size(5);
		}
	}
	
	void on_add()
	{
		if(script.is_playing)
		{
			script.register_charger(self.id());
		}
	}
	
	private void update_emitter_layer()
	{
		sub_layer = state == ChargeIdle
			? (extra_charge ? 6 : 9)
			: (state == ChargeActive ? 7 : 5);
		icon_sublayer = state == ChargeIdle
			? (extra_charge ? 9 : 8)
			: (state == ChargeActive ? 10 : 11);
		
		if(@emitter != null)
		{
			if(emitter_added)
			{
				script.g.remove_entity(emitter);
			}
			
			emitter.vars().get_var('draw_depth_sub').set_int32(icon_sublayer);
			pending_emitter_add = true;
			emitter_added = false;
		}
	}
	
	void step()
	{
		EntityOutlinerSource::step();
		
		if(pulse_dir == 1)
		{
			pulse += 0.1 * script.time_scale;
			if(pulse >= 1)
			{
				pulse = 1;
				pulse_dir = -1;
			}
			
			rotation_t = pulse * 0.5;
			rotation = base_rotation + 90 * rotation_dir * ease_out_sine(rotation_t);
		}
		else if(pulse_dir == -1)
		{
			pulse -= 0.1 * script.time_scale;
			if(pulse <= 0)
			{
				pulse = 0;
				pulse_dir = 0;
			}
			rotation_t = 0.5 + (1 - pulse) * 0.5;
			rotation = base_rotation + 90 * rotation_dir * ease_out_sine(rotation_t);
		}
		
		int i = script.g.get_entity_collision(
			y - radius, y + radius, x - radius, x + radius,
			ColType::Player);
		while(--i >= 0)
		{
			controllable@ c = script.g.get_controllable_collision_index(i);
			if(@c != null)
			{
				hitbox.activate_enter_exit(c);
			}
		}
		
		hitbox.step_enter_exit();
		
		// Readd emitters for updated properties/vars to take effect.
		if(pending_emitter_add)
		{
			script.g.add_entity(emitter, false);
			pending_emitter_add = false;
			emitter_added = true;
		}
	}
	
	void editor_step()
	{
		x = self.x();
		y = self.y();
	}
	
	void on_activate(dustman@ dm)
	{
		if(state == ChargeError)
			return;
		
		if(state == ChargeIdle)
		{
			const string character = dm.character();
			state = character == 'dustman' || character == 'dustgirl' ? ChargeActive : ChargeError;
			update_emitter_layer();
			script.activate_charger(self.id());
		}
		
		if(state != ChargeError)
		{
			dm.dash(int(max(extra_charge ? dm.dash_max() + 1 : dm.dash_max(), dm.dash())));
		}
		
		entity@ activate_emitter = create_emitter(EmitterId::DustGroundCreate, x, y, 48, 48, 17, icon_sublayer, 0);
		script.g.add_entity(activate_emitter, false);
		
		for(int i = 0; i < 5; i++)
		{
			audio@ snd = script.g.play_sound('sfx_sparks_' + rand_range(1, 12), x, y, rand_range(0.9, 1.0), false, true);
			snd.time_scale(rand_range(0.95, 1.25));
		}
		
		if(pulse_dir == 0)
		{
			base_rotation = rotation;
			rotation_dir = -rotation_dir;
		}
		
		pulse_dir = 1;
	}
	
	void draw(float)
	{
		if(x + x1 > script.view_x2 || x + x2 < script.view_x1 || y + y1 > script.view_y2 || y + y2 < script.view_y1)
			return;
		
		const RechargerSprites@ spr = @script.sprites;
		spr.ChargeBack.draw(x, y, rotation, 1.0);
		
		const SpriteGroup@ charge_spr = extra_charge
			? (state == ChargeActive ? spr.ChargeDoubleActive : state == ChargeActive ? spr.ChargeDoubleError : spr.ChargeDouble)
			: (state == ChargeActive ? spr.ChargeSingleActive : state == ChargeActive ? spr.ChargeSingleError : spr.ChargeSingle);
		float scale = lerp(1.0, 1.25, pulse_dir == 1 ? ease_out_sine(pulse) : pulse_dir == -1 ? ease_in_sine(pulse) : 0.0);
		charge_spr.draw(x, y, 0.0, scale);
		
		//outline_rect(g, 22, 22, x + x1, y + y1, x + x2, y + y2, 1, 0xffff0000);
	}
	
	void editor_draw(float)
	{
		draw(0);
		
		const float x = self.x();
		const float y = self.y();
		drawing::circle(
			script.g, 22, 22,
			x, y, radius, 32,
			1, 0xaaff0000);
	}
	
}

class RechargerActivation : EnterExitTrigger
{
	
	Recharger@ recharger;
	
	bool can_trigger_enter_exit(controllable@ c)
	{
		return c.player_index() != -1;
	}
	
	void on_trigger_enter(controllable@ c)
	{
		dustman@ dm = c.as_dustman();
		if(@dm == null)
			return;
		
		recharger.on_activate(dm);
	}
	
}
