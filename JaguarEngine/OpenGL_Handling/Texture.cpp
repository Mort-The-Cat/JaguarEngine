#include "../JaguarEngine.hpp"

namespace Jaguar
{
	void Attach_Texture(Texture* Target_Texture, Shader* Target_Shader, const char* Texture_Name, int Texture_Number)
	{
		glUniform1i(glGetUniformLocation(Target_Shader->Program_ID, Texture_Name), Texture_Number);
		glActiveTexture(GL_TEXTURE0 + Texture_Number);
		glBindTexture(GL_TEXTURE_2D, Target_Texture->Texture_Buffer_ID);
	}

	void Delete_Texture_Buffer(Texture* Target_Texture)
	{
		glBindTexture(GL_TEXTURE_2D, Target_Texture->Texture_Buffer_ID);
		glDeleteTextures(1, &Target_Texture->Texture_Buffer_ID);
	}

	void Create_Texture_Buffer(
		Texture* Target_Texture,		// Just pointer to the Jaguar::Texture struct that will be initialised
		GLuint Target_Colour_Format,	// GL_RGBA or GL_RGB typically
		GLuint Width,
		GLuint Height,
		GLuint Source_Colour_Format,	// GL_RGBA or GL_RGB typically
		GLuint Data_Type,				// Generally GL_UNSIGNED_BYTE, but generally use GL_FLOAT for G-buffer render targets
		const void* Pixel_Data,			// If NULL pointer is parsed, no issue
		bool Generate_Mipmap
	)
	{
		glGenTextures(1, &Target_Texture->Texture_Buffer_ID);

		glBindTexture(GL_TEXTURE_2D, Target_Texture->Texture_Buffer_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Generate_Mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, Target_Colour_Format, Width, Height, 0, Source_Colour_Format, Data_Type, Pixel_Data);

		if (Generate_Mipmap)
			glGenerateMipmap(GL_TEXTURE_2D);
	}

}