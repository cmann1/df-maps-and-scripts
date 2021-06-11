const float NaN = float(0x7fc00000);
const float Infinity = float(0x7f800000);

enum Endian
{
	LITTLE_ENDIAN,
	BIG_ENDIAN
}

class ByteArray
{
	
	protected bool big_endian;
	Endian endian{
		get{ return big_endian ? BIG_ENDIAN : LITTLE_ENDIAN; }
		set{ big_endian = value == BIG_ENDIAN; }
	}
	string data;
	uint length;
	uint position = 0;
	
	uint buffer_size = 0;
	uint expand_size = 200;
	
	ByteArray(const string &in data, Endian endian=BIG_ENDIAN)
	{
		this.data = data;
		this.endian = endian;
		buffer_size = length = data.length();
	}
	
	void trimData()
	{
		data = data.substr(0, length);
	}
	
	string base64_encode(){
		uint a = 0;
		
		const array<string> base64Chars = {
			'A','B','C','D','E','F','G','H',
			'I','J','K','L','M','N','O','P',
			'Q','R','S','T','U','V','W','X',
			'Y','Z','a','b','c','d','e','f',
			'g','h','i','j','k','l','m','n',
			'o','p','q','r','s','t','u','v',
			'w','x','y','z','0','1','2','3',
			'4','5','6','7','8','9','+','/'
		};
		
		string base64EncStr = '';
		uint bufferSize;
		string bufferEncStr = '';
		
		while(a < length){
			bufferSize = (length - a) >= 3 ? 3 : (length - a);
			
			bufferEncStr = base64Chars[data[a] >> 2];
			
			if(bufferSize == 1){
				bufferEncStr += base64Chars[((data[a] << 4) & 0x30)];
				bufferEncStr += "=="; 
			}
			else if(bufferSize == 2){
				bufferEncStr += base64Chars[(data[a] << 4) & 0x30 | data[a + 1] >> 4];
				bufferEncStr += base64Chars[(data[a + 1] << 2) & 0x3c];
				bufferEncStr += "=";
			}
			else if(bufferSize == 3){
				bufferEncStr += base64Chars[(data[a] << 4) & 0x30 | data[a + 1] >> 4];
				bufferEncStr += base64Chars[(data[a + 1] << 2) & 0x3c | data[a + 2] >> 6];
				bufferEncStr += base64Chars[data[a + 2] & 0x3F];
			}
			
			a += bufferSize;
			base64EncStr += bufferEncStr;
		}
		
		return base64EncStr;
	}
	
	int readByte()
	{
		if(position >= length)
		{
			puts("Trying to read beyond buffer length");
			return 0;
		}
		
		int value = data[position++];
		
		if((value & 0x80) == 0) return value;
		return -(0xff - value + 1);
	}
	void writeByte(int value)
	{
		if(position + 1 > buffer_size) data.resize(buffer_size += 200);
		
		value = value & 0xFF;
		if(value < 0) value = 0xFF + value + 1;
		
		data[position++] = value;
		if(position > length) length = position;
	}
	uint readUnsignedByte()
	{
		if(position >= length)
		{
			puts("Trying to read beyond buffer length");
			return 0;
		}
		
		return data[position++];
	}
	void writeUnsignedByte(uint value)
	{
		if(position + 1 > buffer_size) data.resize(buffer_size += 200);
		
		data[position++] = value;
		if(position > length) length = position;
	}
	
