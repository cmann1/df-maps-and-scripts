class script
{
	
	controllable@ player = null;
	int down_dash_timer;
	array<int> totals;
	
	textfield@ totals_txt;
	textfield@ current_txt;
	int current_display_timer = 0;
	bool level_end = false;
	
	script()
	{
		down_dash_timer = -1;
		totals.resize(13);
		
		@current_txt = create_textfield();
		current_txt.set_font('ProximaNovaReg', 26);
		current_txt.align_horizontal(0);
		current_txt.align_vertical(1);
		
		@totals_txt = create_textfield();
		totals_txt.set_font('ProximaNovaReg', 36);
		totals_txt.align_horizontal(-1);
		totals_txt.align_vertical(-1);
	}
	
	void on_level_start()
	{
	}
	
	void on_level_end()
	{
		string output = '';
		for(int i = 0, count = int(totals.size()) - 1; i < count; i++)
		{
			if(totals[i] == 0) continue;
			output += '  "' + i + '" - ' + totals[i] + '\n';
		}
		
		totals_txt.text(output);
		level_end= true;
	}
	
	void checkpoint_load()
	{
		@player = null;
		down_dash_timer = -1;
	}
	
	void step(int num_entities)
	{
		if(player is null)
		{
			@player = controller_controllable(0);
			return;
		}
		
		if(current_display_timer > 0)
			current_display_timer--;
		
		if(player.fall_intent() != 0 and !player.ground())
		{
			down_dash_timer = 1;
			return;
		}
		
		if(player.dash_intent() != 0 and down_dash_timer >= 0)
		{
			if(player.ground())
			{
				current_display_timer = 100;
				current_txt.text('Ledge cancel: ' + down_dash_timer + '');
				totals[ down_dash_timer]++;
			}
			
			down_dash_timer = -1;
			return;
		}
		
		if(down_dash_timer >= 0)
		{
			if(++down_dash_timer > 11)
				down_dash_timer = -1;
		}
	}
	
	void draw(float sub_frame)
	{
		if(level_end)
		{
			totals_txt.draw_hud(22, 22, -800 + 20, -450 + 20, 1, 1, 0);
		}
		
		if(current_display_timer > 0)
		{
			current_txt.draw_hud(22, 22, 0, 450 - 20, 1, 1, 0);
		}
	}
	
}