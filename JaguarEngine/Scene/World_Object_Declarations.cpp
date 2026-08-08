#include "../JaguarEngine.hpp"

namespace Jaguar
{
	void Delete_World_Object(World_Object* Object)
	{
		if (Object->Controller)
			delete Object->Controller;

		delete Object;	// When this is called, the destructor for the model data is also called
	}

	void Init_Control(JaguarEngine* Engine, World_Object* Object)
	{
		Object->Controller->Object = Object; // Simple
	}

	void Create_World_Object(
		JaguarEngine* Engine,
		World_Object* Object,
		std::vector<Shader_Mesh_Wrapper> Meshes,
		std::vector<void*> Collision,
		Control* Controller,
		glm::vec3 Position,
		glm::vec3 Orientation,
		glm::vec3 Orientation_Up
	)
	{
		// This will push the meshes to the corresponding render queues

		Object->Model.Meshes.resize(Meshes.size());

		for (size_t Mesh_Index = 0; Mesh_Index < Meshes.size(); Mesh_Index++)
		{
			//Object->Model.Meshes[Mesh_Index]->Textures = std::move(Meshes[Mesh_Index].Textures);

			Object->Model.Meshes[Mesh_Index] = Meshes[Mesh_Index].Shader.Create_Mesh_Wrapper(Meshes[Mesh_Index].Mesh, Meshes[Mesh_Index].Textures);
		
			Engine->Pipeline.Queues[Engine->Pipeline.Queue_Table[Meshes[Mesh_Index].Shader.Program_ID]].Models.push_back
			(
				{ Object->Model.Meshes[Mesh_Index], Object }
			);
		}

		Object->Controller = Controller;

		// TODO: Add collisions etc

		Engine->Scene.Objects.push_back(Object);

		Object->Position = Position;
		Object->Orientation = Orientation;
		Object->Orientation_Up = Orientation_Up;

		if(Controller)
			if (Controller->Init)						// If there's an init function?
				Controller->Init(Engine, Object);	// Call it
	}

}