#include "JSON_Declarations.hpp"

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

			Value Buffer_Value;

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
					{
						Index += Scan_JSON_Objects(String + Index, Object, false);
					}
					Object->Flag = Value::Type::T_Object;
					Index++;
					return Index;

				case '[':	// a new array!
					Index++;
					while (String[Index] != ']')
					{
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

	int Read_JSON_Object(JSON_Object* Target_Object, const char* String)
	{
		if (!String)
		{
			printf("Bad string!");
			return 1;
		}

		size_t Index = 0;

		Value Read_Value;
		
		Index = IO::Scan_JSON_Objects(String, &Read_Value);

		*Target_Object = std::move(Read_Value.Object);

		return 0;
	}

}