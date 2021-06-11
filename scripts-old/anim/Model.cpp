#include '../common/math.cpp'
#include '../common/ByteArray.cpp'
#include 'StringTable.cpp'
#include 'Node.cpp'
#include 'Bone.cpp'
#include 'drawables/Drawable.cpp'
#include 'anim/Animation.cpp'

class Model : trigger_base
{
	
	scene@ g;
	script@ script;
	scripttrigger@ self;
	
	canvas@ c;
	
	Bone root('root');
	
	[text] string current_anim_name = '';
	[text] bool is_playing = true;
	[text] float playback_speed = 1;
	
	[angle] float rotation = 0;
	[text] float scale_x = 1;
	[text] float scale_y = 1;
	[text] bool show_bones = false;
	
	[hidden] float current_time = 0;
	[hidden] int current_frame = -2;
	[hidden] int target_frame = 0;
	[hidden] float fps_step = 0;
	
	dictionary animations;
	Animation@ current_anim = null;
	
	array<Bone@> bones;
	int num_bones = 0;
	array<Node@> nodes;
	int num_nodes = 0;
	
	Model()
	{
		@g = get_scene();
		@c = create_canvas(false, 12, 19);
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = @script;
		@this.self = @self;
		
		self.editor_handle_size(8);
		
		Bone bone2('bone2');
		Bone bone3('bone3');
		Bone bone4('bone4');
		Bone bone5('bone5');
		Bone bone6('bone6');
		
		root.addDrawable(Sprite('flag', 'cidle'));
		root.addChild(@bone2);
		root.addChild(@bone6);
		Sprite@ spr = cast<Sprite>(bone2.addDrawable(Sprite('props1', 'backdrops_5')));
		spr.y = 85;
		bone2.rotation = 20;
		bone3.rotation = 40;
		bone4.rotation = 60;
		bone5.rotation = 80;
		bone5.rotation = 30;
		bone2.addChild(@bone3);
		bone3.addChild(@bone4);
		bone4.addChild(@bone5);
		bone3.rotation = 45;
		bone3.addDrawable(Rect(0, 0, 100, 100, 0, 0xAAFF0000));
		Drawable@ text = bone3.addDrawable(Text(10, 10, 'XZua-23489UHo8'));
		
		array<Drawable@> lines;
		for(float y = -30; y <= 30; y += 15)
		{
			lines.insertLast(bone2.addDrawable(Line(100, 0, 150 - abs(y) * 0.5, y, 2, 0xFF00FF00)));
		}
		
		build();
		
		Animation@ anim = Animation('Default', this);
		anim.fps = 30;
		anim.length = 180;
		add_animation(anim);
		anim.loop = true;
		anim.skip_last_frame = true;
		
		Track@ track;
		
		@track = anim.addTrack(root);
		track.addKeyFrame(0, 0, 0, 1, 1, 0);
		track.addKeyFrame(89, 0, 0, 1, 1, 180);
		track.addKeyFrame(179, 0, 0, 1, 1, 0);
		
		@track = anim.addTrack(bone3);
		track.addKeyFrame(0, 0, 0, 1, 1, 45);
		track.addKeyFrame(59, 0, 0, 1, 1, 90);
		track.addKeyFrame(119, 0, 0, 1, 1, 45);
		
		@track = anim.addTrack(bone2);
		track.addKeyFrame(0, 0, 0, 1, 1, 20);
		track.addKeyFrame(59, 0, 0, 1, 1, -90);
		track.addKeyFrame(119, 0, 0, 1, 1, 20);
		
		@track = anim.addTrack(text);
		track.addKeyFrame(0, 10, 10, 1, 1, 0);
		track.addKeyFrame(89, 10, 10, 0.5714285, 0.5714285, 0);
		track.addKeyFrame(179, 10, 10, 1, 1, 0);
		
		for(int i = 0; i < 5; i++)
		{
			float t = i / 4.0 - 0.5;
			@track = anim.addTrack(lines[i]);
			track.addKeyFrame(0, 0, 0, 1, 1, 0);
			track.addKeyFrame(29, 20, t * 60, 1, 1, 0);
			track.addKeyFrame(59, 0, 0, 1, 1, 0);
		}
		
		set_animation(current_anim_name);
	}
	
