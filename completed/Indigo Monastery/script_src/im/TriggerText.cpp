//#include '../common/utils.cpp'

class TriggerText : trigger_base, callback_base
{
	
	script@ script;
	scene@ g;
	scripttrigger@ self;
	
	[entity] uint text_id;
	[text] string event;
	
	TriggerText()
	{
		@g = get_scene();
		
		add_broadcast_receiver(event, this, 'on_message');
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
	}
	
	void on_message(string id, message@ msg)
	{
		entity@ text_trigger = entity_by_id(text_id);
		if(@text_trigger != null)
		{
			text_trigger.set_xy(self.x(), self.y());
		}
		
		g.remove_entity(self.as_entity());
	}
	
	void editor_draw(float sub_frame)
	{
		entity@ text_trigger = entity_by_id(text_id);
		if(@text_trigger != null)
		{
			draw_arrow(g, 22, 22, self.x(), self.y(), text_trigger.x(), text_trigger.y(), 4, 20, 0.5, 0x99FF0000);
		}
	}
	
}