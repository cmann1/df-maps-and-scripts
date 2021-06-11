#include "math.cpp"

const float DT = 1.0 / 60;

const float SCREEN_LEFT = -800;
const float SCREEN_TOP = -450;
const float SCREEN_RIGHT = 800;
const float SCREEN_BOTTOM = 450;

const int VAR_TYPE_NONE = 0;
const int VAR_TYPE_BOOL = 1;
const int VAR_TYPE_INT8 = 2;
const int VAR_TYPE_INT16 = 3;
const int VAR_TYPE_INT32 = 4;
const int VAR_TYPE_INT64 = 5;
const int VAR_TYPE_FLOAT = 6;
const int VAR_TYPE_STRING = 7;
const int VAR_TYPE_ARRAY = 8;
const int VAR_TYPE_STRUCT = 9;
const int VAR_TYPE_VEC2 = 10;

const int LT_NORMAL = 0;
const int LT_NEXUS = 1;
const int LT_NEXUS_MP = 2;
const int LT_TUGOFWAR = 3;
const int LT_SURVIVAL = 4;
const int LT_RUSH = 5;
const int LT_DUSTMOD = 6;

string bin(uint64 x, uint max_bits=32)
{
	string result = "";
	
	while(max_bits-- > 0)
	{
		result = ((x & 1 == 1) ? "1" : "0") + result;
		x >>= 1;
	}
	
	return result;
}

prop@ create_prop(uint set, uint group, uint index, int layer=19, int sub_layer=19, float x=0, float y=0, float rotation=0)
{
	prop@ p = create_prop();
	p.prop_set(set);
	p.prop_group(group);
	p.prop_index(index);
	p.layer(layer);
	p.sub_layer(sub_layer);
	p.x(x);
	p.y(y);
	p.rotation(rotation);
	
	return p;
}

void print_vars(entity@ e)
{
	varstruct@ vars = e.vars();
	puts(e.type_name() + "[" + vars.num_vars() + "]");
	
	for(uint i = 0; i < vars.num_vars(); i++)
	{
//		string name = vars.var_name(i);
//		varvalue@ value = vars.get_var(i);
		puts( print_var_value(vars.var_name(i), vars.get_var(i), "   ") );
	}
}

string print_var_value(string name, varvalue@ value, string indent="", int array_max=25, bool print_type=true)
{
	const int id = value.type_id();
	
	string value_str = "";
	
	if(id == VAR_TYPE_BOOL)
	{
		value_str = "" + value.get_bool();
	}
	else if(id == VAR_TYPE_INT8)
	{
		value_str = "" + value.get_int8();
	}
	else if(id == VAR_TYPE_INT16)
	{
		value_str = "" + value.get_int16();
	}
	else if(id == VAR_TYPE_INT32)
	{
		value_str = "" + value.get_int32();
	}
	else if(id == VAR_TYPE_INT64)
	{
		value_str = "" + value.get_int64();
	}
	else if(id == VAR_TYPE_FLOAT)
	{
		value_str = "" + value.get_float();
	}
	else if(id == VAR_TYPE_STRING)
	{
		value_str = "\"" + value.get_string() +"\"";
	}
	else if(id == VAR_TYPE_VEC2)
	{
		value_str = "<" + value.get_vec2_x() + ", " + value.get_vec2_y() + ">";
	}
	else if(id == VAR_TYPE_ARRAY)
	{
		vararray@ arr_value = value.get_array();
		int size = arr_value.size();
		value_str = var_type_string(arr_value.element_type_id()) + "[" + size + "]";
		
		for(int i = 0; i < size; i++)
		{
			varvalue@ item = arr_value.at(i);
			value_str += "\n" + print_var_value(i + "", item, indent + "   ", array_max, false);
			
			if(array_max > 0 and i > array_max and  i + 1 < size)
			{
				value_str += "\n   " + indent + "... " + (size - array_max) + " more";
				break;
			}
		}
	}
	else if(id == VAR_TYPE_STRUCT)
	{
//		value_str = "<" + value.get_vec2_x() + ", " + value.get_vec2_y() + ">";
	}
	
	return indent + name + (print_type ? "[" + var_type_string(id) + "]" : "") + " = " + value_str;
}

