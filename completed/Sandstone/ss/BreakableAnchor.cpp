#include '../lib/drawing/Sprite.cpp';
#include '../lib/drawing/SpriteGroup.cpp';

namespace BreakableAnchor
{
	
	const float AttachmentOffset = 28;
	const int Layer = 15;
	const int SubLayer = 9;
	const float Scale = 0.7218;
	
	const int BreakEmitterId = EmitterId::SlimeGroundCreate;
	
}

class BreakableAnchor :
	enemy_base, callback_base,
	LifecycleEntity, ConstraintTracker, IRopeAttachment
{
	
	[hidden] bool broken;
	
	script@ script;
	scriptenemy@ self;
	Sprite spr('props1', 'chains_5', 0.5, 0.3);
	sprites@ broken_spr;
	
	Rope@ rope;
	PositionConstraint@ attachment;
	
	string get_identifier() const override { return 'BreakableAnchor' + self.id(); }
	
	void init(script@ script, scriptenemy@ self)
	{
		@this.self = self;
		@this.script = script;
		@spring_system = script.spring_system;
		
		@broken_spr = create_sprites();
		broken_spr.add_sprite_set('script');
		
		self.auto_physics(false);
		
		if(broken)
		{
			self.hit_collision().remove();
		}
		
		const float s = 10;
		self.base_rectangle(-s, s, -s, s);
		self.hit_rectangle(-s, s, -s, s);
		
		self.on_hurt_callback(this, 'on_hurt', 0);
	}
	
	void on_create() override
	{
		create_constraints();
	}
	
	void on_activate() override
	{
		create_constraints();
	}
	
	void on_deactivate() override
	{
		remove_constraints();
	}
	
	void on_destroy()
	{
		remove_constraints();
	}
	
	void on_hurt(controllable@ attacked, controllable@ attacker, hitbox@ attack_hitbox, int arg)
	{
		if(broken)
			return;
		if(attack_hitbox.damage() < 10000)
			return;
		
		self.hit_collision().remove();
		broken = true;
		
		script.add_emitter_burst(
			BreakableAnchor::BreakEmitterId,
			19, 7,
			self.x(), self.y(),
			30, 50, 90, 0.25, 2);
		
		if(@rope != null)
		{
			rope.detach(this);
			script.spring_system.remove_constraint(attachment);
			constraints.removeAt(constraints.findByRef(attachment));
			@attachment = null;
		}
	}
	
	void step()
	{
		self.base_collision().remove();
		init_lifecycle();
	}
	
	void draw(float sub_frame)
	{
		if(!broken)
		{
			spr.draw(
				BreakableAnchor::Layer, BreakableAnchor::SubLayer, 0, 0,
				self.x(), self.y(), 180, BreakableAnchor::Scale, BreakableAnchor::Scale);
		}
		else
		{
			broken_spr.draw_world(
				BreakableAnchor::Layer, BreakableAnchor::SubLayer, 'anchor_broken', 0, 0,
				self.x(), self.y(), 5, 1, 1, 0xffffffff);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		float x, y;
		get_attachment_point(x, y);
		script.g.draw_rectangle_world(22, 22, x - 2, y - 2, x + 2, y + 2, 45, 0xffff0000);
	}
	
	void get_attachment_point(float &out x, float &out y) const override
	{
		const float angle = self.rotation() * DEG2RAD;
		const float nx = cos(angle);
		const float ny = sin(angle);
		
		x = self.x() + nx * BreakableAnchor::AttachmentOffset;
		y = self.y() + ny * BreakableAnchor::AttachmentOffset;
	}
	
	float get_attachment_rotation() const override
	{
		return self.rotation();
	}
	
	void get_position(float &out x, float &out y) const override
	{
		x = self.x();
		y = self.y();
	}
	
	void attach(Rope@ rope, Particle@ p) override
	{
		@this.rope = rope;
		
		if(@attachment != null)
			return;
		
		float x, y;
		get_attachment_point(x, y);
		@attachment = PositionConstraint(p, x, y);
		script.spring_system.add_constraint(attachment);
		track_constraint(attachment);
	}
	
}
