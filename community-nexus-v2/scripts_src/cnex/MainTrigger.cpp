#include 'EasterTrigger.cpp'
#include 'ValentinesTrigger.cpp'
#include 'HalloweenTrigger.cpp'
#include 'ChristmasTrigger.cpp'

class MainTrigger : trigger_base, callback_base
{
	
	scene@ g;
	
	[position,mode:world,layer:19,y:origin_y] float origin_x;
	[hidden] float origin_y;
	[entity] uint fog_trigger;
	[text] array<Holiday> holidays;
	
	[text] int oyear = 0;
	[text] int omonth = 0;
	[text] int oday = 0;
	
	MainTrigger()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		if(!s.in_game)
			return;
		
		timedate@ date = localtime();
		int day = date.mday();
		int month = date.mon() + 1;
		int year = date.year() + 1900;
		
		if (oyear != 0) year = oyear;
		if (omonth != 0) month = omonth;
		if (oday != 0) day = oday;
		
		int days = date::days_from_civil(year, month, day);
		
		const float x = round(origin_x / 48) * 48;
		const float y = round(origin_y / 48) * 48;
		
		// puts(year + '/' + month + '/' + day);
		
		for (uint i = 0, count = holidays.length(); i < count; i++)
		{
			if (holidays[i].force)
			{
				holidays[i].check(g, days, day, month, year, x, y, fog_trigger);
				return;
			}
		}
		
		for (uint i = 0, count = holidays.length(); i < count; i++)
		{
			if (holidays[i].check(g, days, day, month, year, x, y, fog_trigger))
				break;
		}
	}
	
	void editor_draw(float sub_frame)
	{
		g.draw_rectangle_world(21,21, origin_x-4,origin_y-4, origin_x+4, origin_y+4, 0, 0xFFFF0000);
	}
	
}

class Holiday
{
	
	[text] string name;
	[persist] bool force = false;
	
	[option, 1:January, 2:February, 3:March, 4:April, 5:May, 6:June, 7:July, 8:August, 9:September, 10:October, 11:November, 12:December]
		int from_month = 1;
	[text]
		int from_day = 1;
	
	[option, 1:January, 2:February, 3:March, 4:April, 5:May, 6:June, 7:July, 8:August, 9:September, 10:October, 11:November, 12:December]
		int to_month = 1;
	[text]
		int to_day = 1;
	
	[text] bool is_easter = false;
	
	bool check(scene@ g, int days, int day, int month, int year, const float x, const float y, const uint main_fog_trigger)
	{
		if (force)
		{
			create_trigger(g, x, y);
			return true;
		}
		
		int year1 = year;
		int year2 = year;
		int month1 = from_month;
		int month2 = to_month;
		int day1 = from_day;
		int day2 = to_day;
		
		if (month2 < month1 || month1 == month2 && day2 < day1)
		{
			if (month > month1 || month == month1 && day >= day1)
				year2++;
			else
				year1--;
		}
		
		int from_days, to_days;
		
		if (is_easter)
		{
			int easter_month, easter_day;
			date::calculate_easter(year, easter_month, easter_day);
			
			easter_day = date::days_from_civil(year, easter_month, easter_day);
			from_days = easter_day + from_day;
			to_days = easter_day + to_day;
		}
		else
		{
			from_days = date::days_from_civil(year1, month1, day1);
			to_days = date::days_from_civil(year2, month2, day2);
		}
		
		// puts('  ' + year1+'/'+month1+'/'+day1+' > ' + year2+'/'+month2+'/'+day2);
		// puts('   = ' + days + ' ::: ' + from_days +' > ' + to_days);
		
		if (days >= from_days && days <= to_days)
		{
			create_trigger(g, x, y);
			return true;
		}
		
		return false;
	}
	
	void create_trigger(scene@ g, const float x, const float y)
	{
		HolidayTrigger@ trigger;
		
		if(name == 'Valentines')
			@trigger = ValentinesTrigger();
		else if(name == 'Easter')
			@trigger = EasterTrigger();
		else if(name == 'Halloween')
			@trigger = HalloweenTrigger();
		else if(name == 'Christmas')
			@trigger = ChristmasTrigger();
		else
			return;
		
		scripttrigger@ st = create_scripttrigger(trigger);
		st.set_xy(x, y);
		g.add_entity(st.as_entity(), false);
		
		// if(main_fog_trigger != 0 and fog_trigger != 0)
		// {
		// 	entity@ main_fog = entity_by_id(main_fog_trigger);
		// 	entity@ fog = entity_by_id(fog_trigger);
			
		// 	if(main_fog !is null and fog !is null)
		// 	{
		// 		main_fog.set_xy(fog.x(), fog.y());
		// 		camera@ cam = get_active_camera();
		// 		fog.set_xy(cam.x(), cam.y());
		// 	}
		// }
	}
	
}

class HolidayTrigger : trigger_base
{
	
	scripttrigger @self;
	
	void init(script @s, scripttrigger @self)
	{
		@this.self = self;
		self.editor_handle_size(4);
	}
	
}