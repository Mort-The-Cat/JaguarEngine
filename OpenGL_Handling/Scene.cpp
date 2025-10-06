#include "Scene.h"
#include "OpenGL_Handling.h"
#include "Render_Queue.h"

#include "../Controllers/Jaguar_Engine_Wrapper.h"

namespace Jaguar
{
	glm::mat4 Get_Model_Matrix(const World_Object* Object) // Generates model transformation matrix from orientation vectors and world-object's position
	{
		glm::vec3 Right = glm::cross(Object->Orientation, Object->Orientation_Up);
		return glm::mat4(																// Note that OpenGL stores matrices in Column-Major format
			Right.x, Right.y, Right.z, 0,
			Object->Orientation_Up.x, Object->Orientation_Up.y, Object->Orientation_Up.z, 0,
			Object->Orientation.x, Object->Orientation.y, Object->Orientation.z, 0,
			Object->Position.x, Object->Position.y, Object->Position.z, 1
		);

		/*
		Matrix element indices in column-major format (used by OpenGL)
			|	0	3	6	|
			|	1	4	7	|
			|	2	5	8	|
		
		*/
	}

	void Add_Scene_Object(Scene_Data* Target_Scene, World_Object* New_Object, Render_Pipeline* Target_Pipeline, const Shader* Shader_Program)
	{
		Target_Scene->Objects.push_back(New_Object);

		if (Target_Pipeline)
		{
			Target_Pipeline->Render_Queues[
				Target_Pipeline->Queue_Table[Shader_Program->Program_ID]	// Gets the queue we want
			].Objects.push_back(New_Object);								// Adds pointer to this object into render queue
		}
	}

	void Create_World_Object(Jaguar_Engine* Engine, World_Object* Object, Shader* Object_Shader,
		Vertex_Buffer Mesh_Buffer,
		Texture Albedo,
		Texture Normal,
		Controller* Control,
		glm::vec3 Position,
		glm::vec3 Orientation,
		glm::vec3 Orientation_Up
	)
	{
		Object->Mesh = Mesh_Buffer;
		Object->Albedo = Albedo;
		Object->Normal = Normal;
		Object->Control = Control;
		Object->Position = Position;
		Object->Orientation = Orientation;
		Object->Orientation_Up = Orientation_Up;

		Add_Scene_Object(&Engine->Scene, Object, &Engine->Pipeline, Object_Shader);
	}


	void Destroy_World_Object(World_Object* Target_Object)
	{
		delete Target_Object; // There will be more code added here later, related to controllers and hitboxes
	}
}