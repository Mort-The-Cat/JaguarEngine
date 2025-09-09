#ifndef JAGUAR_SCENE_DECLARATIONS
#define JAGUAR_SCENE_DECLARATIONS

#include "OpenGL_Handling.h"
#include "Render_Queue.h"
#include "Texture_Uniform_Buffer.h"

namespace Jaguar
{
	class Controller; // These objects control what a World_Object does

	class World_Object	// All world-objects will be stored in larger "object pool" which will be responsible for their allocation/deallocation
	{					// This is such that a deleted object's reference can be removed from render queues, hitbox pools, and THEN will be deleted from the scene
#define MF_TO_BE_DELETED 0u

	public:
		bool Flags[1] = { false }; // Will be used later

		glm::vec3 Position;
		glm::vec3 Orientation;		// Used to generate direction matrix
		glm::vec3 Orientation_Up;	// Used to generate direction matrix

		Controller* Control; // This handles object-related logic

		void* Uniform_Buffer;	// This is a pointer to any additional uniform buffer information (such as skeletal animations etc)

		Texture Albedo;
		// Texture Material;

		Vertex_Buffer Mesh;			// Bound and rendered (can be shared by multiple world objects, pulled from same asset pool)
	};

	void Destroy_World_Object(World_Object* Target_Object);

	glm::mat4 Get_Model_Matrix(const World_Object* Object);

	struct Scene_Data	// The Scene_Data object is responsible for memory management of world objects as well as
	{					// lighting, hitboxes, and camera information for the scene
		std::vector<World_Object*> Objects;

		glm::mat4 Camera_Projection_Matrix;
	};

	void Handle_Scene_Deletions(Scene_Data* Target_Scene);
	void Delete_All(Scene_Data* Target_Scene);

	void Add_Scene_Object(Scene_Data* Target_Scene, World_Object* New_Object, Render_Pipeline* Target_Pipeline = nullptr, const Shader* Shader_Program = nullptr);
}

#endif