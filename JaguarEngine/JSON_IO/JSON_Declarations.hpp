#ifndef JAGUAR_JSON_DECLARATIONS
#define JAGUAR_JSON_DECLARATIONS

// This is a lightweight JSON-reader used by JaguarEngine for UI, scene data, and .gltf files

#include<map>
#include<vector>
#include<string>

#include<cassert>

#include "../Jaguar_Config.hpp"

#include "../OpenGL_Handling/OpenGL_Declarations.hpp"

namespace GLTF
{

	struct Attribute					// Attribute data and datatype
	{
	private:
		template<const int Dimension, typename T_1, typename T_0>
		void Copy_To_Buffer(std::vector<T_0>& Buffer, std::vector<char>& Bytes)
		{
			Buffer.resize(Data.size() / (Dimension * sizeof(T_1)));

			T_1* Read = (T_1*)Bytes.data();
			for (size_t Index = 0, Read_Index = 0; Index < Buffer.size(); Index++)
				for (size_t D = 0; D < Dimension; D++, Read_Index++)
					Buffer[Index][D] = Read[Read_Index];
		}

		template<const int Dimension, typename T_1, typename T_0>
		void Copy_To_Buffer(std::vector<glm::mat<Dimension, Dimension, T_0>>& Buffer, std::vector<char>& Bytes)
		{
			Buffer.resize(Data.size() / (Dimension * sizeof(T_1)));

			T_1* Read = (T_1*)Bytes.data();
			for (size_t Index = 0, Read_Index = 0; Index < Buffer.size(); Index++)
				for (size_t Column = 0; Column < Dimension; Column++)
					for (size_t Row = 0; Row < Dimension; Row++, Read_Index++)
						Buffer[Index][Column][Row] = Read[Read_Index];
		}
	public:
		std::vector<char> Data;
		GLint Dimension;				// 1 - scalar, 2/3/4 - vector, 9/16 - matrix
											// This is important because rotation can be either defined by a quaternion (vec4) OR a matrix of some kind (mat3 or mat4)
		GLint Type;						// the type of a single value of this attribute (i.e. float int32 uint32 int16 uint16 etc

		template<typename T> // This uses glm-style structures with a [] accessor and a ::length() term (representing number of elements)
		std::vector<T> Get_Attribute_Buffer()
		{
			std::vector<T> Buffer;


			switch (Type)
			{
			case GL_FLOAT:
				Copy_To_Buffer<T::length(), float>(Buffer, Data);
				return Buffer;
			case GL_INT:
			case GL_UNSIGNED_INT:
				Copy_To_Buffer<T::length(), int>(Buffer, Data);
				return Buffer;
			case GL_SHORT:
			case GL_UNSIGNED_SHORT:
				Copy_To_Buffer<T::length(), short>(Buffer, Data);
				return Buffer;
			case GL_BYTE:
			case GL_UNSIGNED_BYTE:
				Copy_To_Buffer<T::length(), char>(Buffer, Data);
				return Buffer;

			default:

#if DEBUG
				printf(" >> Invalid attribute type! %d\n", Type);
#endif

				return std::vector<T>(0u);					// return nothing
			}
		}
	};

}

namespace JSON
{
	struct Value;

#ifndef JSON_Object_Filename
	#define JSON_Object_Filename "%filename%"
#endif

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

	struct JSON_Reader	// This is a wrapper for a JSON file that can be used for including accessors, bufferviews, buffers, etc (as defined in .gltf files)
	{
		JSON_Object Object;

		std::vector<std::vector<char>> Buffer;	// This is the data, stored in 'buffer'

		Value& operator[](std::string Key)
		{
			return Object[Key];
		}

		std::vector<char>& operator[](size_t Index)
		{
			return Buffer[Index];
		}

		GLTF::Attribute Get_Accessor_Buffer(uint64_t Accessor_Index)
		{
			// "accessors" [Accessor_Index] -> "bufferViews" [bufferView] -> "buffers" [buffer] + byteOffset				(pointer to databuffer etc)
			// "accessors" [Accessor_Index] -> "componentType"		(datatype i.e. unsigned byte, unsigned short, unsigned int, float, etc)
			// "accessors" [Accessor_Index] -> "type"				(type i.e. scalar, vector2/3/4, matrix2/3/4)
			size_t Buffer_View;
			size_t Buffer_Index;
			size_t Byte_Offset;
			size_t Dimensions[2];	// Dimensions[0] is scalar/vector/matrix Dimensions[1] is 1/2/3/4
			size_t Type;			// GL_UNSIGNED_CHAR, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, GL_FLOAT

			Buffer_View = Object["accessors"][Accessor_Index]["bufferView"].Float;
			Buffer_Index = Object["bufferViews"][Buffer_View]["buffer"].Float;
			Byte_Offset = Object["bufferViews"][Buffer_View]["byteOffset"].Float;
			Type = Object["accessors"][Accessor_Index]["componentType"].Float;

			std::string Dim = Object["accessors"][Accessor_Index]["type"].String;
			// Only accepts SCALAR, VECx, or MATx
			Dimensions[0] = Dim[0] == 'V' | ((Dim[0] == 'M') << 1);	// 0 by default, 1 if vector, 2 if matrix
			Dimensions[1] = Dim.back() - '0';						// crude method to get digit value of last element

			size_t Size; // size of 1 entry

			switch (Type)			// Crude method to get size of a particular entry
			{
			case GL_FLOAT:
			case GL_UNSIGNED_INT:	// 32-bits
			case GL_INT:
				Size = 4;
				break;
			case GL_UNSIGNED_SHORT:	// 16-bits
			case GL_SHORT:
				Size = 2;
				break;
			case GL_UNSIGNED_BYTE:
			case GL_BYTE:
			default:
				Size = 1;			// assume 8-bits by default
			}

			if (Dimensions[0] == 2)
				Dimensions[1] *= Dimensions[1];

			if (Dimensions[0])		// if vector or matrix, multiply by number of axes
				Size *= Dimensions[1];
			else
				Dimensions[1] = 1;

			//if (Dimensions[0] == 2)	// if matrix, multiply it again
			//	Size *= Dimensions[1];	// The size of 1 entry

			size_t Count = Object["accessors"][Accessor_Index]["count"].Float;	// Number of entries

			Count *= Size;	// Size of all entries from this accessor

			GLTF::Attribute Accessor;
			//Accessor.Size = Size;

			Accessor.Dimension = Dimensions[1];	// 1 - scalar, 2/3/4 - vector, 9/16 - matrix
			Accessor.Type = Type;
			Accessor.Data.resize(Count);	// Number of bytes total
			memcpy(Accessor.Data.data(), Buffer[Buffer_Index].data() + Byte_Offset, Count);	// Copies the memory from the buffer

			return Accessor;				// returns attribute buffer
		}
	};

	//

	int Read_JSON_Object(JSON_Object* Target_Object, const char* String);				// Loads from string
	int Load_JSON_Object(JSON_Object* Target_Object, const char* String);				// Loads standalone JSON object from a file
	int Load_JSON_Reader_Buffers(JSON_Reader* Target_Reader, const char* Buffers = "buffers");		// Loads JSON reader 
}

#endif