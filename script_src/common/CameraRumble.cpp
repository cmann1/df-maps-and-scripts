#include 'math.cpp';

class CameraRumble : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	[hidden] float t = 0;
	[hidden] float force;
	[hidden] float duration;
	
	CameraRumble()
	{
		@g = get_scene();
	}
	
	CameraRumble(float force, float duration)
	{
		@g = get_scene();
		this.force = force;
		this.duration = duration;
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
	}
	
	void step()
	{
		for(int i = int(num_cameras()) - 1; i >= 0; i--)
		{
			camera@ cam = get_camera(i);
			const float f = force * (1 - t / duration);
			cam.x(cam.x() + f * (frand() - 0.5));
			cam.y(cam.y() + f * (frand() - 0.5));
		}
		
		if(++t >= duration)
			g.remove_entity(self.as_entity());
	}
	
}

CameraRumble@ add_camera_rumble(float x, float y, float force, float duration, bool persist=false)
{
	CameraRumble@ rumble = CameraRumble(force, duration);
	scripttrigger@ st = create_scripttrigger(rumble);
	st.set_xy(x, y);
	get_scene().add_entity(@st.as_entity(), persist);
	
	return rumble;
}