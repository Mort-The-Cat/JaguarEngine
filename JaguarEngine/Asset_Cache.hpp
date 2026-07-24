#ifndef JAGUAR_ASSET_CACHE
#define JAGUAR_ASSET_CACHE

#include<vector>
#include<string>

#include "GLTF_Reader/GLTF_Declarations.hpp"

#include "../JaguarEngine/Animation/Animation.hpp"

namespace Jaguar
{
	class Mesh;

	// there are obviously different mesh types,
	// I think these can be differentiated at runtime 
	// using the function that converted them from GLTF to the mesh
	// 
	// Since that's just a single 64-bit pointer, 
	// it's really easy to use as a key into a hashtable etc

	typedef Mesh*(*Mesh_Conversion)(GLTF::GLTF_Object*, bool);

	struct Mesh_Cache_Info
	{
		std::string Name;			// The name of this mesh
		Mesh* Mesh;					// mesh data itself
		Mesh_Conversion Conversion;	// Function that was used to create this mesh
		bool operator==(const Mesh_Cache_Info& Other) const
		{
			return
				Conversion == Other.Conversion &&
				Name == Other.Name;
				//strcmp(Name, Other.Name) == 0;
		}
	};

	struct Texture_Cache_Info
	{
		std::string Name;
		unsigned char* Pixel_Data;
		int Width, Height, Channels, Type;
		Texture Texture;
		bool operator==(const Texture_Cache_Info& Other) const
		{
			return
				Name == Other.Name;
				//strcmp(Name, Other.Name) == 0;
		}
	};

	struct Animation_Cache_Info
	{
		std::string Name;			// Filename
		std::string Animation_Name;	// name of the animation/action itself
		Animation* Animation_Info;
		bool operator==(const Animation_Cache_Info& Other) const
		{
			return
				Name == Other.Name &&
				Animation_Name == Other.Animation_Name;
		}
	};

	struct Skeleton_Cache_Info
	{
		std::string Name;
		Skeleton* Rig;
		bool operator==(const Skeleton_Cache_Info& Other) const
		{
			return Name == Other.Name;
		}
	};

	// Animations will also be cached here in a fairly standard format

	struct Asset_Cache_Data
	{
		std::vector<Mesh_Cache_Info> Mesh_Cache;

		std::vector<Texture_Cache_Info> Texture_Cache;

		std::vector<Animation_Cache_Info> Animation_Cache;
		std::vector<Skeleton_Cache_Info> Skeleton_Cache;
	};

	template<typename Info>
	bool Search_Asset_Cache(const std::vector<Info>& Cache, Info* Object_Info)
	{
		for(size_t Index = 0; Index < Cache.size(); Index++)
			if (Cache[Index] == *Object_Info)
			{
				*Object_Info = Cache[Index];	// Copy over object info (note: copy, not move)
				return true;					// Cache hit!
			}

		return false;							// Cache miss
	}

	Mesh_Cache_Info Pull_Mesh(JaguarEngine* Engine, Mesh_Conversion Conversion, GLTF::GLTF_Object* Object, bool Init_Vertex_Buffer = true);
	Mesh_Cache_Info Pull_Mesh(JaguarEngine* Engine, Mesh_Conversion Conversion, const char* Directory, bool Init_Vertex_Buffer = true);

	// You can either load it directly from a GLTF_Object or from a filename

	Texture_Cache_Info Pull_Texture(JaguarEngine* Engine, const char* Directory); // Will likely add more features to this later
}


#endif