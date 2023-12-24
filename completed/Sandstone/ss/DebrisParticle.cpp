class DebrisParticle : Particle
{
	
	float prev_rotation;
	float rotation;
	float vr;
	float rf;
	float life;
	
	Sprite@ spr;
	int layer;
	int sub_layer;
	uint32 frame;
	uint32 palette;
	float scale_x;
	float scale_y;
	
	DebrisParticle(
		const float x, const float y,
		const float rotation,
		const float vx, const float vy, const float vr,
		const float vf, const float rf,
		const float life,
		const string &in sprite_set, const string &in sprite_name, const float origin_x, const float origin_y,
		const int layer, const int sub_layer,
		const uint32 frame, const uint32 palette, const float scale_x, const float scale_y)
	{
		super(x, y);
		
		this.rotation = rotation;
		impulse_x = vx;
		impulse_y = vy;
		this.vr = vr;
		this.rf = rf;
		this.life = life;
		this.layer = layer;
		this.sub_layer = sub_layer;
		this.frame = frame;
		this.palette = palette;
		this.scale_x = scale_x;
		this.scale_y = scale_y;
		
		@spr = Sprite(sprite_set, sprite_name, origin_x, origin_y);
	}
	
	bool step(const int index, array<DebrisParticle@>@ debris)
	{
		prev_rotation = rotation;
		
		rotation += vr;
		vr *= 1 / (1 + (DT * rf));
		
		life -= DT;
		return life > 0;
	}
	
	void draw(const float sub_frame)
	{
		if(life <= 0)
			return;
		
		const float alpha = clamp01(life / 0.5);
		spr.draw(
			layer, sub_layer,
			frame, palette,
			lerp(prev_x, x, sub_frame),
			lerp(prev_y, y, sub_frame),
			lerp_angle_degrees(prev_rotation, rotation, sub_frame),
			scale_x, scale_y, 0xffffff | (uint(alpha * 255) << 24));
	}
	
}