	void build()
	{
		array<Bone@> bones_stack = {root};
		int bones_stack_size = 1;
		int bones_stack_index = 1;
		
		bones.resize(0);
		num_bones = 0;
		
		nodes.resize(0);
		num_nodes = 0;
		
		while(bones_stack_index > 0)
		{
			Bone@ bone = bones_stack[--bones_stack_index];
			
			if(bone is null)
			{
				bones.insertLast(null);
				num_bones++;
				continue;
			}
			
			bone.id = num_nodes;
			
			bones.insertLast(bone);
			num_bones++;
			
			nodes.insertLast(bone);
			num_nodes++;
			
			if(bones_stack_size < bones_stack_index + bone.num_children * 2)
			{
				bones_stack.resize(bones_stack_size = bones_stack_index + bone.num_children * 2);
			}
			
			for(int i = 0, count = bone.num_drawables; i < count; i++)
			{
				Drawable@ drawable = bone.drawables[i];
				drawable.id = num_nodes;
				
				nodes.insertLast(drawable);
				num_nodes++;
			}
			
			@bones_stack[bones_stack_index++] = null;
			for(int i = bone.num_children - 1; i >= 0; i--)
			{
				Bone@ child = bone.children[i];
				@bones_stack[bones_stack_index++] = child;
			}
		}
	}
	
	string save()
	{
		ByteArray data('');
		StringTable string_table;
		
		data.writeEncodedInt(num_nodes);
		for(int i = 0; i < num_nodes; i++)
		{
			nodes[i].save(data, string_table);
		}
		
		data.writeEncodedInt(num_bones);
		for(int i = 0; i < num_bones; i++)
		{
			Bone@ bone = bones[i];
			data.writeEncodedInt(bone is null ? 0 : bone.id + 1);
		}
		
		array<string>@ anim_keys = animations.getKeys();
		uint anim_count = anim_keys.length();
		for(uint i = 0; i < anim_count; i++)
		{
			Animation@ anim = cast<Animation>(animations[anim_keys[i]]);
			if(@anim != null) anim.save(data, string_table);
		}
		
		data.trimData();
		return string_table.save() + data.data;
	}
	
	void load(string data)
	{
		
	}
	
	void add_animation(Animation@ animation)
	{
		@animations[animation.name] = animation;
	}
	
	void play()
	{
		if(@current_anim != null) is_playing = true;
	}
	
	void pause()
	{
		is_playing = false;
	}
	
	void goto_next_frame()
	{
		if(current_anim is null) return;
		
		target_frame++;
		if(target_frame >= current_anim.length - (current_anim.skip_last_frame and current_anim.loop ? 1 : 0))
		{
			target_frame = current_anim.loop ? 0 : current_anim.length - 1;
		}
	}
	
	void goto_prev_frame()
	{
		if(current_anim is null) return;
		
		target_frame--;
		if(target_frame < 0)
		{
			target_frame = current_anim.loop ? current_anim.length - (current_anim.skip_last_frame ? 2 : 1) : 0;
		}
	}
	
	void set_position(int frame)
	{
		if(current_anim is null or frame == current_frame) return;
		
		if(current_anim.loop)
		{
			frame = frame % (current_anim.length - (current_anim.skip_last_frame ? 1 : 0));
		}
		else
		{
			if(frame < 0) frame = 0;
			else if(frame >= current_anim.length) frame = int(current_anim.length) - 1;
		}
		
		target_frame = frame;
	}
	
	void set_animation(string name)
	{
		if(!animations.exists(name)) name = '_bind';
		
		@current_anim = cast<Animation>(animations[name]);
		
		if(@current_anim != null)
		{
			fps_step = current_anim.fps / 60;
			current_frame = -2;
			current_time = 0;
			target_frame = 0;
		}
	}
	
