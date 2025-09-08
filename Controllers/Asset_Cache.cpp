#ifndef JAGUAR_ASSET_CACHE_DEFINITIONS
#define JAGUAR_ASSET_CACHE_DEFINITIONS

#include "../OpenGL_Handling/OpenGL_Handling.h"
#include "../Controllers/Jaguar_Engine_Wrapper.h"
#include "../Controllers/Asset_Cache.h"

#ifndef STB_IMAGE_IMPLEMENTATION 1
#define STB_IMAGE_IMPLEMENTATION 1

#include <stb/stb_image.h>

#endif

namespace Jaguar
{
	Texture_Cache_Info Pull_Texture(Asset_Cache_Data* Cache, const char* Directory)
	{
		Texture_Cache_Info Texture_Info;

		if (Search_Asset_Cache(Cache->Texture_Cache, Directory, &Texture_Info))
			return Texture_Info;

		// We'll need to load the image and save it here

		stbi_set_flip_vertically_on_load(true);

		Texture_Info.Pixel_Data = stbi_load(Directory, &Texture_Info.Width, &Texture_Info.Height, &Texture_Info.Channels, 4);

		Create_Texture_Buffer(
			&Texture_Info.Texture,
			GL_RGBA,			// We want to read as 4-channel image
			Texture_Info.Width,
			Texture_Info.Height,
			GL_RGBA,			// We ask for a 4-channel image in stbi_load, so that's what we should be getting
			GL_UNSIGNED_BYTE,
			Texture_Info.Pixel_Data
		);

		Texture_Info.Name = Directory;

		Cache->Texture_Cache.push_back(Texture_Info);

		return Texture_Info;
	}


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

	void Delete_All_Texture_Cache(Asset_Cache_Data* Cache)
	{
		// This will free all of the buffers in the texture cache as well as free the image memory

		for (size_t W = 0; W < Cache->Texture_Cache.size(); W++)
		{
			Destroy_Texture_Buffer(&Cache->Texture_Cache[W].Texture);
			stbi_image_free(Cache->Texture_Cache[W].Pixel_Data);
		}

		Cache->Texture_Cache.clear();
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