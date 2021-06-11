#include '../common/drawing_utils.cpp';
#include '../common/EmitterId.cpp';
#include '../common/RemoveTimer.cpp';

class BreakableWall : enemy_base, callback_base
{
	
	script@ script;
	scene@ g;
	scriptenemy@ self;
	
	[position,mode:world,layer:19,y:y1] float x1;
	[hidden] float y1;
	[position,mode:world,layer:19,y:y2] float x2;
	[hidden] float y2;
	
	[text] int block_health = 4;
	[text] string first_hit_event = '';
	[text] string activate_var = '';
	
	[hidden] array<int> block_list_health;
	
	[hidden] bool has_been_hit = false;
	[hidden] int tile_count = 0;
	
	BreakableWall()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scriptenemy@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.on_hurt_callback(this, 'on_hurt', 0);
		self.auto_physics(false);
		set_collision();
	}
	
	void set_collision()
	{
		if(!script.in_game) return;
		
		const float sx = self.x();
		const float sy = self.y();
		const float rect_x1 = floor(x1 / 48) * 48 - sx;
		const float rect_y1 = floor(y1 / 48) * 48 - sy;
		const float rect_x2 = floor(x2 / 48) * 48 + 48 - sx;
		const float rect_y2 = floor(y2 / 48) * 48 + 48 - sy;
		self.base_rectangle(
			rect_y1, rect_y2,
			rect_x1, rect_x2);
		self.hit_rectangle(
			rect_y1, rect_y2,
			rect_x1, rect_x2);
			
		const int tile_x1 = int(floor(x1 / 48));
		const int tile_y1 = int(floor(y1 / 48));
		const int tile_x2 = int(floor(x2 / 48));
		const int tile_y2 = int(floor(y2 / 48));
		
		block_list_health.resize( (tile_x2 - tile_x1 + 1) * (tile_y2 - tile_y1 + 1) );
		
		int block_index = 0;
		tile_count = 0;
		for(int x = tile_x1; x <= tile_x2; x++)
		{
			for(int y = tile_y1; y <= tile_y2; y++)
			{
				tileinfo@ tile = g.get_tile(x, y);
				if(tile.solid())
				{
					tile_count++;
					block_list_health[block_index] = block_health;
				}
				
				block_index++;
			}
		}// endfor x
	}
	
	void on_hurt(controllable@ attacked, controllable@ attacker, hitbox@ attack_hitbox, int arg)
	{
		bool has_changed = false;
		
		if(attacker.player_index() != -1)
		{
			rectangle@ r = attack_hitbox.base_rectangle();
			const int damage = attack_hitbox.damage();
			const float ax =  attack_hitbox.x();
			const float ay =  attack_hitbox.y();
			const float left = ax + r.left();
			const float right = ax + r.right();
			const float top = ay + r.top();
			const float bottom = ay + r.bottom();
			
			const int tile_x1 = int(floor(x1 / 48));
			const int tile_y1 = int(floor(y1 / 48));
			const int tile_x2 = int(floor(x2 / 48));
			const int tile_y2 = int(floor(y2 / 48));
			int block_index = 0;
			
			const float self_tx = self.x();
			const float self_ty = self.y();
			float min_x =  9999999;
			float min_y =  9999999;
			float max_x = -9999999;
			float max_y = -9999999;
			
			bool hit_block = false;
			
			for(int x = tile_x1; x <= tile_x2; x++)
			{
				for(int y = tile_y1; y <= tile_y2; y++)
				{
					const float tile_x = x * 48;
					const float tile_y = y * 48;
					
					tileinfo@ tile = g.get_tile(x, y);
					
					if(tile.solid())
					{
						if(tile_x <= right && tile_x + 48 >= left && tile_y <= bottom && tile_y + 48 >= top)
						{
							block_list_health[block_index] -= damage;
							hit_block = true;
							
							if(block_list_health[block_index] <= 0)
							{
								tile.solid(false);
								g.set_tile(x, y, 19, tile, true);
								tile_count--;
								
								entity@ emitter = create_emitter(CleansedFb1, tile_x + 24, tile_y + 24, 48, 48, 19, 12);
								g.add_entity(emitter);
								remove_timer(emitter, 1);
								has_changed = true;
							}
						}
					}
					
					if(tile.solid())
					{
						if(tile_x < min_x)
						{
							min_x = tile_x;
						}
						if(tile_y < min_y)
						{
							min_y = tile_y;
						}
						if(tile_x + 48 > max_x)
						{
							max_x = tile_x + 48;
						}
						if(tile_y + 48 > max_y)
						{
							max_y = tile_y + 48;
						}
					}
					
					block_index++;
				}
			}// endfor x
			
			if(has_changed && tile_count > 0)
			{
				self.base_rectangle(
					min_y - self_ty, max_y - self_ty,
					min_x - self_tx, max_x - self_tx);
				self.hit_rectangle(
					min_y - self_ty, max_y - self_ty,
					min_x - self_tx, max_x - self_tx);
			}
			
			if(hit_block && !has_been_hit)
			{
				if(first_hit_event != '')
				{
					message@ msg = create_message();
					broadcast_message(first_hit_event, msg);
				}
				
				has_been_hit = true;
			}
			
		}
		
		if(tile_count <= 0)
		{
			g.remove_entity(self.as_entity());
		}
	}
	
	void draw_coll()
	{
		const float tx1 = floor(x1 / 48) * 48;
		const float ty1 = floor(y1 / 48) * 48;
		const float tx2 = floor(x2 / 48) * 48 + 48;
		const float ty2 = floor(y2 / 48) * 48 + 48;
		g.draw_rectangle_world(22, 22,
			tx1, ty1, tx2, ty2,
			0, 0x44FF0000);
		outline_rect(g, tx1, ty1, tx2, ty2, 22, 22, 1, 0x88FF0000);
	}
	
	void editor_draw(float sub_frame)
	{
		draw_coll();
	}
	
}