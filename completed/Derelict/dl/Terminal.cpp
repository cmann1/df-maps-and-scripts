class Terminal : EnterExitTrigger
{
	
	[entity,trigger] private uint text_id;
	[entity,trigger] private uint gate_id;
	[hidden] private TerminalState state = Idle;
	
	private DLScript@ script;
	private scripttrigger@ self;
	
	private entity@ text_trigger;
	private varvalue@ text_var;
	
	[hidden] private float timer_prev;
	[hidden] private float timer;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = script;
		@this.self = self;
	}
	
	void on_add()
	{
		if(!script.is_playing)
			return;
		
		@text_trigger = entity_by_id(text_id);
		if(@text_trigger != null)
		{
			@text_var = text_trigger.vars().get_var('text');
		}
	}
	
	void activate(controllable@ c)
	{
		if(state != Idle)
			return;
		
		activate_enter_exit(c);
	}
	
	bool can_trigger_enter_exit(controllable@ c)
	{
		if(c.player_index() == -1)
			return false;
		
		return true;
	}
	
	void on_trigger_enter(controllable@ c)
	{
		message@ msg = create_message();
		msg.set_int('open', 0);
		script.messages.broadcast('west_wing_activate', msg);
		
		state = Confirm;
		timer_prev = 0;
		timer = 0;
	}
	
	void step()
	{
		step_enter_exit();
		
		timer_prev = timer;
		
		switch(state)
		{
			case TerminalState::Idle: state_idle(); break;
			case TerminalState::Confirm: state_confirm(); break;
			case TerminalState::WaitReply: state_wait_reply(); break;
			case TerminalState::Rebooting: state_rebooting(); break;
			case TerminalState::Online: state_online(); break;
		}
	}
	
	private void state_idle()
	{
		const int chr = int(timer / 28) % 2;
		text_var.set_string('Door error' + (chr == 0 ? '_' : ' '));
		
		timer += script.time_scale;
	}
	
	private void state_confirm()
	{
		const int chr = int(timer / 28) % 2;
		text_var.set_string('Door error\nConfirm reset?'.substr(0, 11 + int(timer / 8)) + (chr == 0 ? '_' : ' '));
		
		timer += script.time_scale;
		
		if(timer > 145)
		{
			text_var.set_string('Door error\nConfirm reset?\n  [Y] [N]');
			state = WaitReply;
			timer = 0;
		}
	}
	
	private void state_wait_reply()
	{
		timer += script.time_scale;
		
		if(timer < 60)
			return;
		
		const int chr = int(timer / 8) % 2;
		text_var.set_string('Door error\nConfirm reset?\n  ' + (chr == 0 ? '[Y]' : '   ') + ' [N]');
		
		if(timer > 160)
		{
			state = Rebooting;
			timer_prev = 0;
			timer = 0;
		}
	}
	
	private void state_rebooting()
	{
		if(timer < 60)
		{
			text_var.set_string('');
		}
		else
		{
			const int chr = int(timer / 28) % 2;
			text_var.set_string('Rebooting....'.substr(0, int((timer - 60) / 8)) + (chr == 0 ? '_' : ' '));
		}
		
		timer += script.time_scale;
		
		if(timer > 300)
		{
			state = Online;
			timer_prev = 0;
			timer = 0;
		}
	}
	
	private void state_online()
	{
		const int chr = int(timer / 28) % 2;
		text_var.set_string('Systems online.'.substr(0, int(timer / 8)) + (chr == 0 ? '_' : ' '));
		
		timer += script.time_scale;
		
		if(timer > 260)
		{
			text_var.set_string('Systems online.');
			
			IOpenable@ gate = get_gate();
			if(@gate != null)
			{
				gate.open();
			}
			
			message@ msg = create_message();
			msg.set_int('open', 1);
			script.messages.broadcast('west_wing_activate', msg);
			
			script.g.remove_entity(self.as_entity());
		}
	}
	
	private IOpenable@ get_gate()
	{
		scripttrigger@ se = scripttrigger_by_id(gate_id);
		if(@se == null)
			return null;
		
		return cast<IOpenable@>(se.get_object());
	}
	
}

enum TerminalState
{
	
	Idle,
	Confirm,
	WaitReply,
	Rebooting,
	Online,
	
}
