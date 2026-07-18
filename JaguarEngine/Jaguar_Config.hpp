#ifndef JAGUAR_CONFIG
#define JAGUAR_CONFIG

#include<vector>
#include<map>

#define DEBUG 0

#define JSON_Object_Filename "%filename%"

#define STB_IMAGE_IMPLEMENTATION
// This is used for stb_image so I'll keep it in the Jaguar config file

namespace Jaguar
{
	std::vector<char> Load_File_Contents(const char* Filename, bool Is_Binary = false);
}

#endif