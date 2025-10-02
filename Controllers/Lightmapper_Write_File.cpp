#include "Lightmapping.h"
#include "../OpenGL_Handling/Scene.h"
#include "../Collada_Loader/Collada_Loader.h"

namespace Jaguar
{
	void Write_Lightmap3_To_File(const char* Filename, glm::vec3* Data[3], unsigned int Texture_Dimensions)
	{

	}

	void Write_Lightmap_To_File(const char* Filename, glm::vec3* Data, unsigned int Texture_Dimensions)
	{
		// Needs to load binary data into file

		std::ofstream File(Filename, std::ios::binary);

		if (!File.is_open())
			printf(" >> ERROR opening output lightmap file!\n");			// Throw_Error(" >> Error opening lightmap output file! %s\n");
		
		size_t Texture_Size = Texture_Dimensions;
		Texture_Size *= Texture_Size * sizeof(glm::vec3);	// Gets texture size in bytes

		File.write((const char*)&Texture_Dimensions, sizeof(Texture_Dimensions));			// writes texture size into file firstly

		File.write((const char*)Data, Texture_Size);										// Then writes rest of file into the binary

		File.close();

		// Done!
	}


	void Get_Lightmap_From_File(const char* Filename, Lighting_Data* Target_Lighting)	// Loads lightmap data and generates texture from it
	{
		std::ifstream File(Filename, std::ios::binary);

		if (!File.is_open())
			printf(" >> ERROR reading lightmap file!\n");					// Throw_Error(" >> Error reading lightmap file! %s\n");

		unsigned int Texture_Dimensions;

		File.read((char*)&Texture_Dimensions, sizeof(Texture_Dimensions));

		size_t Texture_Size = Texture_Dimensions * Texture_Dimensions; // Squares area

		// Reads texture dimensions!

		// Time to allocate data...

		glm::vec3* Data = new glm::vec3[Texture_Size];

		File.read((char*)Data, Texture_Size * sizeof(glm::vec3));

		File.close();

		Create_Texture_Buffer(&Target_Lighting->Lightmap_Texture, GL_RGB32F, Texture_Dimensions, Texture_Dimensions, GL_RGB, GL_FLOAT, Data, true);

		Target_Lighting->Inverse_Lightmap_Scale = 1.0f / (float)Texture_Dimensions;

		// Creates lightmap! Now we can deallocate the data

		delete[] Data;
	}

	void Get_Lightmap3_From_File(const char* Filename, Lighting_Data* Target_Lighting)
	{

	}
}