#ifndef JAGUAR_TEXTURE_UNIFORM_BUFFER
#define JAGUAR_TEXTURE_UNIFORM_BUFFER

#include "OpenGL_Handling.h"

namespace Jaguar
{

	struct Texture	// May add more fields to this later
	{
		GLuint Texture_Buffer_ID;
	};

	void Create_Texture_Buffer(
		Texture* Target_Texture,		// Just pointer to the Jaguar::Texture struct that will be initialised
		GLuint Target_Colour_Format,	// GL_RGBA or GL_RGB typically
		GLuint Width,
		GLuint Height,
		GLuint Source_Colour_Format,	// GL_RGBA or GL_RGB typically
		GLuint Data_Type,				// Generally GL_UNSIGNED_BYTE, but generally use GL_FLOAT for G-buffer render targets
		const void* Pixel_Data,			// If NULL pointer is parsed, no issue
		bool Generate_Mipmap = true);	// Defaults to generating mipmap. Set this to false for render buffers etc
		// Can be used for both the creation of general image texture buffers and render target textures
		// (Use other functions for cubemap textures)


	void Destroy_Texture_Buffer(Texture* Target_Texture);	
		// This will just deallocate the texture buffer we assigned
		// Note that textures which draw from Pull_Texture() function require additional deallocations
}


#endif