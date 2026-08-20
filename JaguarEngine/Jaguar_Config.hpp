#ifndef JAGUAR_CONFIG
#define JAGUAR_CONFIG

#define DEBUG 0

#define JSON_Object_Filename "%filename%"

#define STB_IMAGE_IMPLEMENTATION
// This is used for stb_image so I'll keep it in the Jaguar config file

//#define __STDC_UTF_32__
//#define __STD_UTF_32__

#include<string>
#include<vector>
#include<map>

namespace Jaguar
{
	std::vector<char> Load_File_Contents(const char* Filename, bool Is_Binary = false);

	std::u32string Load_UTF8_File_Contents(const char* Filename);	// This convers from UTF8 to UTF32 and returns u32string
}

#endif