	void force_update()
	{
		if(current_frame == target_frame or current_anim is null) return;
		
		const bool goto_prev = target_frame == current_frame - 1;
		const bool goto_next = target_frame == current_frame + 1;
		
		array<Track@>@ tracks = current_anim.track_list;
		const int num_tracks = current_anim.num_tracks;
		for(int i = 0; i < num_tracks; i++)
		{
			Track@ track = tracks[i];
			
			const int size = track.num_keyframes;
			if(size == 0) continue;
			const array<int>@ frames = @track.frame_index;
			
			int current = track.current;
			int prev = track.prev;
			int next = track.next;
			
			if(goto_next)
			{
				if(next != -1)
				{
					if(frames[next] == target_frame)
					{
						current = next;
						prev = current > 0 ? current - 1 : -1;
						next = current < size - 1 ? current + 1 : -1;
					}
					else if(current != -1)
					{
						prev = current;
						current = -1;
					}
				}
				else if(current != -1)
				{
					prev = current;
					current = -1;
				}
			}
			else if(goto_prev)
			{
				if(prev != -1)
				{
					if(frames[prev] == target_frame)
					{
						current = prev;
						prev = current > 0 ? current - 1 : -1;
						next = current < size - 1 ? current + 1 : -1;
					}
					else if(current != -1)
					{
						next = current;
						current = -1;
					}
				}
				else if(current != -1)
				{
					next = current;
					current = -1;
				}
			}
			else
			{
				int lo = 0;
				int hi = size - 1;
				int index = -1;
				while(lo < hi)
				{
					const int mid = (lo + hi) / 2;
					if(frames[mid] == target_frame)
					{
						index = mid;
						break;
					}
					else if(frames[mid] > target_frame)
					{
						hi = mid - 1;
					}
					else
					{
						lo = mid + 1;
					}
				}
				
				if(lo > hi)
				{
					const int tmp = lo;
					lo = hi;
					hi = tmp;
				}
				if(frames[lo] == target_frame) index = lo;
				if(index == -1)
				{
					if(frames[lo] > target_frame) lo = -1;
					else if(hi >= size) hi = -1;
					else if(lo == hi) hi++;
				}
				else{
					lo = index > 0 ? index - 1 : -1;
					hi = index < size - 1 ? index + 1 : -1;
				}
				
				current = index;
				prev = lo;
				next = hi >= size ? -1 : hi;
			}
			
			track.current = current;
			track.prev = prev;
			track.next = next;
			
			Node@ node = nodes[track.node_id];
			
			const int index = current != -1 ? current : (prev == -1 and next != -1 ? next : (prev != -1 and next == -1 ? prev : -1));
			if(index != -1)
			{
				node.x = track.x[index];
				node.y = track.y[index];
				node.scale_x = track.scale_x[index];
				node.scale_y = track.scale_y[index];
				node.rotation = track.rotation[index];
			}
			else
			{
				const float t = (target_frame - track.frame_index[prev]) / float(track.frame_index[next] - track.frame_index[prev]);
				node.x = track.x[prev] + (track.x[next] - track.x[prev]) * t;
				node.y = track.y[prev] + (track.y[next] - track.y[prev]) * t;
				node.scale_x = track.scale_x[prev] + (track.scale_x[next] - track.scale_x[prev]) * t;
				node.scale_y = track.scale_y[prev] + (track.scale_y[next] - track.scale_y[prev]) * t;
				node.rotation = track.rotation[prev] + (((track.rotation[next] - track.rotation[prev]) + 180) % 360 - 180) * t;
			}
		}
		
		current_frame = target_frame;
		if(!goto_prev and !goto_next) current_time = target_frame;
	}
	