	int readShort()
	{
		if(position + 2 > length)
		{
			puts('Trying to read beyond buffer length');
			return 0;
		}
		
		int value = 0;
		for(int i = 0; i < 2; i++)
		{
			value = value << 8 | data[big_endian ? position + i : position + 1 - i];
		}
		position += 2;
		
		if(value & 0x8000 == 0) return value;
		return -(0xffff - value + 1);
	}
	void writeShort(int value)
	{
		if(position + 2 > buffer_size) data.resize(buffer_size += 200);
		
		if(value < 0) value = 0xFFFF + value + 1;
		
		for(int i = 0; i < 2; i++) 
		{
			data[position + i] = (value >> ((big_endian ? 1 - i : i) * 8)) & 0xFF;
		}
		position += 2;
		if(position > length) length = position;
	}
	uint readUnsignedShort()
	{
		if(position + 2 > length)
		{
			puts('Trying to read beyond buffer length');
			return 0;
		}
		
		uint value = 0;
		for(int i = 0; i < 2; i++)
		{
			value = value << 8 | data[big_endian ? position + i : position + 1 - i];
		}
		position += 2;
		
		return value;
	}
	void writeUnsignedShort(uint value)
	{
		if(position + 2 > buffer_size) data.resize(buffer_size += 200);
		
		for(int i = 0; i < 2; i++) 
		{
			data[position + i] = (value >> ((big_endian ? 1 - i : i) * 8)) & 0xFF;
		}
		position += 2;
		if(position > length) length = position;
	}
	
	
	int readInt()
	{
		if(position + 4 > length)
		{
			puts('Trying to read beyond buffer length');
			return 0;
		}
		
		uint value = 0;
		for(int i = 0; i < 4; i++)
		{
			value = value << 8 | data[big_endian ? position + i : position + 3 - i];
		}
		position += 4;
		
		if(value & 0x80000000 == 0) return value;
		return -(0xFFFFFFFF - value + 1);
	}
	void writeInt(int value)
	{
		if(position + 4 > buffer_size) data.resize(buffer_size += 200);
		
		uint v = value < 0 ? 0xFFFFFFFF + uint(value) + 1 : uint(value);
		
		if(position + 4 > buffer_size) data.resize(buffer_size += 200);
		
		for(int i = 0; i < 4; i++) 
		{
			data[position + i] = (v >> ((big_endian ? 3 - i : i) * 8)) & 0xFF;
		}
		position += 4;
		if(position > length) length = position;
	}
	uint readUnsignedInt()
	{
		if(position + 4 > length)
		{
			puts('Trying to read beyond buffer length');
			return 0;
		}
		
		uint value = 0;
		for(int i = 0; i < 4; i++)
		{
			value = value << 8 | data[big_endian ? position + i : position + 3 - i];
		}
		position += 4;
		
		return value;
	}
	void writeUnsignedInt(uint value)
	{
		if(position + 4 > buffer_size) data.resize(buffer_size += 200);
		
		for(int i = 0; i < 4; i++) 
		{
			data[position + i] = (value >> ((big_endian ? 3 - i : i) * 8)) & 0xFF;
		}
		position += 4;
		if(position > length) length = position;
	}
	
	int64 readLong()
	{
		if(position + 8 > length)
		{
			puts('Trying to read beyond buffer length');
			return 0;
		}
		
		uint64 value = 0;
		for(int i = 0; i < 8; i++)
		{
			value = value << 8 | data[big_endian ? position + i : position + 7 - i];
		}
		position += 8;
		
		if(value & 0x8000000000000000 == 0) return value;
		return -(0xFFFFFFFFFFFFFFFF - value + 1);
	}
	void writeLong(int64 value)
	{
		uint64 v = value < 0 ? 0xFFFFFFFFFFFFFFFF + uint64(value) + 1 : uint64(value);
		
		if(position + 8 > buffer_size) data.resize(buffer_size += 200);
		
		for(int i = 0; i < 8; i++) 
		{
			puts(' w ' + ((v >> ((big_endian ? 7 - i : i) * 8)) & 0xFF));
			data[position + i] = (v >> ((big_endian ? 7 - i : i) * 8)) & 0xFF;
		}
		position += 8;
		if(position > length) length = position;
	}
	uint64 readUnsignedLong()
	{
		if(position + 8 > length)
		{
			puts('Trying to read beyond buffer length');
			return 0;
		}
		
		uint64 value = 0;
		for(int i = 0; i < 8; i++)
		{
			value = value << 8 | data[big_endian ? position + i : position + 7 - i];
		}
		position += 8;
		
		return value;
	}
	void writeUnsignedLong(uint64 value)
	{
		if(position + 8 > buffer_size) data.resize(buffer_size += 200);
		
		for(int i = 0; i < 8; i++) 
		{
			data[position + i] = (value >> ((big_endian ? 7 - i : i) * 8)) & 0xFF;
		}
		position += 8;
		if(position > length) length = position;
	}
	
	uint readEncodedShort()
	{
		if(position >= length)
		{
			puts('Trying to read beyond buffer length');
			return 0;
		}
		
		uint byte = data[position++];
		uint value = byte >> 1;
		
		if((byte & 0x1) == 1)
		{
			if(position >= length)
			{
				puts('Trying to read beyond buffer length');
				return 0;
			}
			
			value |= data[position++] << 7;
		}
		
		return value;
	}
	void writeEncodedShort(uint value)
	{
		if(position + 2 > buffer_size) data.resize(buffer_size += 200);
		
		const int numExtraBytes = value > 0x7F ? 1 : 0;
		data[position++] = ((value << 1) & 0xFF) | numExtraBytes;
		
		if(numExtraBytes == 1)
		{
			data[position++] = (value >> 7) & 0xFF;
		}
		
		if(position > length) length = position;
	}
	
