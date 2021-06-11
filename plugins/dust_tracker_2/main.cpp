#include '../lib/std.cpp';
#include '../lib/embed_utils.cpp';
#include '../lib/math/math.cpp';
#include '../lib/drawing/common.cpp';
#include '../lib/drawing/common.cpp';
#include 'EnemyData.cpp';
#include 'TileData.cpp';
#include 'functions.cpp';
// TODO: REMOVE
//#include '../lib/debug/Debug.cpp';

const string EMBED_BASE					= 'dust_tracker_2/images/';
const string EMBED_spr_arrow_red		= EMBED_BASE + 'arrow_red.png';
const string EMBED_spr_arrow_yellow		= EMBED_BASE + 'arrow_yellow.png';
const string EMBED_spr_dust_block		= EMBED_BASE + 'dust_block.png';

const float ARROW_LENGTH_MIN = 60;
const float ARROW_LENGTH_MAX = 40;
const float ARROW_DISTANCE_MIN = 900;
const float ARROW_DISTANCE_MAX = 2000;
const float ARROW_SIZE_MIN = 20;
const float ARROW_SIZE_MAX = 15;
const float ARROW_SCALE_MIN = 1;
const float ARROW_SCALE_MAX = 0.75;
const float SCREEN_PADDING = 50;
const uint TILE_INNER_COLOUR = 0xFFFFFFFF;
const uint TILE_OUTER_COLOUR = 0xFFFF5555;
const uint TILE_CLEAN_INNER_COLOUR = 0xffaaaaaa;
const uint TILE_CLEAN_OUTER_COLOUR = 0xff770000;
const uint DUSTBLOCK_INNER_COLOUR = 0xFFFFFFFF;
const uint DUSTBLOCK_OUTER_COLOUR = 0xffFF5555;
const float INNER_WIDTH = 2;
const float OUTER_WIDTH = 6;
const float TILE_INNER_WIDTH = 4;
const float TILE_OUTER_WIDTH = 9;
const float WIDTH_DIFF = OUTER_WIDTH - INNER_WIDTH;

/// How many cells around the player to check
const int player_check_grid_size = 3;
const int inner_check_grid_size = 6;
const int outer_check_grid_size = 12;
/// How many cells in the small grid around the player to check each frame
const int player_cells_per_frame = 7;
const int inner_cells_per_frame = 3;
const int outer_cells_per_frame = 2;

class script
{

	// TODO: REMOVE
//	Debug debug;
	scene@ g;
	sprites@ spr;
	int cell_tile_count = 4;
	float cell_size = cell_tile_count * 48;
	int check_player_x = -player_check_grid_size;
	int check_player_y = -player_check_grid_size;
	int check_inner_x = -inner_check_grid_size;
	int check_inner_y = -inner_check_grid_size;
	int check_outer_x = -outer_check_grid_size;
	int check_outer_y = -outer_check_grid_size;
	array<int> cell_queue((player_cells_per_frame + inner_cells_per_frame + outer_cells_per_frame) * 3);
	int cell_queue_index;
	controllable@ player = null;
	camera@ cam;
	
	TileData@ new_tile_data = TileData();

	dictionary dust;
	dictionary dust_checkpoint;
	dictionary enemies;
	dictionary enemies_checkpoint;
	int dust_view_index = -2;
	int dust_current_view_index = -2;
	array<string>@ dust_keys;
	int num_dust_keys = 0;
	array<string>@ enemies_keys;
	int num_enemies_keys = 0;
	
	float view_x;
	float view_y;
	float view_x1;
	float view_y1;
	float view_x2;
	float view_y2;

	script()
	{
		@g = get_scene();
		@cam = get_camera(0);
		
		@spr = create_sprites();
	}
	
	void build_sprites(message@ msg)
	{
		build_sprite(msg, 'arrow_red',		66, 24);
		build_sprite(msg, 'arrow_yellow',	66, 24);
		build_sprite(msg, 'dust_block',		4, 4);
	}
	
	void on_level_start()
	{
		spr.add_sprite_set('script');
	}

	void checkpoint_save()
	{
//		puts('>>> checkpoint_save');
		dust_checkpoint = dust;
		enemies_checkpoint = enemies;
	}

	int enemy_validation_timer = 0;
	
