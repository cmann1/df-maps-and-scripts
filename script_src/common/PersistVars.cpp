class PersistVars
{
	
	dictionary variables;
	dictionary variables_checkpoint;
	
	string dict_key_prefix = '__persist_var__';
	
	PersistVars()
	{
		
	}
	
	void listen(string key, callback_base@ obj, string meth_name)
	{
		add_broadcast_receiver(dict_key_prefix + key, obj, meth_name);
	}
	
	void checkpoint_save()
	{
		variables_checkpoint = variables;
	}
	
	void checkpoint_load()
	{
		variables = variables_checkpoint;
	}
	
	bool has_key(string key)
	{
		return variables.exists(dict_key_prefix + key);
	}
	
	void set_int(string key, int value)
	{
		bool has_changed = false;
		key = dict_key_prefix + key;
		
		if(!variables.exists(key))
		{
			has_changed = true;
		}
		else
		{
			const int old_value = int(variables[key]);
			
			if(old_value != value)
				has_changed = true;
		}
		
		if(has_changed)
		{
			variables[key] = value;
			broadcast_message(key, create_message());
		}
	}
	int get_int(string key, int default_value=0)
	{
		key = dict_key_prefix + key;
		
		if(!variables.exists(key))
			return default_value;
		
		return int(variables[key]);
	}
	
	void set_bool(string key, bool value)
	{
		bool has_changed = false;
		key = dict_key_prefix + key;
		
		if(!variables.exists(key))
		{
			has_changed = true;
		}
		else
		{
			const bool old_value = bool(variables[key]);
			
			if(old_value != value)
				has_changed = true;
		}
		
		if(has_changed)
		{
			variables[key] = value;
			broadcast_message(key, create_message());
		}
	}
	bool get_bool(string key, bool default_value=false)
	{
		key = dict_key_prefix + key;
		
		if(!variables.exists(key))
			return default_value;
		
		return bool(variables[key]);
	}
	
	void set_string(string key, string value)
	{
		bool has_changed = false;
		key = dict_key_prefix + key;
		
		if(!variables.exists(key))
		{
			has_changed = true;
		}
		else
		{
			const string old_value = string(variables[key]);
			
			if(old_value != value)
				has_changed = true;
		}
		
		if(has_changed)
		{
			variables[key] = value;
			broadcast_message(key, create_message());
		}
	}
	string get_string(string key, string default_value='')
	{
		key = dict_key_prefix + key;
		
		if(!variables.exists(key))
			return default_value;
		
		return string(variables[key]);
	}
	
}

class SetPersistVarTrigger : trigger_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	[text] string var_name = '';
	[option,0:Bool,1:Int,2:Float,3:String]
	int var_type = 0;
	[text] bool bool_value = false;
	[text] int int_value = 0;
	[text] float float_value = 0;
	[text] string string_value = '';
	
	SetPersistVarTrigger()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
	}
	
	void activate(controllable@ e)
	{
		if(e.player_index() == -1)
			return;
		
		switch(var_type)
		{
			case 0: script.vars.set_bool(var_name, bool_value); break;
			case 1: script.vars.set_int(var_name, int_value); break;
//			case 2: script.vars.set_int(var_name, int_value); break; // TODO: Once set_flaot is implemented
			case 3: script.vars.set_string(var_name, string_value); break;
		}
	}
	
}