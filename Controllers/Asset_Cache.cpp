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
	Animation_Cache_Info Pull_Animation(Asset_Cache_Data* Cache, const char* Directory)
	{
		Animation_Cache_Info Animation_Info;

		if (Search_Asset_Cache(Cache->Animation_Cache, Directory, &Animation_Info))
			return Animation_Info;

		Collada::XML_Document Document;
		Animation_Info.Animation = new Collada::Collada_Animation();

		Collada::Load_XML_Document(Directory, &Document);
		Collada::Load_Animation(Document, Animation_Info.Animation);

		Animation_Info.Name = Directory;

		Cache->Animation_Cache.push_back(Animation_Info);

		return Animation_Info;
	}

	Skeleton_Cache_Info Pull_Skeleton(Asset_Cache_Data* Cache, const char* Directory)
	{
		Skeleton_Cache_Info Skeleton_Info;

		if (Search_Asset_Cache(Cache->Skeleton_Cache, Directory, &Skeleton_Info))
			return Skeleton_Info;

		// Otherwise, we'll need to load a new one in

		Collada::XML_Document Document;
		Skeleton_Info.Skeleton = new Collada::Collada_Skeleton(); // Allocates required skeleton memory

		Collada::Load_XML_Document(Directory, &Document);
		Collada::Load_Skeleton(Document, Skeleton_Info.Skeleton);

		Skeleton_Info.Name = Directory;

		Cache->Skeleton_Cache.push_back(Skeleton_Info);

		return Skeleton_Info;
	}

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

	Mesh_Cache_Info Get_Mesh_From_Buffer_ID(Asset_Cache_Data* Cache, GLuint Buffer_ID)
	{
		for (size_t W = 1; W < Cache->Mesh_Cache.size(); W++)
			if (Cache->Mesh_Cache[W].Buffer.Vertex_Buffer_ID == Buffer_ID)
				return Cache->Mesh_Cache[W];

		return Cache->Mesh_Cache[0]; // Rubbish data
	}

	Texture_Cache_Info Get_Texture_From_Buffer_ID(Asset_Cache_Data* Cache, GLuint Buffer_ID)
	{
		for (size_t W = 1; W < Cache->Texture_Cache.size(); W++)
			if (Cache->Texture_Cache[W].Texture.Texture_Buffer_ID == Buffer_ID)
				return Cache->Texture_Cache[W];

		return Cache->Texture_Cache[0];
	}

	Mesh_Cache_Info Pull_Mesh(Asset_Cache_Data* Cache, const char* Directory, unsigned char Flags)
	{
		Mesh_Cache_Info Mesh_Info;

		if (Search_Asset_Cache(Cache->Mesh_Cache, Directory, &Mesh_Info))
			return Mesh_Info;

		Collada::XML_Document Document; // Loads document
		Collada::Load_XML_Document(Directory, &Document); // Loads in document (could be optimised later)

		Mesh_Info.Mesh = new Collada::Collada_Mesh(); // Allocates memory

		Collada::Collada_Skeleton* Skeleton = nullptr;

		if (Flags & LOAD_MESH_HINT_SKELETON_INCLUDED)	// If we should load skeleton?
		{
			Collada::XML_Document Libraries = Document["COLLADA"][0];
			if (Libraries.Nodes.find("library_controllers") != Libraries.Nodes.end())	// If there's animation data? 
			{
				Skeleton = Pull_Skeleton(Cache, Directory).Skeleton;	// Load skeleton into memory
			}
		}

		Collada::Load_Mesh(Document, Mesh_Info.Mesh, Skeleton);

		// This will then generate a vertex buffer from this mesh

		Create_Vertex_Buffer(Mesh_Info.Mesh, &Mesh_Info.Buffer);

		if (Flags & LOAD_MESH_HINT_LIGHTMAP_STATIC)
			Initialise_Static_Lightmap_Vertex_Attributes(&Mesh_Info.Buffer);
		else
			Initialise_Joint_Vertex_Attributes(&Mesh_Info.Buffer);

		// Then this will push the Mesh Cache Info to the asset cache

		Mesh_Info.Name = Directory;

		Cache->Mesh_Cache.push_back(Mesh_Info);

		return Mesh_Info;	// Returns mesh info
	}

	void Delete_All_Animation_Cache(Asset_Cache_Data* Cache)
	{
		for (size_t W = 0; W < Cache->Animation_Cache.size(); W++)
			delete Cache->Animation_Cache[W].Animation;

		Cache->Animation_Cache.clear();
	}

	void Delete_All_Skeleton_Cache(Asset_Cache_Data* Cache)
	{
		// This will need to deallocate the skeleton memory we assigned

		for (size_t W = 0; W < Cache->Skeleton_Cache.size(); W++)
		{
			delete Cache->Skeleton_Cache[W].Skeleton; // Deallocates pointer
		}

		Cache->Skeleton_Cache.clear(); // Clears cache vector
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
			delete Cache->Mesh_Cache[W].Mesh; // Deallocates mesh data

			// Very important!!!
		}

		Cache->Mesh_Cache.clear();	// There is nothing else which needs to be deallocated manually
	}

}

#endif