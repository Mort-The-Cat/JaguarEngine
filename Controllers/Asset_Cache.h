#ifndef JAGUAR_ASSET_CACHE
#define JAGUAR_ASSET_CACHE

#include "../OpenGL_Handling/OpenGL_Handling.h"
#include "../OpenGL_Handling/Texture_Uniform_Buffer.h"

namespace Jaguar
{
	struct Mesh_Cache_Info
	{
		const char* Name;
		Collada::Collada_Mesh* Mesh;	// This should be a pointer to prevent vector reallocations every time we call Pull_Mesh
										// This does however mean that we need to manually handle memory allocations for the mesh cache,
										// but we already handle buffer allocations/deallocations so that shouldn't be a problem
		Vertex_Buffer Buffer;

		bool operator<(const Mesh_Cache_Info& Other) const
		{
			return Buffer.Vertex_Buffer_ID < Other.Buffer.Vertex_Buffer_ID;
		}

		bool operator==(const Mesh_Cache_Info& Other) const
		{
			return Buffer.Vertex_Buffer_ID == Other.Buffer.Vertex_Buffer_ID;
		}
	};

	struct Texture_Cache_Info
	{
		// This will contain the pixel data as well as the texture buffer ID etc
		const char* Name;
		unsigned char* Pixel_Data;
		int Width, Height, Channels;	// Assume that each channel is unsigned 8-bits
		Texture Texture;
	};

	struct Skeleton_Cache_Info
	{
		const char* Name;
		Collada::Collada_Skeleton* Skeleton;	// This pointer will most likely be shared my many animator objects
												// So this is kept as a pointer to prevent the memory location from changing
	};

	struct Animation_Cache_Info
	{
		const char* Name;
		Collada::Collada_Animation* Animation;	// Pointer to animation that an animator object can use
	};

	struct Asset_Cache_Data
	{
		std::vector<Mesh_Cache_Info> Mesh_Cache;

		std::vector<Texture_Cache_Info> Texture_Cache;

		std::vector<Skeleton_Cache_Info> Skeleton_Cache;

		std::vector<Animation_Cache_Info> Animation_Cache;
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

#define LOAD_MESH_HINT_SKELETON_INCLUDED 1u // This flag bit lets the engine know that a skeleton should be created for this mesh (if one exists)
#define LOAD_MESH_HINT_LIGHTMAP_STATIC 2u	// This flag bit lets the engine know that this object will use lightmap coordinates
	Mesh_Cache_Info Get_Mesh_From_Buffer_ID(Asset_Cache_Data* Cache, GLuint Buffer_ID);
	Mesh_Cache_Info Pull_Mesh(Asset_Cache_Data* Cache, const char* Directory, unsigned char Flags = LOAD_MESH_HINT_SKELETON_INCLUDED);
	void Delete_All_Mesh_Cache(Asset_Cache_Data* Cache);

	Texture_Cache_Info Get_Texture_From_Buffer_ID(Asset_Cache_Data* Cache, GLuint Buffer_ID);
	Texture_Cache_Info Pull_Texture(Asset_Cache_Data* Cache, const char* Directory);
	void Delete_All_Texture_Cache(Asset_Cache_Data* Cache);

	Skeleton_Cache_Info Pull_Skeleton(Asset_Cache_Data* Cache, const char* Directory);
	void Delete_All_Skeleton_Cache(Asset_Cache_Data* Cache);

	Animation_Cache_Info Pull_Animation(Asset_Cache_Data* Cache, const char* Directory);
	void Delete_All_Animation_Cache(Asset_Cache_Data* Cache);
}


#endif