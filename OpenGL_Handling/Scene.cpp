#include "Scene.h"
#include "OpenGL_Handling.h"
#include "Render_Queue.h"

namespace Jaguar
{
	glm::mat4 Get_Model_Matrix(const World_Object* Object)
	{
		glm::vec3 Right = glm::cross(Object->Orientation, Object->Orientation_Up);
		return glm::mat4(
			Right.x, Right.y, Right.z, 0,
			Object->Orientation_Up.x, Object->Orientation_Up.y, Object->Orientation_Up.z, 0,
			Object->Orientation.x, Object->Orientation.y, Object->Orientation.z, 0,
			Object->Position.x, Object->Position.y, Object->Position.z, 1
		);
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

	void Destroy_World_Object(World_Object* Target_Object)
	{
		delete Target_Object; // There will be more code added here later, related to controllers and hitboxes
	}
}