	void checkpoint_load()
	{
//		puts('>>> checkpoint_load');
		dust = dust_checkpoint;
		enemies = enemies_checkpoint;
		dust_view_index = -2;
		dust_current_view_index = -2;
		@dust_keys = null;
		num_dust_keys = 0;
		@dust_keys = enemies_keys;
		num_enemies_keys = 0;
		
		validate_enemies();
		enemy_validation_timer = 1;
		
		@player = null;
	}

	void entity_on_remove(entity @e)
	{
		if(enemies.exists(e.id() + ''))
		{
//			puts('DELETING ' + e.type_name()+'-'+e.id());
			enemies.delete(e.id() + '');
		}
	}

	void move_cameras()
	{
		if(dust_view_index > -1 and dust_current_view_index != dust_view_index)
		{
			float x;
			float y;

			if(dust_view_index < num_dust_keys)
			{
				const string key = dust_keys[dust_view_index];
				const int index = key.findFirst(',');
				x = parseFloat(key.substr(0, index)) * 48;
				y = parseFloat(key.substr(index + 1)) * 48;
			}
			else
			{
				const EnemyData@ e = cast<EnemyData>(enemies[enemies_keys[dust_view_index - num_dust_keys]]);
				x = e.ex();
				y = e.ey();
			}

			cam.x(x);
			cam.y(y);
			dust_current_view_index = dust_view_index;
		}
	}

	void validate_enemies()
	{
		@enemies_keys = enemies.getKeys();
		num_enemies_keys = int(enemies_keys.length());
		
		for(int i = 0; i < num_enemies_keys; i++)
		{
			const string key = enemies_keys[i];
			EnemyData@ e = cast<EnemyData>(enemies[key]);
			@e.enemy = entity_by_id(e.id);
		}
	}
	
	void step(int entities)
	{
		if(enemy_validation_timer > 0 && enemy_validation_timer++ == 2)
		{
			validate_enemies();
			enemy_validation_timer = -1;
		}
		
		if(player is null)
		{
			@player = controller_controllable(0);
			@cam = get_camera(0);
		}
		
		if(player is null)
			return;
		
		view_dust();
		
		const int player_cell_x = int(floor((player.x()) / cell_size));
		const int player_cell_y = int(floor((player.y() - 48) / cell_size));
		cell_queue_index = 0;
		queue_player_dust(player_cell_x, player_cell_y);
		queue_inner_dust(player_cell_x, player_cell_y);
		queue_outer_dust(player_cell_x, player_cell_y);
		check_dust();
		
		check_enemies(entities);
		
//		debug.step();
	}
	
	void view_dust()
	{
		if(player.taunt_intent() != 1)
			return;
		
		if(dust_view_index == -2)
		{
			puts('==== MISSED DUST ==============================================');
			@dust_keys = dust.getKeys();
			num_dust_keys = int(dust_keys.length());
			int missed_dust = 0;
			for(int i = 0; i < num_dust_keys; i++)
			{
				const string key = dust_keys[i];
				const int index = key.findFirst(',');
				const float x = parseFloat(key.substr(0, index)) * 48;
				const float y = parseFloat(key.substr(index + 1)) * 48;
				const int dust_count = int(dust[key]);
				missed_dust += dust_count;
				puts(key + ' (' + x + ',' + y + ')');
			}
			puts('   === TOTAL: ' + missed_dust);

			puts('==== MISSED ENEMIES ==============================================');
			@enemies_keys = enemies.getKeys();
			num_enemies_keys = int(enemies_keys.length());
			for(int i = 0; i < num_enemies_keys; i++)
			{
				const string key = enemies_keys[i];
				const EnemyData@ e = cast<EnemyData>(enemies[key]);
				puts('  ' + key + ' (' + e.ex() + ',' + e.ey() + ')');
			}
		}
		else if(dust_view_index == -1)
		{
			@dust_keys = dust.getKeys();
			num_dust_keys = int(dust_keys.length());
			@enemies_keys = enemies.getKeys();
			num_enemies_keys = int(enemies_keys.length());
			cam.script_camera(true);
		}

		if(++dust_view_index >= num_dust_keys + num_enemies_keys)
		{
			dust_view_index = -2;
			dust_current_view_index = -2;
			@dust_keys = null;
			num_dust_keys = 0;
			@dust_keys = enemies_keys;
			num_enemies_keys = 0;
			cam.script_camera(false);
		}
	}
	
