const float FRAME_DELTA = 1.0 / 60;

class script {
	int time_prev;
	float draw_delta;
	int frame_counter;
	int frame_counter_prev;
	int frame_diff_max = 2;
	bool check_for_pause = true;
	bool level_ended;
	bool is_playing = false;
	
	void on_level_start() {
		puts('-- on_level_start -----------------------');
		time_prev = get_time_us();
		is_playing = true;
	}
	
	void on_level_end() {
		puts('-- on_level_end -----------------------');
		level_ended = true;
	}
	void step(int entities) {
		puts('step ' + frame_counter);
		
		if(level_ended)
			return;
		
		if(check_for_pause)
		{
			//puts('X ' + frame_counter_prev+' > '+frame_counter);
			if(frame_counter - frame_counter_prev > frame_diff_max)
			{
				puts('PAUSE DETECTED!!');
			}
			
			frame_counter_prev = frame_counter;
		}
		else
		{
			check_for_pause = true;
		}
	}
	
	void step_post(int entities) {
		puts(' step_post');
	}
	
	void draw(float sub_frame) {
		const int time = get_time_us();
		draw_delta += (time - time_prev) / 1000000.0;
		puts('draw ' + ((time - time_prev) / 1000.0) + 'ms');
		//puts((time - time_prev)+'');
		
		if(draw_delta >= FRAME_DELTA)
		{
			draw_delta -= FRAME_DELTA;
			frame_counter++;
		}
		
		time_prev = time;
	}
	
}
