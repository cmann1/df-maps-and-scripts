#include '../../lib/utils/print_vars.cpp';

class Dialog : trigger_base, callback_base, IInitHandler
{
	
	[entity] int trigger_id;
	[persist] array<TextData> dialog;
	
	script@ script;
	scripttrigger@ self;
	
	TextData base_data;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = script;
		@this.self = self;
		
		self.radius(0);
		self.editor_colour_inactive(0xffcccccc);
		self.editor_colour_active(0xffffffff);
		self.editor_colour_circle(0xffcccccc);
		
		script.init_subscribe(this);
	}
	
	void init() override
	{
		update_trigger();
		update_data();
	}
	
	private void update_trigger()
	{
		base_data.trigger = trigger_id;
		base_data.init_trigger();
	}
	
	private void update_data()
	{
		dictionary events;
		
		for(uint i = 0; i < dialog.length; i++)
		{
			TextData@ data = @dialog[i];
			data.init_trigger();
			
			if(script.is_playing && data.event != '' && !events.exists(data.event))
			{
				script.messages.add_listener(data.event, MessageHandler(on_message));
				events.set(data.event, true);
			}
		}
	}
	
	void editor_draw(float sub_frame)
	{
		if(@base_data.trigger_e != null)
		{
			draw_line(script.g, 21, 21,
				self.x(), self.y(), base_data.trigger_e.x(), base_data.trigger_e.y(),
				1 * script.ed_zoom, 0x66ff5555);
		}
		
		for(uint i = 0; i < dialog.length; i++)
		{
			TextData@ data = @dialog[i];
			if(@data.trigger_e == null)
				continue;
			
			draw_line(script.g, 21, 21,
				self.x(), self.y(), data.trigger_e.x(), data.trigger_e.y(),
				1 * script.ed_zoom, 0x66ffffff);
		}
	}
	
	void editor_var_changed(var_info@ var)
	{
		const string name = var.name;
		
		if(name == 'trigger_id')
		{
			update_trigger();
		}
		else if(name == 'trigger' || name == 'dialog')
		{
			update_data();
		}
	}
	
	private void on_message(const string &in id, message@ msg)
	{
		for(uint i = 0; i < dialog.length; i++)
		{
			TextData@ data = @dialog[i];
			if(data.event == '' || data.event != id)
				continue;
			if(data.event_type != '' && data.event_type != msg.get_string('type'))
				continue;
			if(data.event_id != -1 && data.event_id != msg.get_int('id'))
				continue;
			
			if(data.count == -99 && data.trigger == 0)
			{
				remove_all();
				return;
			}
			
			data.count -= msg.get_int('count');
			if(data.count > 0)
				return;
			data.event = '';
			
			TextData d = base_data;
			if(!base_data.swap(script, data))
			{
				remove_all();
				return;
			}
			
			trigger_id = data.trigger;
			base_data = data;
			dialog[i] = d;
			
			if(base_data.count == -100)
			{
				remove_all(false);
				break;
			}
		}
	}
	
	private void remove_all(const bool remove_base=true)
	{
		if(remove_base && @base_data.trigger_e != null)
		{
			script.g.remove_entity(base_data.trigger_e);
		}
		
		for(uint i = 0; i < dialog.length; i++)
		{
			TextData@ data = @dialog[i];
			
			if(@data.trigger_e != null)
			{
				script.g.remove_entity(data.trigger_e);
			}
		}
		
		script.g.remove_entity(self.as_entity());
	}
	
}

class TextData
{
	
	[persist] string event;
	[persist] string event_type = '';
	[persist] int event_id = -1;
	[persist] int count = 0;
	[entity] int trigger;
	
	entity@ trigger_e;
	varstruct@ vars;
	varvalue@ text_var;
	varvalue@ width_var;
	
	void init_trigger()
	{
		if(trigger <= 0)
			return;
		
		@trigger_e = entity_by_id(trigger);
		
		if(@trigger_e != null)
		{
			varstruct@ vars = trigger_e.vars();
			@width_var = vars.get_var('width');
			@text_var = vars.get_var('text_string');
		}
	}
	
	bool swap(script@ script, TextData@ other)
	{
		if(@trigger_e == null)
			return true;
		
		if(@other.trigger_e == null)
		{
			script.g.remove_entity(trigger_e);
			@trigger_e = null;
			return false;
		}
		
		const float x = trigger_e.x();
		const float y = trigger_e.y();
		const int w = width_var.get_int32();
		trigger_e.set_xy(other.trigger_e.x(), other.trigger_e.y());
		width_var.set_int32(other.width_var.get_int32());
		
		other.width_var.set_int32(w);
		other.trigger_e.set_xy(x, y);
		return true;
	}
	
}
