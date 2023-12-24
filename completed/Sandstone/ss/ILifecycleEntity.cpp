interface ILifecycleEntity
{
	
	/// Just for debugging
	string get_identifier() const;
	
	/// Is this entity loaded and being stepped
	bool get_active() const;
	void set_active(const bool value);
	/// Was run this frame. Used to determine when 
	bool get_was_run() const;
	void set_was_run(const bool value);
	/// Is this entity in view
	bool get_in_view() const;
	
	/// Called when this entity is initialised.
	/// Unlike init, this will be called after script.checkpoint_load and other all on_create on that frame
	void on_create();
	/// Called when this entity is starts stepping
	void on_activate();
	/// Called when this entity stops stepping
	void on_deactivate();
	/// Called when this entity is removed from the scene
	void on_destroy();
	/// Called when the chunk this entity is in enters one of the camera viewports
	void on_enter_view();
	/// Called when the chunk this entity is no longer visible in any of the camera viewports
	void on_leave_view();
	
	void trigger_on_leave_view();
	
	void on_checkpoint_save();
	
}

mixin class LifecycleEntity : ILifecycleEntity
{
	
	protected script@ script;
	protected bool trigger_view_events;
	
	protected bool _active;
	protected bool _was_run;
	protected bool _initialised;
	protected bool _in_view;
	
	
	float prev_x;
	float prev_y;
	float x;
	float y;
	
	protected float bounds_x1, bounds_y1, bounds_x2, bounds_y2;
	
	bool get_active() const override { return _active; }
	void set_active(const bool value) override { _active = value; }
	bool get_in_view() const override { return _in_view; }
	bool get_was_run() const override { return _was_run; }
	void set_was_run(const bool value) override { _was_run = value; }
	
	protected void init_lifecycle()
	{
		if(!_initialised)
		{
			script.register_lifecycle_entity(this);
			_initialised = true;
			on_create();
		}
		
		if(!_active)
		{
			script.queue_lifecycle_activate(this);
		}
		
		if(trigger_view_events)
		{
			bool in_view = false;
			const float x = self.x();
			const float y = self.y();
			const int cx1 = int(floor((x + bounds_x1) / (48 * 16)));
			const int cy1 = int(floor((y + bounds_y1) / (48 * 16)));
			const int cx2 = int(floor((x + bounds_x2) / (48 * 16)));
			const int cy2 = int(floor((y + bounds_y2) / (48 * 16)));
			
			
			for(int i = 0; i < script.num_players; i++)
			{
				Player@ p = script.players[i];
				if(cx2 >= p.chunk_x1 && cy2 >= p.chunk_y1 && cx1 <= p.chunk_x2 && cy1 <= p.chunk_y2)
				{
					in_view = true;
					break;
				}
			}
			
			if(in_view && !_in_view)
			{
				_in_view = true;
				if(_active)
				{
					on_enter_view();
				}
			}
			else if(!in_view && _in_view)
			{
				_in_view = false;
				on_leave_view();
			}
		}
		
		_was_run = true;
	}
	
	void on_remove()
	{
		if(!_initialised)
			return;
		
		script.unregister_lifecycle_entity(this);
		
		trigger_on_leave_view();
		
		if(_active)
		{
			_active = false;
			on_deactivate();
		}
		
		if(_initialised)
		{
			_initialised = false;
			on_destroy();
		}
	}
	
	void trigger_on_leave_view() override
	{
		if(trigger_view_events && _in_view)
		{
			_in_view = false;
			on_leave_view();
		}
	}
	
	void on_enter_view() override { }
	void on_leave_view() override { }
	void on_create() override { }
	void on_activate() override { }
	void on_deactivate() override { }
	void on_destroy() override { }
	void on_checkpoint_save() override { }
	
}

// TODO: Remove if not used
//interface IPreStepHandler
//{
//	
//	void pre_step();
//	
//}
//
//interface IPostStepHandler
//{
//	
//	void post_step();
//	
//}
