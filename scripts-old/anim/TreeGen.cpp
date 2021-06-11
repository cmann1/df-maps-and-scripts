class TreeGen : Model
{
	
	[text] uint seed = 0;
	[text] int layer = 16;
	[text] int sub_layer = 19;
	[text] int leaf_sub_layer_min = 20;
	[text] int leaf_sub_layer_max = 20;
	[text] float start_width = 36;
	[text] float start_height = 300;
	[text] float width_factor = 0.75;
	[text] float height_factor = 0.75;
	[text] int branch_min = 2;
	[text] int branch_max = 4;
	[text] float branch_angle = 50;
	[text] int max_depth = 4;
	[text] float swing_min = 1;
	[text] float swing_max = 3;
	[text] float swing_factor = 1.2;
	[text] int swing_length = 240;
	[colour,alpha] uint colour = 0xFFFFFFFF;
	[text] int leaf_min = 2;
	[text] int leaf_max = 8;
	[text] float leaf_scale_min = 0.6;
	[text] float leaf_scale_max = 0.8;
	[text] float leaf_x_min = 0.7;
	[text] float leaf_x_max = 1;
	[text] float leaf_angle = 20;
	[text] float leaf_swing_angle = 120;
	[text] string leaf_set = 'slimeboss';
	[text] string leaf_sprite = 'sbcleanse';
	[text] int leaf_frame = 0;
	
	uint prev_seed = 0;
	
	void init(script@ script, scripttrigger@ self)
	{
		Model::init(script, self);
		
		puts("START");
		generate();
		puts(num_nodes);
		puts('>>>>> ' + save().length());
	}
	
	protected void generate()
	{
		array<Sprite@> leaves;
		
		root.clear();
		root.rotation = -90;
		srand(seed);
		generate_branch(root, null, leaves, start_width, start_height, 0, 0);
		build();
		
		Animation@ anim = Animation('Default', this);
		anim.fps = 30;
		anim.length = swing_length;
		add_animation(anim);
		anim.loop = true;
		anim.skip_last_frame = true;
		
		Track@ track;
		
		for(int i = 0; i < num_bones; i++)
		{
			Bone@ bone = bones[i];
			if(bone is null) continue;
			@track = anim.addTrack(bone);
			const int steps = 30;
			const float angle = (swing_min + frand() * (swing_max - swing_min)) * pow(swing_factor, bone.depth);
			for(int j = 0; j < steps; j++)
			{
				const float t = j / (steps - 1.0);
				track.addKeyFrame(int(t * (swing_length - 1)), 0, 0, 1, 1, bone.rotation + angle * sin(t * PI2));
			}
		}
		
		for(int i = int(leaves.length()) - 1; i >= 0; i--)
		{
			Sprite@ leaf = leaves[i];
			@track = anim.addTrack(leaf);
			track.addKeyFrame(0, leaf.x, leaf.y, leaf.scale_x, leaf.scale_y, leaf.rotation);
			track.addKeyFrame(int(swing_length * rand_range(0.35, 0.65)), leaf.x, leaf.y, leaf.scale_x, leaf.scale_y, leaf.rotation + rand_range(-leaf_swing_angle, leaf_swing_angle));
			track.addKeyFrame(swing_length, leaf.x, leaf.y, leaf.scale_x, leaf.scale_y, leaf.rotation);
		}
		
		set_animation('Default');
	}
	
	protected Bone@ generate_branch(Bone@ branch, Bone@ parent, array<Sprite@>@ leaves, float width, float height, int branch_index, int current_depth)
	{
		if(branch is null) @branch = Bone('bone.' + current_depth + '.' + branch_index);
		if(parent !is null) parent.addChild(branch);
		
		branch.length = height;
		
		const float scale_x = height / 200;
		const float scale_y = width / 25;
		Sprite@ spr = Sprite('props1', 'backdrops_5', 120 * scale_x, 82 * scale_y, scale_x, scale_y);
		// branch.addDrawable(Rect(0, -width*0.5, height, width*0.5, 0, colour));
		branch.addDrawable(spr);
		spr.layer = layer;
		spr.sub_layer = sub_layer;
		
		if(current_depth > 0 and leaf_set != '' and leaf_sprite != '')
		{
			int leaf_count = rand_range(leaf_min, leaf_max);
			
			for(int i = 0; i < leaf_count; i++)
			{
				const float flip = frand() > 0.5 ? 1 : -1;
				const float leaf_scale = rand_range(leaf_scale_min, leaf_scale_max);
				@spr = Sprite(leaf_set, leaf_sprite,
					branch.length * rand_range(leaf_x_min, leaf_x_max), -18 * scale_y * flip,
					leaf_scale, leaf_scale * flip);
				branch.addDrawable(spr);
				spr.layer = layer;
				spr.sub_layer = rand_range(leaf_sub_layer_min, leaf_sub_layer_max);
				spr.rotation = rand_range(-leaf_angle, leaf_angle);
				spr.frame = leaf_frame;
				
				leaves.insertLast(spr);
			}
		}
		
		if(current_depth >= max_depth) return branch;
		
		width *= rand_factor(width_factor, 0.35);
		height *= rand_factor(height_factor, 0.5);
		current_depth++;
		
		int child_count = rand_range(branch_min, branch_max);
		for(int i = 0; i < child_count; i++)
		{
			Bone@ child = generate_branch(null, branch, leaves, width, height, i, current_depth);
			child.rotation = (i / max(1, child_count - 1.0) - 0.5) * rand_factor(branch_angle, 0.5);
		}
		
		return branch;
	}
	
	protected float rand_factor(float value, float amount)
	{
		return value * (1 + (frand() - 0.5) * amount);
	}
	
	void editor_step()
	{
		if(prev_seed != seed)
		{
			generate();
			prev_seed = seed;
		}
	}
	
}