#include "JSON_Declarations.hpp"
#include "../JaguarEngine.hpp"

namespace JSON
{

	namespace IO
	{
		size_t Skip_Whitespace(const char* String)
		{
			size_t Index = 0;

			while (
				String[Index] == ' ' ||
				String[Index] == '\t' ||
				String[Index] == '\n' ||
				String[Index] == ':'
				)
				Index++;

			return Index;
		}

		size_t Scan_JSON_Objects(const char* String, Value* Object, bool Is_Field = false)
		{
			size_t Index = 0;

			size_t Delta;

			//Value Buffer_Value;

			std::string Buffer;

			while (String[Index])
			{

				switch (String[Index])
				{
				case 't':	// a boolean value
				case 'f':
					if (strncmp(String + Index, "true", 4))
					{
						Object->Boolean = true;
						Object->Flag = Value::Type::T_Boolean;
						Index += 4;
						return Index;
					}
					else if (strncmp(String + Index, "false", 5))
					{
						Object->Boolean = false;
						Object->Flag = Value::Type::T_Boolean;
						Index += 5;
						return Index;
					}

					printf(" >> Syntax error in .json file!\n");

					return Index + 1;

				case '.':	// a value
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					Object->Float = std::stod(String + Index, &Delta);
					Object->Flag = Value::Type::T_Float;
					Index += Delta;

					return Index;


				case'\"': // either a string value or a new field
					Index++;
					Buffer = "";
					while(String[Index] != '\"')
						Buffer += String[Index++];
					Index++;

					if (Is_Field)
					{
						// assign the new value!
						Object->String = Buffer;
						Object->Flag = Value::Type::T_String;
					}
					else
					{
						Index += Scan_JSON_Objects(String + Index, &Object->Object[Buffer], true);
					}

					return Index;

				case '{':	// a new object!
					Index++;
					while (String[Index] != '}')
						Index += Scan_JSON_Objects(String + Index, Object, false);

					Object->Flag = Value::Type::T_Object;
					Index++;
					return Index;

				case '[':	// a new array!
					Index++;
					while (String[Index] != ']')
					{
						Value Buffer_Value;

						Buffer_Value.Flag = Value::Type::T_Unassigned;
						Index += Scan_JSON_Objects(String + Index, &Buffer_Value, true);
						if (Buffer_Value.Flag)
							Object->Array.push_back(Buffer_Value);
					}
					Object->Flag = Value::Type::T_Array;

					return Index + 1;

				case '}':
				case ']':
					return Index;

				case ',':	// Commas will simply be skipped over
				default:
					Index++;
					Index += Skip_Whitespace(String + Index);
					//return Index;
				}

			}

			return Index;
		}
	}

	//

	int Read_JSON_Object(JSON_Object* Target_Object, const char* String)	// Loads from string
	{
		if (!String)
		{
			printf(" >> Bad string!\n");
			return -1;
		}

		size_t Index = 0;

		Value Read_Value;

		Index = IO::Scan_JSON_Objects(String, &Read_Value);

		*Target_Object = std::move(Read_Value.Object);

		return 0;
	}

	int Load_JSON_Object(JSON_Object* Target_Object, const char* Filename)	// Loads from file
	{
		if (!Filename)
		{
			printf(" >> Bad directory!\n");
			return -1;
		}

		std::vector<char> String = Jaguar::Load_File_Contents(Filename);

		if (Read_JSON_Object(Target_Object, String.data()))
		{
			printf(" >> Failed to parse JSON string!\n");
			return -1;
		}

		Target_Object->Fields[JSON_Object_Filename].String = Filename;
		Target_Object->Fields[JSON_Object_Filename].Flag = Value::Type::T_String;

		return 0;
	}

	int Load_JSON_Reader_Buffers(JSON_Reader* Target_Reader, const char* Buffers)
	{
		std::string Parent_Directory = Target_Reader->Object[JSON_Object_Filename].String;

		size_t Index = Parent_Directory.find_last_of("/");
		Parent_Directory = Parent_Directory.substr(0, Index == std::string::npos ? 0 : Index + 1);	// Crude solution to get the parent 'local' directory of the .json file

		#if DEBUG
		if (!Target_Reader->Object.Fields.count(Buffers))
		{
			printf(" >> No buffers on JSON reader");
			return 0;									// Not necessarily an error
		}
		#endif

		for (Index = 0; Index < Target_Reader->Object[Buffers].Array.size(); Index++)
		{
			std::string URI = Parent_Directory + Target_Reader->Object[Buffers][Index]["uri"].String;
			Target_Reader->Buffer.push_back(Jaguar::Load_File_Contents(URI.c_str()));					// This loads the binary and adds it to the list of buffers
		}

		return 0;
	}
}