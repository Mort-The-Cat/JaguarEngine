#include "Asset_Cache.hpp"
#include "JaguarEngine.hpp"

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

		// TODO: Maybe I'll make some error-handling here in case the engine fails to load a mesh or something

		Engine->Asset_Cache.Mesh_Cache.push_back(Mesh_Info);	// This stores the mesh info in the asset cache accordingly

		return Mesh_Info;
	}

	Mesh_Cache_Info Pull_Mesh(JaguarEngine* Engine, Mesh_Conversion Conversion, const char* Filename, bool Init_Vertex_Buffer)
	{
		Mesh_Cache_Info Mesh_Info;
		Mesh_Info.Name = Filename;
		Mesh_Info.Conversion = Conversion;

		if (Search_Asset_Cache(Engine->Asset_Cache.Mesh_Cache, &Mesh_Info))
			return Mesh_Info;

		JSON::JSON_Reader Reader;
		GLTF::GLTF_Object Object;

		JSON::Load_JSON_Object(&Reader.Object, Filename);
		JSON::Load_JSON_Reader_Buffers(&Reader);

		GLTF::Load_GLTF_Object(&Object, &Reader);

		// This loads in the JSON and subsequent GLTF object as necessary

		Mesh_Info.Mesh = Conversion(&Object, Init_Vertex_Buffer);

		Engine->Asset_Cache.Mesh_Cache.push_back(Mesh_Info);

		return Mesh_Info;
		
	}
}