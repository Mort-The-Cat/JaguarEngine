#ifndef JAGUAR_JSON_DECLARATIONS
#define JAGUAR_JSON_DECLARATIONS

// This is a lightweight JSON-reader used by JaguarEngine for UI, scene data, and .gltf files

#include<map>
#include<vector>
//#include<memory>
#include<string>

#include<cassert>

namespace JSON
{
	struct Value;

	struct JSON_Reader	// This is a wrapper for a JSON file that can be used for including accessors, bufferviews, buffers, etc (as defined in .gltf files)
	{

	};

	struct JSON_Object
	{
		std::map<std::string, Value> Fields;

		Value& operator[](std::string Key)
		{
			return Fields[Key];
		}
	};

	//

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
#if DEBUG
			assert(Flag == Type::T_Array);
#endif

			return Array.at(Index);
		}

		Value& operator[](std::string Key)
		{
#if DEBUG
			assert(Flag == Type::T_Object);
#endif

			return Object[Key];
		}
	};

	//

	int Read_JSON_Object(JSON_Object* Target_Object, const char* String);
}

#endif