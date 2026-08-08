#ifndef JAGUAR_WORLD_OBJECT_DECLARATIONS
#define JAGUAR_WORLD_OBJECT_DECLARATIONS

#include "../Jaguar_Config.hpp"

#include "Model_Declarations.hpp"

namespace Jaguar
{
	struct Control;


#define MF_TO_BE_DELETED 0u
	class World_Object
	{
	public:
		char Flags[1] = { false };

		Model_Data Model;

		Control* Controller;	// This is the object that controls this world object (if any)

		glm::vec3 Position, Orientation, Orientation_Up;	// 3 important vectors for any world object

		World_Object() {}
	};

	struct Shader_Mesh_Wrapper
	{
		Shader Shader;
		Mesh* Mesh;
		std::vector<Texture> Textures;
	};

	void Create_World_Object(
		JaguarEngine* Engine,
		World_Object* Object,
		std::vector<Shader_Mesh_Wrapper> Meshes,
		std::vector<void*> Collision,
		Control* Controller,
		glm::vec3 Position = glm::vec3(0.0f),
		glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3 Orientation_Up = glm::vec3(0.0f, 1.0f, 0.0f)
	);

	void Delete_World_Object(World_Object* Object);
}

#endif