	/// A small area around the player that is updated more frequently
	void queue_player_dust(const int player_cell_x, const int player_cell_y)
	{
		for(int i = 0; i < player_cells_per_frame; i++)
		{
			const int cell_x = (player_cell_x + check_player_x) * cell_tile_count;
			const int cell_y = (player_cell_y + check_player_y) * cell_tile_count;
			
			cell_queue[cell_queue_index++] = cell_x;
			cell_queue[cell_queue_index++] = cell_y;
			cell_queue[cell_queue_index++] = 0;
			
			if(++check_player_x > player_check_grid_size)
			{
				check_player_x = -player_check_grid_size;
				
				if(++check_player_y > player_check_grid_size)
				{
					check_player_y = -player_check_grid_size;
				}
			}
		}
	}
	
	/// A slightly larger area that is updated less often
	void queue_inner_dust(const int player_cell_x, const int player_cell_y)
	{
		for(int i = 0; i < inner_cells_per_frame; i++)
		{
			const int cell_x = (player_cell_x + check_inner_x) * cell_tile_count;
			const int cell_y = (player_cell_y + check_inner_y) * cell_tile_count;
			
			cell_queue[cell_queue_index++] = cell_x;
			cell_queue[cell_queue_index++] = cell_y;
			cell_queue[cell_queue_index++] = 1;
			
			check_inner_x++;
			
			if(check_inner_x == -player_check_grid_size && check_inner_y >= -player_check_grid_size && check_inner_y <= player_check_grid_size)
			{
				check_inner_x = player_check_grid_size + 1;
			}
			else if(check_inner_x > inner_check_grid_size)
			{
				check_inner_x = -inner_check_grid_size;
				
				check_inner_y++;
				
				if(check_inner_y > inner_check_grid_size)
				{
					check_inner_y = -inner_check_grid_size;
				}
			}
		}
	}
	
	/// A very large area that is updated infrequently
	void queue_outer_dust(const int player_cell_x, const int player_cell_y)
	{
		for(int i = 0; i < outer_cells_per_frame; i++)
		{
			const int cell_x = (player_cell_x + check_outer_x) * cell_tile_count;
			const int cell_y = (player_cell_y + check_outer_y) * cell_tile_count;
			
			cell_queue[cell_queue_index++] = cell_x;
			cell_queue[cell_queue_index++] = cell_y;
			cell_queue[cell_queue_index++] = 2;
			
			check_outer_x++;
			
			if(check_outer_x == -inner_check_grid_size && check_outer_y >= -inner_check_grid_size && check_outer_y <= inner_check_grid_size)
			{
				check_outer_x = inner_check_grid_size + 1;
			}
			else if(check_outer_x > outer_check_grid_size)
			{
				check_outer_x = -outer_check_grid_size;
				
				check_outer_y++;
				
				if(check_outer_y > outer_check_grid_size)
				{
					check_outer_y = -outer_check_grid_size;
				}
			}
		}
	}
	
	void check_dust()
	{
//		const int player_cell_x = int(floor((player.x()) / cell_size));
//		const int player_cell_y = int(floor((player.y() - 48) / cell_size));
//		
//		debug.rect(22, 22,
//			(player_cell_x - player_check_grid_size) * cell_size,
//			(player_cell_y - player_check_grid_size) * cell_size,
//			(player_cell_x + player_check_grid_size + 1) * cell_size,
//			(player_cell_y + player_check_grid_size + 1) * cell_size,
//			0, 5, 0x55ffffff,
//			true, 1);
//		
//		debug.rect(22, 22,
//			(player_cell_x - inner_check_grid_size) * cell_size,
//			(player_cell_y - inner_check_grid_size) * cell_size,
//			(player_cell_x + inner_check_grid_size + 1) * cell_size,
//			(player_cell_y + inner_check_grid_size + 1) * cell_size,
//			0, 5, 0x55ff0000,
//			true, 1);
//		
//		debug.rect(22, 22,
//			(player_cell_x - outer_check_grid_size) * cell_size,
//			(player_cell_y - outer_check_grid_size) * cell_size,
//			(player_cell_x + outer_check_grid_size + 1) * cell_size,
//			(player_cell_y + outer_check_grid_size + 1) * cell_size,
//			0, 5, 0x5500ff00,
//			true, 1);
		
		for(int i = cell_queue_index - 3; i >= 0; i -= 3)
		{
			const int cell_x = cell_queue[i];
			const int cell_y = cell_queue[i + 1];
			const int type = cell_queue[i + 2];
			
//			debug.rect(22, 22,
//				cell_x * 48,
//				cell_y * 48,
//				cell_x * 48 + cell_size,
//				cell_y * 48 + cell_size, 0, 5,
//				type == 0 ? 0xffffffff : (type == 1 ? 0xffff0000 : 0xff00ff00),
//				true, 1);
			
			for(int tx = 0; tx < cell_tile_count; tx++)
			{
				const int tile_x = cell_x + tx;

				for(int ty = 0; ty < cell_tile_count; ty++)
				{
					const int tile_y = cell_y + ty;
					const string key = tile_x + ',' + tile_y;
					
					bool exists = dust.exists(key);
					TileData@ tile_data = exists ? cast<TileData>(dust[key]) : @new_tile_data;
					
					int old_dust_count = tile_data.dust_count;
					tile_data.update(g, tile_x, tile_y);
					
					if(tile_data.dust_count != old_dust_count || old_dust_count == 0 && exists)
					{
						if(tile_data.dust_count == 0)
						{
							dust.delete(key);
						}
						else
						{
							@dust[key] = @tile_data;
							@new_tile_data = TileData();
						}
					}
				}
			}
		}
	}
	
