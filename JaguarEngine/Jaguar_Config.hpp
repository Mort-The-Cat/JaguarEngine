#ifndef JAGUAR_CONFIG
#define JAGUAR_CONFIG

#define DEBUG 0

#define JSON_Object_Filename "%filename%"

#define STB_IMAGE_IMPLEMENTATION
// This is used for stb_image so I'll keep it in the Jaguar config file

#include<string>
#include<vector>
#include<map>

#define UTF8(Bytes) Jaguar::Convert_UTF8(Bytes)

namespace Jaguar
{
	std::vector<char> Load_File_Contents(const char* Filename, bool Is_Binary = false);

	std::u32string Convert_UTF8(const char* Bytes);

	std::u32string Load_UTF8_File_Contents(const char* Filename);	// This convers from UTF8 to UTF32 and returns u32string
}

#endif