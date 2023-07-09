#include '../lib/enums/ColType.cpp';

class PlayerSpawner : trigger_base
{
	
	[persist] int player_index;
	[persist] string character;
	[persist] int facing;
	[persist] int skill_combo;
	
	private DLScript@ script;
	private scripttrigger@ self;
	
	private dustman@ player;
	
	PlayerSpawner() {}
	
	PlayerSpawner(scene@ g, dustman@ player)
	{
		if(player.player_index() == -1)
			return;
		
		dustman@ dm = player.as_dustman();
		if(@dm == null)
			return;
		
		skill_combo = dm.skill_combo();
		dm.combo_timer(0);
		dm.combo_count(0);
		
		player_index = player.player_index();
		character = dm.character();
		character = character.substr(0, 4) + '_' + character.substr(4);
		facing = player.face();
		@this.player = player;
		
		scripttrigger@ st = create_scripttrigger(@this);
		st.set_xy(player.x(), player.y());
		g.add_entity(st.as_entity(), true);
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		self.editor_colour_inactive(0xff22ff22);
		self.editor_colour_circle(0xff22ff22);
		self.editor_colour_active(0xff66ff66);
		
		self.radius(0);
		
		if(@player == null)
		{
			entity@ e = create_entity(character);
			@player = @e != null ? e.as_dustman() : null;
			
			if(@player != null)
			{
				player.set_xy(self.x(), self.y());
				player.face(facing);
				player.ai_disabled(true);
				player.team(Team::Cleaner);
				script.g.add_entity(e, false);
			}
		}
	}
	
	void on_add()
	{
		PlayerData@ player = script.players[player_index];
		@player.player_spawner = this;
	}
	
	void on_remove()
	{
		PlayerData@ player = script.players[player_index];
		if(@player.player_spawner == @this)
		{
			@player.player_spawner = null;
		}
	}
	
	void take_control()
	{
		player.skill_combo(skill_combo);
		player.combo_timer(0);
		controller_entity(player_index, player.as_controllable());
		
		script.g.remove_entity(self.as_entity());
	}
	
}
