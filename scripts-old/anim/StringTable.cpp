class StringTable
{
	
	dictionary string_table_map;
	array<string> string_table;
	
	StringTable()
	{
		
	}
	
	uint get_string_id(const string &in str)
	{
		if(!string_table_map.exists(str))
		{
			string_table.insertLast(str);
			uint id = string_table.length() - 1;
			string_table_map[str] = id;
			return id;
		}
		
		return uint(string_table_map[str]);
	}
	
	string save()
	{
		ByteArray data('');
		const uint size = string_table.length();
		data.writeUnsignedInt(size);
		
		for(uint i = 0; i < size; i++)
		{
			data.writeUTF(string_table[i]);
		}
		
		data.trimData();
		return data.data;
	}
	
}