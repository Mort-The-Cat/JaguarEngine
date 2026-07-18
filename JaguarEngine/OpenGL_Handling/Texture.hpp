#ifndef JAGUAR_TEXTURE
#define JAGUAR_TEXTURE

#include "OpenGL_Declarations.hpp"

namespace Jaguar
{

	struct Texture
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
		bool Generate_Mipmap = true);

	void Delete_Texture_Buffer(Texture* Target_Texture);
}

#endif