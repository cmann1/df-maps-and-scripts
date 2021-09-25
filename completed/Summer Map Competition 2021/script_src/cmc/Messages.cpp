funcdef void MessageHandler(const string &in, message@);

class Messages
{
	
	private dictionary message_handlers;
	
	void clear()
	{
		message_handlers.deleteAll();
	}
	
	void add_listener(const string &in id, MessageHandler@ handler)
	{
		array<MessageHandler@>@ handlers;
		
		if(!message_handlers.exists(id))
		{
			@handlers = array<MessageHandler@>();
			@message_handlers[id] = handlers;
		}
		else
		{
			@handlers = cast<array<MessageHandler@>@>(message_handlers[id]);
		}
		
		handlers.insertLast(handler);
	}
	
	void remove_listener(const string &in id, MessageHandler@ handler)
	{
		if(!message_handlers.exists(id))
			return;
		
		array<MessageHandler@>@ handlers = cast<array<MessageHandler@>@>(message_handlers[id]);
		const int index = handlers.findByRef(handler);
		
		if(index != -1)
		{
			handlers.removeAt(index);
		}
	}
	
	void broadcast(const string &in id, message@ msg)
	{
		if(!message_handlers.exists(id))
			return;
		
		array<MessageHandler@>@ handlers = cast<array<MessageHandler@>@>(message_handlers[id]);
		
		for(uint i = 0; i < handlers.length; i++)
		{
			handlers[i](id, msg);
		}
	}
	
}
