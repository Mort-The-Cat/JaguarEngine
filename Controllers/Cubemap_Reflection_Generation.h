#ifndef JAGUAR_CUBEMAP
#define JAGUAR_CUBEMAP

#include "../OpenGL_Handling/OpenGL_Handling.h"
#include "../Collada_Loader/Collada_Loader.h"
#include "../Controllers/Asset_Cache.h"

namespace Jaguar
{
	struct Cubemap
	{
		// This will include bounds, origin, and the cubemap texture object
		// need to create a simple create/delete function for the cubemap
		// as well as a function to bind the cubemap and cubemap info to the shader for rendering etc

		glm::vec3 A, B, Origin; // A and B are the bounds of the cubemap AABB

		GLuint Cubemap_Texture; // This is the cubemap texture id
	};

	void Generate_Cubemap(Jaguar::Jaguar_Engine* Engine, Cubemap* Target_Cubemap, unsigned int Width = 1024, unsigned Height = 1024);
}

#endif