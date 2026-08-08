#include "Asset_Cache.hpp"
#include "JaguarEngine.hpp"

#include<stb/stb_image.h>

namespace Jaguar
{
	Mesh_Cache_Info Pull_Mesh(JaguarEngine* Engine, Mesh_Conversion Conversion, GLTF::GLTF_Object* Object, bool Init_Vertex_Buffer)
	{
		// NOTE: First check if we have this mesh in the asset cache already

		// If we do? Great! Return it

		// Otherwise? Load it and store it in the asset cache

		//return Mesh_Cache_Info();

		Mesh_Cache_Info Mesh_Info;
		Mesh_Info.Name = Object->Name;
		Mesh_Info.Conversion = Conversion;

		if (Search_Asset_Cache(Engine->Asset_Cache.Mesh_Cache, &Mesh_Info))
			return Mesh_Info;

		// Otherwise? Load it and store it in the asset cache

		Mesh_Info.Mesh = Conversion(Object, Init_Vertex_Buffer);

		Mesh_Info.Mesh->Name = Mesh_Info.Name.c_str();

		// TODO: Maybe I'll make some error-handling here in case the engine fails to load a mesh or something

		Engine->Asset_Cache.Mesh_Cache.push_back(Mesh_Info);	// This stores the mesh info in the asset cache accordingly

		return Mesh_Info;
	}

	Mesh_Cache_Info Pull_Mesh(JaguarEngine* Engine, Mesh_Conversion Conversion, const char* Directory, bool Init_Vertex_Buffer)
	{
		Mesh_Cache_Info Mesh_Info;
		Mesh_Info.Name = Directory;
		Mesh_Info.Conversion = Conversion;

		if (Search_Asset_Cache(Engine->Asset_Cache.Mesh_Cache, &Mesh_Info))
			return Mesh_Info;

		JSON::JSON_Reader Reader;
		GLTF::GLTF_Object Object;

		JSON::Load_JSON_Object(&Reader.Object, Directory);
		JSON::Load_JSON_Reader_Buffers(&Reader);

		GLTF::Load_GLTF_Object(&Object, &Reader);

		// This loads in the JSON and subsequent GLTF object as necessary

		Mesh_Info.Mesh = Conversion(&Object, Init_Vertex_Buffer);

		Mesh_Info.Mesh->Name = Mesh_Info.Name.c_str();

		Engine->Asset_Cache.Mesh_Cache.push_back(Mesh_Info);

		return Mesh_Info;
		
	}

	//

	Texture_Cache_Info Pull_Texture(JaguarEngine* Engine, const char* Directory)	// Will likely change this later
	{
		Texture_Cache_Info Texture_Info;
		Texture_Info.Name = Directory;

		if (Search_Asset_Cache(Engine->Asset_Cache.Texture_Cache, &Texture_Info))
			return Texture_Info;

		// Otherwise? Load/create texture, save in pool, and return

		Texture_Info.Pixel_Data = stbi_load(Directory, &Texture_Info.Width, &Texture_Info.Height, &Texture_Info.Channels, 4);	// 9/10 times I want a four-channel image
		
		if (Texture_Info.Pixel_Data == nullptr)
			printf(" >> Failed to load image! %s\n", Directory);

		Create_Texture_Buffer(
			&Texture_Info.Texture,
			GL_RGBA,
			Texture_Info.Width,
			Texture_Info.Height,
			GL_RGBA,
			GL_UNSIGNED_BYTE,		// 32bpp
			Texture_Info.Pixel_Data,
			true
		);

		Engine->Asset_Cache.Texture_Cache.push_back(Texture_Info);

		return Texture_Info;
	}

	Skeleton_Cache_Info Pull_Skeleton(JaguarEngine* Engine, GLTF::GLTF_Object* Object)
	{
		Skeleton_Cache_Info Skeleton_Info;
		Skeleton_Info.Name = Object->Name;

		if (Search_Asset_Cache(Engine->Asset_Cache.Skeleton_Cache, &Skeleton_Info))
			return Skeleton_Info;

		// otherwise? load and store in asset cache

		Skeleton_Info.Rig = new Skeleton();

		Create_Skeleton(Skeleton_Info.Rig, Object);

		Engine->Asset_Cache.Skeleton_Cache.push_back(Skeleton_Info);

		return Skeleton_Info;
	}

	Skeleton_Cache_Info Pull_Skeleton(JaguarEngine* Engine, const char* Directory)
	{
		Skeleton_Cache_Info Skeleton_Info;
		Skeleton_Info.Name = Directory;

		if (Search_Asset_Cache(Engine->Asset_Cache.Skeleton_Cache, &Skeleton_Info))
			return Skeleton_Info;

		// otherwise? load and store in asset cache

		Skeleton_Info.Rig = new Skeleton();

		Create_Skeleton(Skeleton_Info.Rig, Directory);

		Engine->Asset_Cache.Skeleton_Cache.push_back(Skeleton_Info);

		return Skeleton_Info;
	}

	//

	Animation_Cache_Info Pull_Animation(JaguarEngine* Engine, GLTF::GLTF_Object* Object, const char* Animation_Name)
	{
		Animation_Cache_Info Animation_Info;
		Animation_Info.Name = Object->Name;
		Animation_Info.Animation_Name = Animation_Name;	// NOTE: if "", it'll just return the first animation it finds

		if (Search_Asset_Cache(Engine->Asset_Cache.Animation_Cache, &Animation_Info))
			return Animation_Info;

		Animation_Info.Animation_Info = new Animation();
		Create_Animation(Animation_Info.Animation_Info, Object, Animation_Name);

		Engine->Asset_Cache.Animation_Cache.push_back(Animation_Info);

		return Animation_Info;
	}

	Animation_Cache_Info Pull_Animation(JaguarEngine* Engine, const char* Directory, const char* Animation_Name)
	{
		Animation_Cache_Info Animation_Info;
		Animation_Info.Name = Directory;
		Animation_Info.Animation_Name = Animation_Name;	// NOTE: if "", it'll just return the first animation it finds

		if (Search_Asset_Cache(Engine->Asset_Cache.Animation_Cache, &Animation_Info))
			return Animation_Info;

		Animation_Info.Animation_Info = new Animation();
		Create_Animation(Animation_Info.Animation_Info, Directory, Animation_Name);

		Engine->Asset_Cache.Animation_Cache.push_back(Animation_Info);

		return Animation_Info;
	}
}