	uint readVariableLengthInteger()
	{
		if(position >= length)
		{
			puts('Trying to read beyond buffer length');
			return 0;
		}
		
		uint byte = data[position++];
		uint value = byte >> 2;
		uint numBytes = byte & 0x3; // 00000011;
		uint shift = 6;
		
		while(numBytes > 0)
		{
			if(position >= length)
			{
				puts('Trying to read beyond buffer length');
				return 0;
			}
			
			byte = data[position++];
			value |= byte << shift;
			
			shift += 8;
			numBytes--;
		}
		
		return value;
	}
	void writeVariableLengthInteger(uint value)
	{
		if(position + 4 > buffer_size) data.resize(buffer_size += 200);
		
		int bytesWritten = 0;
		
		const uint firstPosition = position;
		uint nextByte = (value & 0x3F) << 2; // 0x3F = 00111111
		value >>= 6;
		
		do
		{
			data[position++] = nextByte;
			nextByte = value & 0xFF;
			value >>= 8;
			bytesWritten++;
		}
		while(value > 0 or nextByte > 0);
		
		data[firstPosition] |= (bytesWritten - 1);
		
		if(position > length) length = position;
	}
	
	uint readEncodedInt()
	{
		uint mask = 0;
		uint check_mask = 0x00000080;
		uint shift = 0;
		uint result = 0;
		
		for(int i = 0; i < 5; i++)
		{
			if(position >= length)
			{
				puts('Trying to read beyond buffer length');
				return 0;
			}
			
			result = (result & mask) | data[position++] << shift;
			
			if((result & check_mask) == 0)
				return result;
			
			mask = 0x0000007f | (mask << 7);
			check_mask <<= 7;
			shift += 7;
		}
		
		return result;
	}
	void writeEncodedInt(uint value)
	{
		if(position + 4 > buffer_size) data.resize(buffer_size += 200);
		
		uint byte;						// The value of the current 7 extracted bits
		uint nextByte = value & 0x7f;	// The value of the next 7 extracted bits
		
		do{
			byte = nextByte;
			value >>= 7;
			nextByte = value & 0x7f; // 0x7f = 01111111
			
			// Flag this byte to indicate that the next byte also contains data
			if(value != 0) byte |= 0x80; // 0x80 = 10000000
			
			data[position++] = byte;
		}
		while(value != 0);
		
		if(position > length) length = position;
	}
	
	float readFloat()
	{
		if(position + 4 > length)
		{
			puts('Trying to read beyond buffer length');
			return 0;
		}
		
		uint raw = 0;
		for(int i = 0; i < 4; i++)
		{
			raw = raw << 8 | data[big_endian ? position + i : position + 3 - i];
		}
		position += 4;
		return fpFromIEEE(raw);
	}
	void writeFloat(float value)
	{
		if(position + 4 > buffer_size) data.resize(buffer_size += 200);
		
		uint raw = fpToIEEE(value);
		for(int i = 0; i < 4; i++) 
		{
			data[position + i] = (raw >> ((big_endian ? 3 - i : i) * 8)) & 0xFF;
		}
		position += 4;
		if(position > length) length = position;
	}
	
	
	double readDouble()
	{
		if(position + 8 > length)
		{
			puts('Trying to read beyond buffer length');
			return 0;
		}
		
		uint64 raw = 0;
		for(int i = 0; i < 8; i++)
		{
			raw = raw << 8 | data[big_endian ? position + i : position + 7 - i];
		}
		position += 8;
		return fpFromIEEE(raw);
	}
	void writeDouble(double value)
	{
		if(position + 8 > buffer_size) data.resize(buffer_size += 200);
		
		const uint64 raw = fpToIEEE(value);
		for(int i = 0; i < 8; i++) 
		{
			data[position + i] = (raw >> ((big_endian ? 7 - i : i) * 8)) & 0xFF;
		}
		position += 8;
		if(position > length) length = position;
	}
	
	
	string readUTF(){
		uint string_length = readUnsignedInt();
		int code, code2, code3;
		uint i = position;
		string result = '';
		
		if(position + string_length > length)
		{
			puts('Trying to read beyond buffer length');
			return '';
		}
		
		result.resize(string_length);
		uint result_index = 0;
		
		string_length += position;

		while(i < string_length){
			code = data[i++];
			if(code < 128){
				result[result_index++] = code;
			}
			else if( (code > 191) && (code < 224) ){
				code2 = data[i++];
				result[result_index++] = ((code & 31) << 6) | (code2 & 63);
			}
			else{
				code2 = data[i++];
				code3 = data[i++];
				result[result_index++] = ((code & 15) << 12) | ((code2 & 63) << 6) | (code3 & 63);
			}
		}
		
		position = string_length;
		
		return result;
	}
	void writeUTF(const string &in value)
	{
		const uint string_length = value.length();
		
		writeUnsignedInt(string_length);
		
		if(position + string_length > buffer_size) data.resize(buffer_size += string_length + 200);
		
		for(uint a = 0; a < string_length; a++){
			const int code = value[a];

			if(code < 128){
				data[position++] = code;
			}
			else if( (code > 127) && (code < 2048) ){
				data[position++] = (code >> 6) | 192;
				data[position++] = (code & 63) | 128;
			}
			else{
				data[position++] = (code >> 12) | 224;
				data[position++] = ((code >> 6) & 63) | 128;
				data[position++] = (code & 63) | 128;
			}
		}
		
		if(position > length) length = position;
	}
	
}