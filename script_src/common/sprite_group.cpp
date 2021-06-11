class simple_transform {
  float x;
  float y;
  float rot;
  float scale_x;
  float scale_y;
  uint colour;
  uint frame;
  uint palette;
  int layer;
  int sub_layer;

  simple_transform() {
    x = 0;
    y = 0;
    rot = 0;
    scale_x = 1;
    scale_y = 1;
	colour = 0xFFFFFFFF;
	frame = 0;
	palette = 0;
	layer = -1;
	sub_layer = -1;
  }

  simple_transform(float _x, float _y, float _rot, float _scale_x, float _scale_y, uint _colour, uint _frame=0, uint _palette=0, int _layer=-1, int _sub_layer=-1) {
    x = _x;
    y = _y;
    rot = _rot;
    scale_x = _scale_x;
    scale_y = _scale_y;
	colour = _colour;
	frame = _frame;
	palette = _palette;
	layer = _layer;
	sub_layer = _sub_layer;
  }
}

class sprite_rectangle {
  float top;
  float bottom;
  float left;
  float right;

  sprite_rectangle() {
    top = 0;
    bottom = 0;
    left = 0;
    right = 0;
  }
}

class sprite_group {
  dictionary sprite_map;
  
  [hidden] array<string> sprite_sets;
  [hidden] array<string> sprite_names;
  [hidden] array<sprites@> sprite_sprites;
  [hidden] array<simple_transform> sprite_transforms;

  sprite_group() {
  }

  sprite_group(const array<string> &in sprite_set_name,
	const array<int> &in layer_sub_layer,
	const array<float> &in align_x_y,
	const array<float> &in off_x_y,
	const array<float> &in rotation,
	const array<float> &in scale_x_y,
	const array<uint> &in colour,
	const array<uint> &in frame,
	const array<uint> &in palette
	) {
		populate(
		sprite_set_name, layer_sub_layer,
		align_x_y, off_x_y, rotation, scale_x_y, colour,
		frame, palette);
  }
  
  void populate(
	const array<string> &in sprite_set_name,
	const array<int> &in layer_sub_layer,
	const array<float> &in align_x_y,
	const array<float> &in off_x_y,
	const array<float> &in rotation,
	const array<float> &in scale_x_y,
	const array<uint> &in colour,
	const array<uint> &in frame,
	const array<uint> &in palette
	) {
	  const int num_sprites = int(palette.length());
	  sprite_sets.resize(num_sprites);
	  sprite_names.resize(num_sprites);
	  sprite_sprites.resize(num_sprites);
	  sprite_transforms.resize(0);
	  int ix = 0, iy = 1;
	  
	  for(int i = 0; i < num_sprites; i++, ix += 2, iy = ix + 1) {
        const string sprite_set = sprite_set_name[ix];
        const string sprite_name = sprite_set_name[iy];
		
		sprites@ spr = cast<sprites>(sprite_map[sprite_set]);
		if(spr is null)
		{
			@sprite_map[sprite_set] = @spr = create_sprites();
			spr.add_sprite_set(sprite_set);
		}
		
		sprite_sets[i] = sprite_set;
		sprite_names[i] = sprite_name;
		@sprite_sprites[i] = @spr;

		rectangle@ rc = spr.get_sprite_rect(sprite_name, 0);
        float width = rc.get_width();
        float height = rc.get_height();
	    
		float off_x = off_x_y[ix];
		float off_y = off_x_y[iy];
		const float rot = rotation[i];
		const float scale_x = scale_x_y[ix];
		const float scale_y = scale_x_y[iy];
		const float align_x = align_x_y[ix];
		const float align_y = align_x_y[iy];
		
        float rx = (rc.left() + width * align_x) * scale_x;
        float ry = (rc.top() + height * align_y) * scale_y;
        float cs = cos(rot * DEG2RAD);
        float sn = sin(rot * DEG2RAD);
        off_x -= rx * cs - ry * sn;
        off_y -= ry * cs + rx * sn;

        sprite_transforms.insertLast(
		  simple_transform(off_x, off_y, rot, scale_x, scale_y, colour[i], frame[i], palette[i], layer_sub_layer[ix], layer_sub_layer[iy]));
	  }
  }
  
	sprites@ get_sprites(string sprite_set)
	{
		sprites@ spr = cast<sprites>(sprite_map[sprite_set]);
		if(spr is null)
		{
			@sprite_map[sprite_set] = @spr = create_sprites();
			spr.add_sprite_set(sprite_set);
		}
		
		return spr;
	}
  