string var_type_string(const int id)
{
	if(id == VAR_TYPE_NONE)
		return "None";
	if(id == VAR_TYPE_BOOL)
		return "Bool";
	if(id == VAR_TYPE_INT8)
		return "Int8";
	if(id == VAR_TYPE_INT16)
		return "Int16";
	if(id == VAR_TYPE_INT32)
		return "Int32";
	if(id == VAR_TYPE_INT64)
		return "Int64";
	if(id == VAR_TYPE_FLOAT)
		return "Float";
	if(id == VAR_TYPE_STRING)
		return "String";
	if(id == VAR_TYPE_ARRAY)
		return "Array";
	if(id == VAR_TYPE_STRUCT)
		return "Struct";
	if(id == VAR_TYPE_VEC2)
		return "Vec2";
	
	return "Uknown";
}

string str(float x)
{
	return formatFloat(x, "", 0, 3);
}
string vstr(float x, float y)
{
	return "<" + str(x) + ", " + str(y) + "> ";
}

entity@ update_text_trigger(scene@ g, entity@ original_trigger, string new_text)
{
	entity@ text_trigger = create_entity("text_trigger");
	varstruct@ o_vars = original_trigger.vars();
	varstruct@ n_vars = text_trigger.vars();
	n_vars.get_var("text_string").set_string(new_text);
	n_vars.get_var("width").set_int32(o_vars.get_var("width").get_int32() );
	text_trigger.x(original_trigger.x());
	text_trigger.y(original_trigger.y());
	g.add_entity(text_trigger);
	g.remove_entity(original_trigger);
	
	return text_trigger;
}

void play_script_stream(scene@ g, string name, uint soundGroup, float x, float y, bool loop, float volume, bool positional)
{
	audio@ a;
	
	while(volume > 0)
	{
		@a = g.play_script_stream(name, soundGroup, x, y, loop, min(1, volume));
		if(positional) a.positional(true);
		
		volume--;
	}
}

entity@ create_emitter(int id, float x, float y, int width, int height, int layer, int sub_layer)
{
	entity@ emitter = create_entity("entity_emitter");
	varstruct@ vars = emitter.vars();
	emitter.layer(layer);
	vars.get_var("emitter_id").set_int32(id);
	vars.get_var("width").set_int32(width);
	vars.get_var("height").set_int32(height);
	vars.get_var("draw_depth_sub").set_int32(sub_layer);
	vars.get_var("r_area").set_bool(true);
	emitter.set_xy(x, y);
	
	return emitter;
}

void world_to_screen(camera@ cam, const float x, const float y, float &out out_x, float &out out_y)
{
	const float zoom = cam.screen_height() / 1080;
	out_x = (x - cam.x()) / (960 * zoom) * 800;
	out_y = (y - cam.y()) / (540 * zoom) * 450;
}

entity@ replace_text_trigger(scene@ g, entity@ original)
{
	entity@ text_trigger = create_entity("text_trigger");
	text_trigger.vars().get_var("text_string").set_string( original.vars().get_var("text_string").get_string() );
	text_trigger.vars().get_var("width").set_int32( original.vars().get_var("width").get_int32() );
	text_trigger.x(original.x());
	text_trigger.y(original.y());
	g.add_entity(text_trigger);
	g.remove_entity(original);
	
	return text_trigger;
}

void puts(bool x) { puts(x + ""); }
void puts(int x) { puts(x + ""); }
void puts(int64 x) { puts(x + ""); }
void puts(uint x) { puts(x + ""); }
void puts(uint64 x) { puts(x + ""); }
void puts(float x) { puts(x + ""); }
void puts(double x) { puts(x + ""); }
void puts(int x, int y) { puts(x + ", " + y); }
void puts(float x, float y) { puts(str(x) + ", " + str(y)); }
void puts(rectangle@ r)
{
	if(@r != null)
		puts(vstr(r.left(), r.top()) + "" + vstr(r.right(), r.bottom()));
	else
		puts('null');
}
void puts(entity@ e, rectangle@ r)
{
	if(@r != null)
	{
		const float x = e is null ? 0 : e.x();
		const float y = e is null ? 0 : e.x();
		puts(vstr(x + r.left(), y + r.top()) + "" + vstr(x + r.right(), y + r.bottom()));
	}
	else
	{
		puts('null');
	}
}
void puts(controllable@ e, rectangle@ r)
{
	puts(e.as_entity(), r);
}
void puts(collision@ c) { if(@c != null) puts(c.rectangle()); else puts('null'); }


