#ifndef JAGUAR_ASSET_CACHE
#define JAGUAR_ASSET_CACHE

#include "../OpenGL_Handling/OpenGL_Handling.h"
#include "../OpenGL_Handling/Texture_Uniform_Buffer.h"

namespace Jaguar
{
	struct Mesh_Cache_Info
	{
		const char* Name;
		Collada::Collada_Mesh Mesh;
		Vertex_Buffer Buffer;
	};

	struct Texture_Cache_Info
	{
		// This will contain the pixel data as well as the texture buffer ID etc
		const char* Name;
		unsigned char* Pixel_Data;
		int Width, Height, Channels;	// Assume that each channel is unsigned 8-bits
		Texture Texture;
	};

	struct Asset_Cache_Data
	{
		std::vector<Mesh_Cache_Info> Mesh_Cache;

		std::vector<Texture_Cache_Info> Texture_Cache;
	};

	template<typename T>
	bool Search_Asset_Cache(const std::vector<T>& Cache, const char* Name, T* Target_Info)
	{
		for (size_t W = 0; W < Cache.size(); W++)
		{
			if (!strcmp(Name, Cache[W].Name))
			{
				*Target_Info = Cache[W];

				return true;
			}
		}

		return false;
	}

	struct Jaguar_Engine;

	Mesh_Cache_Info Pull_Mesh(Asset_Cache_Data* Cache, const char* Directory);
	void Delete_All_Mesh_Cache(Asset_Cache_Data* Cache);

	Texture_Cache_Info Pull_Texture(Asset_Cache_Data* Cache, const char* Directory);
	void Delete_All_Texture_Cache(Asset_Cache_Data* Cache);
}


#endif