  void clear() {
	  sprite_names.resize(0);
	  sprite_transforms.resize(0);
  }
  
  void add_sprite(string sprite_set) {
//	   spr.add_sprite_set(sprite_set);
  }

  void add_sprite(string sprite_set, string sprite_name,
                  float align_x = 0.5, float align_y = 0.5,
                  float off_x = 0, float off_y = 0, float rot = 0,
                  float scale_x = 1, float scale_y = 1, uint colour=0xFFFFFFFF, uint frame=0, uint palette=0,
				  int layer=-1, int sub_layer=-1) { 
	sprites@ spr = get_sprites(sprite_set);
    sprite_sets.insertLast(sprite_set);
    sprite_names.insertLast(sprite_name);
    sprite_sprites.insertLast(@spr);
	
    rectangle@ rc = spr.get_sprite_rect(sprite_name, 0);

    float width = rc.get_width();
    float height = rc.get_height();

    float rx = (rc.left() + width * align_x) * scale_x;
    float ry = (rc.top() + height * align_y) * scale_y;
    float cs = cos(rot * DEG2RAD);
    float sn = sin(rot * DEG2RAD);
    off_x -= rx * cs - ry * sn;
    off_y -= ry * cs + rx * sn;

    sprite_transforms.insertLast(
          simple_transform(off_x, off_y, rot, scale_x, scale_y, colour, frame, palette, layer, sub_layer));
  }
  
  void set_colour(uint colour)
  {
	for (uint i = 0; i < sprite_names.size(); i++) {
		sprite_transforms[i].colour = colour;
	}
  }

  void draw(int layer, int sub_layer, float x, float y, float rot, float scale) {
    float cs = cos(rot * DEG2RAD);
    float sn = sin(rot * DEG2RAD);
    for (uint i = 0; i < sprite_names.size(); i++) {
      simple_transform@ tx = @sprite_transforms[i];
      sprites@ spr = @sprite_sprites[i];

      float px = tx.x * cs - tx.y * sn;
      float py = tx.y * cs + tx.x * sn;
      spr.draw_world(tx.layer == -1 ? layer : tx.layer, tx.sub_layer == -1 ? sub_layer : tx.sub_layer, sprite_names[i], tx.frame, tx.palette,
                     x + px * scale, y + py * scale,
                     rot + tx.rot, tx.scale_x * scale, tx.scale_y * scale, tx.colour);
    }
  }

  void draw_hud(int layer, int sub_layer, float x, float y, float rot, float scale) {
    float cs = cos(rot * DEG2RAD);
    float sn = sin(rot * DEG2RAD);
    for (uint i = 0; i < sprite_names.size(); i++) {
      simple_transform@ tx = @sprite_transforms[i];
	  sprites@ spr = @sprite_sprites[i];

      float px = tx.x * cs - tx.y * sn;
      float py = tx.y * cs + tx.x * sn;
      spr.draw_hud(tx.layer == -1 ? layer : tx.layer, tx.sub_layer == -1 ? sub_layer : tx.sub_layer, sprite_names[i], tx.frame, tx.palette,
                     x + px * scale, y + py * scale,
                     rot + tx.rot, tx.scale_x * scale, tx.scale_y * scale, tx.colour);
    }
  }

  sprite_rectangle get_rectangle(float rot, float scale) {
    float cs = cos(rot * DEG2RAD);
    float sn = sin(rot * DEG2RAD);

    sprite_rectangle rc;
    for (uint i = 0; i < sprite_names.size(); i++) {
      simple_transform@ tx = @sprite_transforms[i];
      sprites@ spr = @sprite_sprites[i];

      float px = tx.x * cs - tx.y * sn;
      float py = tx.y * cs + tx.x * sn;
      rectangle@ src = spr.get_sprite_rect(sprite_names[i], 0);

      float scs = cos((rot + tx.rot) * DEG2RAD);
      float ssn = sin((rot + tx.rot) * DEG2RAD);
      array<float> xs = {src.left(), src.right()};
      array<float> ys = {src.top(), src.bottom()};
      for (int j = 0; j < 2; j++) {
        for (int k = 0; k < 2; k++) {
          float cx = (px + (xs[j] * scs - ys[k] * ssn) * tx.scale_x) * scale;
          float cy = (py + (ys[k] * scs + xs[j] * ssn) * tx.scale_y) * scale;

          rc.left = min(rc.left, cx);
          rc.right = max(rc.right, cx);
          rc.top = min(rc.top, cy);
          rc.bottom = max(rc.bottom, cy);
        }
      }
    }
    return rc;
  }
}

