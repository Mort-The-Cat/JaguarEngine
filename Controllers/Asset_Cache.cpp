#ifndef JAGUAR_ASSET_CACHE_DEFINITIONS
#define JAGUAR_ASSET_CACHE_DEFINITIONS

#include "../OpenGL_Handling/OpenGL_Handling.h"
#include "../Controllers/Jaguar_Engine_Wrapper.h"
#include "../Controllers/Asset_Cache.h"

namespace Jaguar
{

	Mesh_Cache_Info Pull_Mesh(Asset_Cache_Data* Cache, const char* Directory)
	{
		Mesh_Cache_Info Mesh_Info;

		if (Search_Asset_Cache(Cache->Mesh_Cache, Directory, &Mesh_Info))
			return Mesh_Info;

		Collada::XML_Document Document; // Loads document
		Collada::Load_XML_Document(Directory, &Document); // Loads in document (could be optimised later)

		Collada::Load_Mesh(Document, &Mesh_Info.Mesh);

		// This will then generate a vertex buffer from this mesh

		Create_Vertex_Buffer(Mesh_Info.Mesh, &Mesh_Info.Buffer);

		// Then this will push the Mesh Cache Info to the asset cache

		Mesh_Info.Name = Directory;

		Cache->Mesh_Cache.push_back(Mesh_Info);

		return Mesh_Info;	// Returns mesh info
	}

	void Delete_All_Mesh_Cache(Asset_Cache_Data* Cache)
	{
		// This will free all of the memory in the mesh cache and clear it

		for (size_t W = 0; W < Cache->Mesh_Cache.size(); W++)
		{
			Destroy_Vertex_Buffer(&Cache->Mesh_Cache[W].Buffer);
		}

		Cache->Mesh_Cache.clear();	// There is nothing else which needs to be deallocated manually since the mesh vector info is automatically deallocated
	}

}

#endif