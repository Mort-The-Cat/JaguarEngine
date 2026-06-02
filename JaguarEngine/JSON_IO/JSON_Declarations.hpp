#ifndef JAGUAR_JSON_DECLARATIONS
#define JAGUAR_JSON_DECLARATIONS

// This is a lightweight JSON-reader used by JaguarEngine for UI, scene data, and .gltf files

#include<map>
#include<vector>
//#include<memory>
#include<string>

namespace JSON
{
	struct Value;

	struct JSON_Object
	{
		std::map<std::string, Value> Fields;

		Value& operator[](std::string Key)
		{
			return Fields[Key];
		}
	};

	struct Value
	{
		union
		{
			//uint64_t Int;
			double Float;		// In many instances, float/int can't be differentiated by JSON file alone
			bool Boolean;
		};

		std::string String;

		JSON_Object Object;
		std::vector<Value> Array;

		enum Type
		{
			T_Unassigned = 0,
			T_Float,
			T_Boolean,
			T_String,
			T_Object,
			T_Array
		};

		Type Flag = Type::T_Unassigned;

		Value& operator[](size_t Index)
		{
			return Array.at(Index);
		}
	};

	//

	int Read_JSON_Object(JSON_Object* Target_Object, const char* String);
}

#endif