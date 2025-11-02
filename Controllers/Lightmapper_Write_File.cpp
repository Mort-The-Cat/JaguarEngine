#include "Lightmapping.h"
#include "../OpenGL_Handling/Scene.h"
#include "../Collada_Loader/Collada_Loader.h"

namespace Jaguar
{
	void Write_Lightmap3_To_File(const char* Filename, glm::vec3* Data[3], unsigned int Texture_Dimensions)
	{
		std::ofstream File(Filename, std::ios::binary);

		if (!File.is_open())
			printf(" >> ERROR opening output lightmap file!\n");

		size_t Texture_Size = Texture_Dimensions;
		Texture_Size *= Texture_Size * sizeof(glm::vec3);

		File.write((const char*)&Texture_Dimensions, sizeof(Texture_Dimensions));

		for (size_t W = 0; W < 3; W++)
			File.write((const char*)Data[W], Texture_Size);

		File.close();
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
#if !TRIPLE_LIGHTMAPPING
		Create_Texture_Buffer(&Target_Lighting->Lightmap_Texture, GL_RGB32F, Texture_Dimensions, Texture_Dimensions, GL_RGB, GL_FLOAT, Data, true);
#endif

		Target_Lighting->Inverse_Lightmap_Scale = 1.0f / (float)Texture_Dimensions;

		// Creates lightmap! Now we can deallocate the data

		delete[] Data;
	}

	void Get_Lightmap3_From_File(const char* Filename, Lighting_Data* Target_Lighting)
	{
		std::ifstream File(Filename, std::ios::binary);

		if (!File.is_open())
		{
			printf(" >> ERROR reading lightmap file!\n");

			return;
		}

		unsigned int Texture_Dimensions;
		File.read((char*)&Texture_Dimensions, sizeof(Texture_Dimensions));

		size_t Texture_Size = Texture_Dimensions * Texture_Dimensions;

		Target_Lighting->Inverse_Lightmap_Scale = 1.0f / (float)Texture_Dimensions;

		glm::vec3* Data = new glm::vec3[Texture_Size];

		for (size_t W = 0; W < 3; W++)
		{
			File.read((char*)Data, Texture_Size * sizeof(glm::vec3));

#if TRIPLE_LIGHTMAPPING
			Create_Texture_Buffer(&Target_Lighting->Lightmap_Textures[W], GL_RGB32F, Texture_Dimensions, Texture_Dimensions, GL_RGB, GL_FLOAT, Data, true);
#endif
		}

		File.close();

		delete[] Data;
	}

	//

	void Get_Triple_Lightmap_Vectors(glm::vec3 Normal, glm::vec3 Tangent, glm::vec3 Bitangent, glm::vec3 Components[3])
	{
		// "Components" is the return array

		const float Sqrt_Sixth = sqrtf(1.0f / 6);
		const float Sqrt_Half = sqrtf(0.5f);
		const float Sqrt_Third = sqrtf(1.0f / 3.0f);
		const float Sqrt_Three_Halves = sqrtf(1.5f);

		Normal *= Sqrt_Third;

		Bitangent *= Sqrt_Half;

		for (size_t W = 0; W < 3; W++)
			Components[W] = Normal;

		Components[2] += Sqrt_Three_Halves * Tangent;

		Tangent *= Sqrt_Sixth;

		for (size_t W = 0; W < 2; W++)
			Components[W] -= Tangent;

		Components[0] -= Bitangent;
		Components[1] += Bitangent;

		// Done! little fiddly to compute but it's fine
		// I'll come up with a faster method for the shader code
	}

	void Get_UV_Tangent_Bitangent_Vectors(glm::vec3 Positions[3], glm::vec2 Texture_Coordinates[3], glm::vec3 Normal, glm::vec3* Tangent, glm::vec3* Bitangent)
	{
		glm::vec2 UV_A, UV_B;
		glm::vec3 Edge_A, Edge_B;

		float Inv;

		UV_A = Texture_Coordinates[1] - Texture_Coordinates[0];
		UV_B = Texture_Coordinates[2] - Texture_Coordinates[0];

		Edge_A = Positions[1] - Positions[0];
		Edge_B = Positions[2] - Positions[0];

		Inv = 1.0f / (UV_A.x * UV_B.y - UV_B.x * UV_A.y);

		*Tangent = -glm::normalize(Inv * (Edge_A * UV_B.y - Edge_B * UV_A.y));
		*Bitangent = glm::normalize(glm::cross(*Tangent, Normal));
	}
}