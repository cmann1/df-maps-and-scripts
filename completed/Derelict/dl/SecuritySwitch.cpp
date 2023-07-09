#include '../lib/triggers/EnterExitTrigger.cpp';
#include '../lib/enums/EmitterId.cpp';
#include '../lib/easing/quart.cpp';

#include 'IOpenable.cpp';

namespace SecuritySwitch
{
	
	const EmitterBurstSettings UnlockParticles(
		EmitterId::Rain, 17, 22,
		38, 38, 3);
	
}

class SecuritySwitch : trigger_base, EnterExitTrigger
{
	
	[entity,trigger] uint gate_id;
	[persist] int security_level = -1;
	[persist] bool once = true;
	[persist] bool toggle = false;
	
	[persist] bool active = false;
	[hidden] bool locked = true;
	
	[hidden] int current_level = 0;
	[hidden] bool can_activate = true;
	
	private bool initialised;
	
	private DLScript@ script;
	private scripttrigger@ self;
	
	private float rotation, rotation_prev;
	private float rotation_t;
	private int rotation_dir;
	private bool unlock_burst = true;
	
	private MessageHandler@ on_security_updated_cb;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.radius(32);
		
		self.editor_colour_inactive(0xffffcc77);
		self.editor_colour_circle(0xffffcc77);
		self.editor_colour_active(0xffff7777);
		
		//if(script.is_playing && !active && !has_props())
		//{
		//	script.sprites.SwitchInactive.place_props(script.g, self.x(), self.y());
		//}
		
		if(script.is_playing && active)
		{
			update_state_from_gate();
		}
		
		rotation = locked ? 90.0 : 0.0;
		rotation_prev = rotation;
		rotation_t = locked ? 0.0 : 1.0;
	}
	
	void on_add()
	{
		if(!script.is_playing)
			return;
		
		if(!active)
		{
			if(@on_security_updated_cb == null)
			{
				@on_security_updated_cb = MessageHandler(on_security_updated);
			}
			
			// Make sure to use `post_security_updated` instead since we need to update
			// the lock state based on the linked gate, which could also change during security_updated.
			script.messages.add_listener('post_security_updated', on_security_updated_cb);
		}
		
		if(current_level != script.security_level)
		{
			on_security_updated('', null);
		}
	}
	
	void on_remove()
	{
		if(!script.is_playing)
			return;
		
		if(@on_security_updated_cb != null)
		{
			script.messages.remove_listener('post_security_updated', on_security_updated_cb);
		}
	}
	
	void on_trigger_enter(controllable@ c)
	{
		IOpenable@ gate = get_gate();
		if(@gate == null)
			return;
		
		locked = !locked;
		rotation_dir = locked ? -1 : 1;
		
		if(toggle)
		{
			gate.toggle();
		}
		else
		{
			gate.open(!locked);
		}
		
		if(!locked && security_level != -1)
		{
			script.update_security_level(security_level);
		}
		
		if(once)
		{
			can_activate = false;
		}
	}
	
	void activate(controllable@ c)
	{
		if(!active || !can_activate)
			return;
		
		activate_enter_exit(c);
	}
	
	void step()
	{
		if(!active)
			return;
		
		if(!initialised)
		{
			update_state_from_gate();
		}
		
		step_enter_exit();
		
		if(rotation_dir != 0)
		{
			rotation_t = rotation_dir > 0
				? min(rotation_t + 3 * DT, 1.0)
				: max(rotation_t - 3 * DT, 0.0);
			rotation_prev = rotation;
			rotation = 90 * ease_in_out_quart(1 - rotation_t);
			
			if(!locked && unlock_burst && rotation_t >= 0.5)
			{
				script.emitter_bursts.add(SecuritySwitch::UnlockParticles, self.x(), self.y(), 0);
				unlock_burst = false;
			}
			
			if(rotation_t == 0 || rotation_t == 1)
			{
				rotation_dir = 0;
				unlock_burst = true;
			}
		}
	}
	
	bool can_trigger_enter_exit(controllable@ c)
	{
		return c.player_index() != -1;
	}
	
	void draw(float sub_frame)
	{
		const float x = self.x();
		const float y = self.y();
		
		if(!active)
		{
			script.sprites.SwitchInactive.draw(x, y);
		}
		else
		{
			script.sprites.SwitchRotator.draw(x, y, lerp_angle_degrees(rotation_prev, rotation, sub_frame), 1);
			script.sprites.Switch.draw(x, y);
			
			if(locked || rotation_t < 1)
			{
				script.sprites.SwitchLightLocked.draw(x, y, uint((1 - rotation_t) * 255) << 24 | 0xffffff);
			}
			if(!locked || rotation_t > 0)
			{
				script.sprites.SwitchLightUnlocked.draw(x, y, uint(rotation_t * 255) << 24 | 0xffffff);
			}
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		if(!script.debug_triggers)
			return;
		
		if(gate_id == 0)
			return;
		
		entity@ e = entity_by_id(gate_id);
		if(@e == null)
			return;
		
		draw_arrow(script.g, 22, 22, self.x(), self.y(), e.x(), e.y(), 2, 14, 0.5, 0x55ffcc77);
	}
	
	private void on_security_updated(const string &in event, message@ msg)
	{
		if(current_level == script.security_level)
			return;
		
		current_level = script.security_level;
		
		if(!active && current_level >= 1)
		{
			active = true;
			update_state_from_gate();
			
			if(@on_security_updated_cb != null)
			{
				script.messages.remove_listener('security_updated', on_security_updated_cb);
				@on_security_updated_cb = null;
			}
		}
	}
	
	private IOpenable@ get_gate()
	{
		scripttrigger@ se = scripttrigger_by_id(gate_id);
		if(@se == null)
			return null;
		
		return cast<IOpenable@>(se.get_object());
	}
	
	private void update_state_from_gate()
	{
		IOpenable@ gate = get_gate();
		if(@gate == null)
			return;
		
		locked = gate.is_closed;
		initialised = true;
	}
	
	/*private bool has_props()
	{
		int i = script.g.get_prop_collision(self.y(), self.y(), self.x(), self.x());
		while(--i >= 0)
		{
			prop@ p = script.g.get_prop_collision_index(i);
			if(p.layer() != 17)
				continue;
			const uint sub_layer = p.sub_layer();
			
			if(
				sub_layer >= 1 && sub_layer <= 3 || sub_layer == 5 || sub_layer == 9 || sub_layer == 10 ||
				sub_layer == 13 || sub_layer == 17 || sub_layer == 20 || sub_layer == 21
			)
				return true;
		}
		
		return false;
	}*/
	
}
