class FinalApple : trigger_base, callback_base
{
	
	[entity] uint apple_id;
	
	script@ script;
	scripttrigger@ self;
	
	controllable@ apple;
	int sign;
	Sprite spr('props3', 'backdrops_3');
	canvas@ canvas;
	entity@ emitter;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		@apple = controllable_by_id(apple_id);
		sign = script.is_playing ? script.players[0].chirp_symbol : 1;
		@canvas = create_canvas(false, Player::SignLayer, Player::SignSubLayer);
		
		if(script.is_playing)
		{
			@emitter = create_emitter(PARTICLES_EMITTER_ID, self.x(), self.y(), 48, 48, Player::SignLayer, Player::SignFxSubLayer);
			script.g.add_entity(emitter, false);
			
			if(@apple != null)
			{
				apple.on_hurt_callback(this, 'on_apple_hurt', 0);
			}
		}
	}
	
	void on_apple_hurt(controllable@ attacked, controllable@ attacker, hitbox@ attack_hitbox, int arg)
	{
		if(@attacker == null || attacker.player_index() == -1)
			return;
		
		script.players[attacker.player_index()].give_super();
	}
	
	void step()
	{
		if(@apple != null)
		{
			self.set_xy(apple.x(), apple.y());
			
			if(apple.destroyed())
			{
				script.g.remove_entity(self.as_entity());
				if(@emitter != null)
				{
					script.g.remove_entity(emitter);
				}
				@apple = null;
			}
		}
		
		if(@emitter != null)
		{
			emitter.set_xy(self.x(), self.y());
		}
	}
	
	void draw(float sub_frame)
	{
		if(@apple == null)
			return;
		
		const float x = @apple != null ? lerp(apple.prev_x(), apple.x(), sub_frame) : self.x();
		const float y = @apple != null ? lerp(apple.prev_y(), apple.y(), sub_frame) : self.y();
		
		float ox = 2 + apple.draw_offset_x();
		float oy = 0 + apple.draw_offset_y();
		
		switch(@apple != null ? apple.state() : 0)
		{
			case EntityState::Idle:
				oy += -1;
				break;
			case EntityState::Stun:
				ox += 2;
				oy += -1;
				break;
			case EntityState::Fall:
				oy += 16;
				break;
			default:
				break;
		}
		
		const float s = Player::SignSizeMin;
		const uint sign_clr = 0xffffffff;
		draw_sign(canvas, @spr, sign, x - 2*s + ox, y - 2*s + oy, s, 0, sign_clr);
		draw_sign(canvas, @spr, (sign+ 155) % 11, x + 2*s + ox, y - 2*s + oy, s, 0, sign_clr);
		draw_sign(canvas, @spr, (sign+ 456) % 11, x + 2*s + ox, y + 2*s + oy, s, 0, sign_clr);
		draw_sign(canvas, @spr, (sign+ 992) % 11, x - 2*s + ox, y + 2*s + oy, s, 0, sign_clr);
		
		// Glow
		const float scale_x = 0.15;
		const float scale_y = 0.15;
		float px, py;
		spr.real_position(x, y, 0, px, py, scale_x, scale_y);
		canvas.draw_sprite(spr.sprite, spr.sprite_name, 0, 0, px, py, 0, scale_x, scale_y, 0xaaffffff);
		spr.real_position(x, y, 90, px, py, scale_x, scale_y);
		canvas.draw_sprite(spr.sprite, spr.sprite_name, 0, 0, px, py, 90, scale_x, scale_y, 0xaaffffff);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
	
}