	void check_enemies(int num_entities)
	{
		for(int i = 0; i < num_entities; i++)
		{
			entity@ e = entity_by_index(i);
			const string key = e.id() + '';

			if(e.type_name().substr(0, 5) == 'enemy' and !enemies.exists(key))
			{
//				puts('ADDING ' + e.type_name()+'-'+e.id());
				rectangle@ r = e.base_rectangle();
				@enemies[key] = EnemyData(e, r.left(), r.top(), r.right(), r.bottom());
			}
		}
	}
	
	void draw(float sub_frame)
	{
		if(cam is null)
			return;

		float hh = cam.screen_height() * 0.5;
		float hw = hh * SCREEN_WIDTH_F;
		view_x = cam.x();
		view_y = cam.y();
		const float screen_x1 = view_x - hw;
		const float screen_y1 = view_y - hh;
		const float screen_x2 = view_x + hw;
		const float screen_y2 = view_y + hh;
		view_x1 = screen_x1 + SCREEN_PADDING;
		view_y1 = screen_y1 + SCREEN_PADDING;
		view_x2 = screen_x2 - SCREEN_PADDING;
		view_y2 = screen_y2 - SCREEN_PADDING;

//		outline_rect(g, 20, 22, view_x1 - SCREEN_PADDING, view_y1 - SCREEN_PADDING, view_x2 + SCREEN_PADDING, view_y2 + SCREEN_PADDING, 2, 0xFFFF0000);
//		outline_rect(g, 20, 22, view_x1, view_y1, view_x2, view_y2, 2, 0xFFFFFFFF);

		@dust_keys = dust.getKeys();
		num_dust_keys = int(dust_keys.length());

		for(int i = 0; i < num_dust_keys; i++)
		{
			const string key = dust_keys[i];
			TileData@ tile_data = cast<TileData>(dust[key]);
			tile_data.render(g, spr,
				view_x, view_y,
				screen_x1, screen_y1, screen_x2, screen_y2,
				view_x1, view_y1, view_x2, view_y2);
		}

		@enemies_keys = enemies.getKeys();
		num_enemies_keys = int(enemies_keys.length());

		for(int i = 0; i < num_enemies_keys; i++)
		{
			const string key = enemies_keys[i];
			const EnemyData@ e = cast<EnemyData>(enemies[key]);
			outline_entity(e.ex(e.l), e.ey(e.t), e.ex(e.r), e.ey(e.b));
		}
		
//		debug.draw(sub_frame);
	}

	void outline_entity(float x1, float y1, float x2, float y2)
	{
		if(x1 <= view_x2 && x2 >= view_x1 && y1 <= view_y2 && y2 >= view_y1)
		{
			outline_rect(g, 20, 22, x1, y1, x2, y2, 4, 0xFFCCCC00);
			outline_rect(g, 20, 22, x1, y1, x2, y2, 1, 0xFFFFFFFF);
		}
		
		draw_dust_arrow(g, spr, 'arrow_yellow',
			x1, y1, x2, y2,
			view_x, view_y,
			view_x1, view_y1, view_x2, view_y2);
	}
	
}