	void step()
	{
		if(is_playing and @current_anim != null)
		{
			current_time += fps_step * playback_speed;
			target_frame = int(floor(current_time));
			if(target_frame != current_frame)
			{
				if(target_frame >= current_anim.length - (current_anim.skip_last_frame and current_anim.loop ? 1 : 0))
				{
					target_frame = current_anim.loop ? 0 : current_anim.length - 1;
				}
				else if(target_frame < 0)
				{
					target_frame = current_anim.loop ? current_anim.length - (current_anim.skip_last_frame ? 2 : 1) : 0;
				}
			}
		}
		
		if(current_frame != target_frame)
		{
			force_update();
		}
	}
	
	void draw(float sub_frame)
	{
		c.reset();
		c.translate(self.x(), self.y());
		c.rotate(rotation, 0, 0);
		c.scale(scale_x, scale_y);
		
		int layer = c.layer();
		int sub_layer = c.sub_layer();
		
		array<Bone@>@ bones = @this.bones;
		const int num_bones = this.num_bones;
		for(int j = 0; j < num_bones; j++)
		{
			Bone@ bone = bones[j];
			if(bone is null)
			{
				c.pop();
				continue;
			}
			
			c.push();
			c.translate(bone.x, bone.y);
			if(bone.rotation != 0) c.rotate(bone.rotation, 0, 0);
			if(bone.scale_x != 1 or bone.scale_y != 1) c.scale(bone.scale_x, bone.scale_y);
			
			for(int i = 0, count = bone.num_drawables; i < count; i++)
			{
				Drawable@ drawable = bone.drawables[i];
				
				if(layer != drawable.layer) c.layer(layer = drawable.layer);
				if(sub_layer != drawable.sub_layer) c.sub_layer(sub_layer = drawable.sub_layer);
				
				switch(drawable.type)
				{
					case DrawableType::Sprite:
					{
						Sprite@ spr = cast<Sprite>(drawable);
						c.draw_sprite(spr.sprite, spr.sprite_name, spr.frame, spr.palette, spr.x, spr.y, spr.rotation, spr.scale_x, spr.scale_y, 0xFFFFFFFF);
					} break;
					
					case DrawableType::Text:
					{
						Text@ text = cast<Text>(drawable);
						c.draw_text(text.txt, text.x, text.y, text.scale_x, text.scale_y, text.rotation);
					} break;
					
					case DrawableType::Rect:
					{
						Rect@ rect = cast<Rect>(drawable);
						c.draw_rectangle(
							rect.x + rect.x1 * rect.scale_x, rect.y + rect.y1 * rect.scale_y,
							rect.x + rect.x2 * rect.scale_x, rect.y + rect.y2 * rect.scale_y,
							rect.rotation, rect.colour);
					} break;
					
					case DrawableType::Line:
					{
						Line@ line = cast<Line>(drawable);
						c.draw_line(
							line.x + line.x1 * line.scale_x, line.y + line.y1 * line.scale_y,
							line.x + line.x2 * line.scale_x, line.y + line.y2 * line.scale_y,
							line.width, line.colour);
					} break;
				}
			}
			
			c.translate(bone.length, 0);
		}
		
		if(show_bones)
		{
			draw_bones();
		}
	}
	
	protected void draw_bones()
	{
		c.reset();
		c.layer(21);
		c.sub_layer(21);
		c.translate(self.x(), self.y());
		c.rotate(rotation, 0, 0);
		c.scale(scale_x, scale_y);
		
		array<Bone@>@ bones = @this.bones;
		const int num_bones = this.num_bones;
		for(int j = 0; j < num_bones; j++)
		{
			Bone@ bone = bones[j];
			if(bone is null)
			{
				c.pop();
				continue;
			}
			
			c.push();
			c.translate(bone.x, bone.y);
			if(bone.rotation != 0) c.rotate(bone.rotation, 0, 0);
			c.draw_line(0, 0, bone.length * bone.scale_x, 0, 6, 0xFF000000);
			c.draw_line(0, 0, bone.length * bone.scale_x, 0, 3, 0xFFFFFFFF);
			if(bone.scale_x != 1 or bone.scale_y != 1) c.scale(bone.scale_x, bone.scale_y);
			
			c.translate(bone.length, 0);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
	}
}