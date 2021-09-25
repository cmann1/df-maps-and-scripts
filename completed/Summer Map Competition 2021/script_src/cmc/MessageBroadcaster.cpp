#include '../lib/triggers/EnterExitTrigger.cpp';

/// Provides some options for broadcasting messages when the trigger is activated.
class MessageBroadcaster : trigger_base, IInitHandler, EnterExitTrigger
{
	
	/// Can players trigger this.
	[text] bool players	= true;
	/// Can apples trigger this.
	[text] bool apples	= true;
	/// Can enemies trigger this.
	[text] bool enemies	= true;
	/// If true this trigger will remove itself after the first broadcast.
	[text] bool once	= false;
	/// If true the message won't broadcast when an entity enters if there is already another entity inside of the trigger area.
	[text] bool group	= false;
	/// If this is not zero, the message will be sent to the entity with this id instead of a global broadcast_message
	[entity] int entity_id;
	/// The id of the message that will be broadcast.
	[text] string id;
	/// If not empty, an int will be set on the message when broadcast.
	[text] string key;
	/// The int value when 'key' is not empty.
	[text] int value;
	/// If not empty, the trigger will remove itself when it receives this event.
	[text] string remove_event;
	
	scripttrigger@ self;
	script@ script;
	
	bool initialised;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = script;
		@this.self = self;
		
		script.init_subscribe(this);
	}
	
	void init() override
	{
		initialised = true;
		
		if(remove_event != '')
		{
			script.messages.add_listener(remove_event, MessageHandler(on_remove_event));
		}
	}
	
	void activate(controllable@ c)
	{
		if(!initialised)
			return;
		
		activate_enter_exit(c);
	}
	
	void step()
	{
		if(!initialised)
			return;
		
		step_enter_exit();
	}
	
	bool can_trigger_enter_exit(controllable@ c)
	{
		if(c.player_index() != -1 && !players)
			return false;
		
		const string type = c.type_name();
		
		if(!apples && type == 'hittable_apple')
			return false;
		
		if(!enemies && (
			type == 'enemy_tutorial_square' ||
			type == 'enemy_tutorial_hexagon' ||
			type.substr(0, 6) == 'enemy_'))
			return false;
		
		return true;
	}
	
	void on_trigger_enter(controllable@ c)
	{
		if(group && entities_enter_exit_list_count > 1)
			return;
		
		message@ msg = create_message();
		msg.set_entity('broadcaster', self.as_entity());
		msg.set_entity('entity', c.as_entity());
		
		if(key != '')
		{
			msg.set_int(key, value);
		}
		
		entity@ e = entity_id != 0 ? entity_by_id(entity_id) : null;
		
		if(@e != null)
		{
			e.send_message(id, msg);
		}
		else
		{
			script.messages.broadcast(id, msg);
		}
		
		if(once)
		{
			get_scene().remove_entity(self.as_entity());
		}
	}
	
	/*void on_trigger_exit(controllable@ c)
	{
		puts('on_trigger_exit ' + (c.player_index() != -1 ? -c.player_index() : c.id()));
	}*/
	
	void on_remove_event(const string &in, message@)
	{
		get_scene().remove_entity(self.as_entity());
	}
	
}
