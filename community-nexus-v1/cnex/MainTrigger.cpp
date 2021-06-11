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
		if(!s.in_game) return;
		
		timedate@ date = localtime();
		int day = date.mday();
		int month = date.mon() + 1;
		int year = date.year() + 1900;
		
		if(oyear != 0) year = oyear;
		if(omonth != 0) month = omonth;
		if(oday != 0) day = oday;
		
		int days = days_from_civil(year, month, day);
		
		const float x = round(origin_x / 48) * 48;
		const float y = round(origin_y / 48) * 48;
		
//		puts(year + '/' + month + '/' + day);
		
		for(int i = 0, count = int(holidays.length()); i < count; i++)
		{
			holidays[i].check(g, days, day, month, year, x, y, fog_trigger);
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
	[entity] uint fog_trigger = 0;
	
	[option, 1:January, 2:February, 3:March, 4:April, 5:May, 6:June, 7:July, 8:August, 9:September, 10:October, 11:November, 12:December]
		int from_month = 1;
	[text]
		int from_day = 1;
	
	[option, 1:January, 2:February, 3:March, 4:April, 5:May, 6:June, 7:July, 8:August, 9:September, 10:October, 11:November, 12:December]
		int to_month = 1;
	[text]
		int to_day = 1;
	
	[text] bool is_easter = false;
	
	void check(scene@ g, int days, int day, int month, int year, const float x, const float y, const uint main_fog_trigger)
	{
		int year1 = year;
		int year2 = year;
		int month1 = from_month;
		int month2 = to_month;
		int day1 = from_day;
		int day2 = to_day;
		
		if( (month2 < month1 or month1 == month2 and day2 < day1) )
		{
			if(month > month1 or month == month1 and day >= day1)
				year2++;
			else
				year1--;
		}
		
		int from_days, to_days;
		
		if(is_easter)
		{
			int easter_month, easter_day;
			calculate_easter(year, easter_month, easter_day);
			
			easter_day = days_from_civil(year, easter_month, easter_day);
			from_days = easter_day + from_day;
			to_days = easter_day + to_day;
		}
		else
		{
			from_days = days_from_civil(year1, month1, day1);
			to_days = days_from_civil(year2, month2, day2);
		}
		
//		puts('  ' + year1+'/'+month1+'/'+day1+' > ' + year2+'/'+month2+'/'+day2);
//		puts('   = ' + days + ' ::: ' + from_days +' > ' + to_days);
		
		if(days < from_days or days > to_days)
		{
			return;
		}
		
//		puts('   FOUND');
		
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
		
//		if(main_fog_trigger != 0 and fog_trigger != 0)
//		{
//			entity@ main_fog = entity_by_id(main_fog_trigger);
//			entity@ fog = entity_by_id(fog_trigger);
//			
//			if(main_fog !is null and fog !is null)
//			{
//				main_fog.set_xy(fog.x(), fog.y());
//				camera@ cam = get_active_camera();
//				fog.set_xy(cam.x(), cam.y());
//			}
//		}
	}
	
	protected void calculate_easter(const int year, int &out month, int &out day)
	{
		const int A = year % 19;
		const int B = year / 100;
		const int C = year % 100;
		const int D = B / 4;
		const int E = B % 4;
		const int G = (8 * B + 13) / 25;
		const int H = (19 * A + B - D - G + 15) % 30;
		const int M = (A + 11 * H) / 319;
		const int J = C / 4;
		const int K = C % 4;
		const int L = (2 * E + 2 * J - K - H + M + 32) % 7;
		const int N = (H - M + L + 90) / 25;
		const int P = (H - M + L + N + 19) % 32;
		
		month = N;
		day = P;
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