#ifndef JAGUAR_ASSET_CACHE
#define JAGUAR_ASSET_CACHE

#include<vector>
#include<string>

#include "GLTF_Reader/GLTF_Declarations.hpp"

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
		Mesh* Mesh;					// mesh data itself
		Mesh_Conversion Conversion;	// Function that was used to create this mesh
		std::string Name;			// The name of this mesh
	};

	struct Asset_Cache_Data
	{
		std::vector<Mesh_Cache_Info> Mesh_Cache;
	};

	Mesh_Cache_Info Pull_Mesh(JaguarEngine* Engine, Mesh_Conversion Conversion, GLTF::GLTF_Object* Object, bool Init_Vertex_Buffer = true);
	Mesh_Cache_Info Pull_Mesh(JaguarEngine* Engine, Mesh_Conversion Conversion, const char* Filename, bool Init_Vertex_Buffer = true);

	// You can either load it directly from a GLTF_Object or from a filename
}


#endif