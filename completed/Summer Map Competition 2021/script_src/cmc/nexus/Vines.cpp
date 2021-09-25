#include '../../lib/tiles/TileShape.cpp';
#include '../../lib/enums/ColType.cpp';

const float VINES_SPR_OX = 30;
const float VINES_SPR_OY = 0;

class Vines : trigger_base, callback_base, IInitHandler
{
	
	[position,mode:world,layer:19,y:y1] float x1;
	[hidden] float y1;
	[position,mode:world,layer:19,y:y2] float x2;
	[hidden] float y2;
	[persist] string name = 'flowers1';
	[persist] int count = 1;
	
	[hidden] bool open;
	[hidden] int current_count;
	
	script@ script;
	scripttrigger@ self;
	
	sprites@ spr;
	SpriteRectangle bounds;
	SpriteRectangle dead_bounds;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = script;
		@this.self = self;
		
		self.radius(0);
		
		script.init_subscribe(this);
		
		@spr = create_sprites();
		spr.add_sprite_set('script');
		calculate_sprite_bounds();
		//calculate_sprite_batch_bounds();
		
		if(!script.is_playing)
			return;
		
		if(!open)
		{
			update_tiles();
		}
	}
	
	private void calculate_sprite_bounds()
	{
		const float x = self.x() + VINES_SPR_OX;
		const float y = self.y() + VINES_SPR_OY;
		add_bounding_box(bounds, 'vines');
		bounds.shift(x, y);
		add_bounding_box(dead_bounds, 'vines_dead');
		dead_bounds.shift(x, y);
	}
	
	/*private void calculate_sprite_batch_bounds()
	{
		const float x = self.x();
		const float y = self.y();
		// 
		bounds.left = bounds.right = x;
		bounds.top = bounds.bottom = y;
		add_bounding_box(@bounds, script.sprites.vine_1_shadow, x - 140, y);
		add_bounding_box(@bounds, script.sprites.vine_1, x - 140, y);
		add_bounding_box(@bounds, script.sprites.vine_2_shadow, x, y);
		add_bounding_box(@bounds, script.sprites.vine_2, x, y);
		add_bounding_box(@bounds, script.sprites.vine_3_shadow, x + 54, y);
		add_bounding_box(@bounds, script.sprites.vine_3, x + 54, y);
		add_bounding_box(@bounds, script.sprites.vine_4_shadow, x - 34, y);
		add_bounding_box(@bounds, script.sprites.vine_4, x - 34, y);
		// 
		dead_bounds.left = dead_bounds.right = x;
		dead_bounds.top = dead_bounds.bottom = y;
		add_bounding_box(@dead_bounds, script.sprites.vines_dead_shadow, x, y);
		add_bounding_box(@dead_bounds, script.sprites.vines_dead, x, y);
	}*/
	
	private void add_bounding_box(SpriteRectangle@ bounds, SpriteBatch@ spr, const float x, const float y)
	{
		SpriteRectangle r = spr.get_rectangle(x, y);
		bounds.union_with(@r);
	}
	
	private void add_bounding_box(SpriteRectangle@ bounds, const string &in sprite_name)
	{
		rectangle@ r = spr.get_sprite_rect(sprite_name, 0);
		bounds.union_with(r);
	}
	
	void init() override
	{
		if(!script.is_playing)
			return;
		
		script.messages.add_listener(name, MessageHandler(on_dust));
	}
	
	void draw(float sub_frame)
	{
		SpriteRectangle@ r = open ? @dead_bounds : @bounds;
		if(!script.should_draw(r.left, r.top, r.right, r.bottom))
			return;
		
		const float x = self.x();
		const float y = self.y();
		
		if(!open)
		{
			spr.draw_world(18, 23, 'vines', 0, 0, x + VINES_SPR_OX, y + VINES_SPR_OY, 0, 1, 1, 0xffffffff);
			//draw_vine(script.sprites.vine_1_shadow, script.sprites.vine_1,
			//	x - 140, y);
			//draw_vine(script.sprites.vine_2_shadow, script.sprites.vine_2,
			//	x, y);
			//draw_vine(script.sprites.vine_3_shadow, script.sprites.vine_3,
			//	x + 54, y);
			//draw_vine(script.sprites.vine_4_shadow, script.sprites.vine_4,
			//	x - 34, y);
		}
		else
		{
			spr.draw_world(16, 6, 'vines_dead', 0, 0, x + VINES_SPR_OX, y + VINES_SPR_OY, 0, 1, 1, 0xffffffff);
			//script.sprites.vines_dead_shadow.draw(x, y, 0x77ffffff);
			//script.sprites.vines_dead.draw(x, y);
		}
		
		//outline_rect(script.g, 22, 24, bounds.left, bounds.top, bounds.right, bounds.bottom, 2, 0x22ffffff);
		//outline_rect(script.g, 22, 24, dead_bounds.left, dead_bounds.top, dead_bounds.right, dead_bounds.bottom, 2, 0x22ff0000);
	}
	
	private void draw_vine(SpriteBatch@ shadow, SpriteBatch@ vines, const float x, const float y)
	{
		shadow.draw(x, y);
		vines.draw(x, y);
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		const float x1 = floor(this.x1 / 48) * 48;
		const float y1 = floor(this.y1 / 48) * 48;
		const float x2 = ceil(this.x2 / 48) * 48;
		const float y2 = ceil(this.y2 / 48) * 48;
		outline_rect(script.g, 22, 22, x1, y1, x2, y2, 1 * script.ed_zoom, 0xffff3333);
	}
	
	private void update_tiles()
	{
		const int tx1 = floor_int(this.x1 / 48);
		const int ty1 = floor_int(this.y1 / 48);
		const int tx2 = ceil_int(this.x2 / 48);
		const int ty2 = ceil_int(this.y2 / 48);
		tileinfo@ tile = create_tileinfo();
		tile.type(TileShape::Full);
		tile.solid(!open);
		tile.sprite_set(0);
		tile.sprite_tile(0);
		tile.sprite_palette(0);
		
		for(int x = tx1; x < tx2; x++)
		{
			for(int y = ty1; y < ty2; y++)
			{
				script.g.set_tile(x, y, 19, tile, true);
			}
		}
	}
	
	private void on_dust(const string &in id, message@ msg)
	{
		if(msg.get_string('type') != 'dust')
			return;
		
		current_count = clamp(current_count + 1, 0, count);
		
		if(current_count == count)
		{
			open = true;
			update_tiles();
			message@ new_msg = create_message();
			new_msg.set_string('type', 'vines');
			script.messages.broadcast(name, new_msg);
			
			const float x1 = floor(this.x1 / 48) * 48;
			const float y1 = floor(this.y1 / 48) * 48;
			const float x2 = ceil(this.x2 / 48) * 48;
			const float y2 = ceil(this.y2 / 48) * 48;
			
			int i = script.g.get_entity_collision(y1, y2, x1, x2, ColType::Emitter);
			while(i-- > 0)
			{
				entity@ e = script.g.get_entity_collision_index(i);
				if(e.type_name() != 'entity_emitter')
					continue;
				
				varstruct@ vars = e.vars();
				if(vars.get_var('emitter_id').get_int32() != EmitterId::KingZoneRed)
					continue;
				
				script.g.remove_entity(e);
			}
		}
	}
	
}
