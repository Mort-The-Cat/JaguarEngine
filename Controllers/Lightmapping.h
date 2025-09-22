#ifndef JAGUAR_LIGHTMAPPING
#define JAGUAR_LIGHTMAPPING

#include "../OpenGL_Handling/OpenGL_Handling.h"
#include "../Collada_Loader/Collada_Loader.h"
#include "../Controllers/Asset_Cache.h"

namespace Jaguar
{

#define LF_STATIC 1u
	struct Lightsource
	{
		glm::vec3 Colour;
		glm::vec3 Position;
		glm::vec3 Direction;	// This is a direction vector
		float Radius = 1.0f;	// Physical radius of this light

		// I'll implement FOV and other things later

		bool Flags[1] = { false };

	};

	// Lightmapping structures go here

	class Lightmap_Tri
	{
	public:
		size_t Index;
		int Size;
		Mesh_Cache_Info Mesh;

		Lightmap_Tri(Mesh_Cache_Info Meshp, size_t Indexp, int Sizep)
		{
			Mesh = Meshp;
			Index = Indexp;
			Size = Sizep;
		}
	};

	struct World_Object;

	class Lightmap_Chart
	{
	public:
		// This stores all of the occupied pixels.
		// The mesh objects are updated to handle all of the lightmap UVs

		std::vector<std::vector<unsigned char>> Occupied; // Sidelength x sidelength long

		std::vector<Lightmap_Tri> Pushed_Tris;
		std::vector<World_Object*> Pushed_Objects;

		// 0 = not occupied, 1 = occupied

		unsigned int Sidelength;
		// Lightmap charts are always squares so sidelength is all that's necessary
	};

	struct Render_Queue;
	struct Jaguar_Engine;

	void Init_Lightmap_Chart(Lightmap_Chart* Target_Chart);

	void Assemble_Lightmap_Chart(Lightmap_Chart* Target_Chart);
	void Push_Queue_Lightmap_Chart(Jaguar_Engine* Engine, const Render_Queue* Queue, Lightmap_Chart* Target_Chart);
	void Create_Lightmap_From_Chart(Jaguar_Engine* Engine, Texture* Lightmap_Texture, Lightmap_Chart* Target_Chart);

	struct Shader;
	struct Scene_Data;

	void Lightmapped_Shader_Init_Function(const Shader* Target_Shader, const Scene_Data* Scene);

	void Upsize_Chart(Lightmap_Chart* Target_Chart);
}

#endif