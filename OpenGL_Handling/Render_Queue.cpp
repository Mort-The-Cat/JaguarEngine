#ifndef JAGUAR_RENDER_QUEUE
#define JAGUAR_RENDER_QUEUE

#include "../Collada_Loader/Collada_Loader.h"
#include "../OpenGL_Handling/OpenGL_Handling.h"
#include "../OpenGL_Handling/Render_Queue.h"
#include "../OpenGL_Handling/Scene.h"

#include "../Controllers/Skeletal_Animation_Handling.h"

namespace Jaguar
{
	// Note that this likely will NOT include particle rendering as that's so fundamentally different to how we're rendering actors here

	Render_Queue* Get_Render_Queue(Render_Pipeline* Target_Pipeline, const Shader* Target_Shader)
	{
		return &Target_Pipeline->Render_Queues[Target_Pipeline->Queue_Table.at(Target_Shader->Program_ID)];
	}

	void Default_Shader_Init_Function(const Shader* Target_Shader, const Scene_Data* Scene)
	{
		glUniformMatrix4fv(glGetUniformLocation(Target_Shader->Program_ID, "Projection_Matrix"), 1, GL_FALSE, glm::value_ptr(Scene->Camera_Projection_Matrix));
	}

	void Lightmapped_Shader_Init_Function(const Shader* Target_Shader, const Scene_Data* Scene)
	{
		glUniformMatrix4fv(glGetUniformLocation(Target_Shader->Program_ID, "Projection_Matrix"), 1, GL_FALSE, glm::value_ptr(Scene->Camera_Projection_Matrix));

		glUniform1f(glGetUniformLocation(Target_Shader->Program_ID, "Inverse_Lightmap_Size"), Scene->Lighting.Inverse_Lightmap_Scale);

		glUniform1i(glGetUniformLocation(Target_Shader->Program_ID, "Lightmap_Texture"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Scene->Lighting.Lightmap_Texture.Texture_Buffer_ID);
	}

	void Default_Uniform_Assign_Function(const Shader* Target_Shader, const World_Object* Object, const Scene_Data* Scene)
	{
		// This assumes that the orientation vectors are already normalized
		glm::mat4 Model_Matrix = Get_Model_Matrix(Object);

		glUniformMatrix4fv(glGetUniformLocation(Target_Shader->Program_ID, "Model_Matrix"), 1, GL_FALSE, glm::value_ptr(Model_Matrix));

		glUniform1i(glGetUniformLocation(Target_Shader->Program_ID, "Albedo_Texture"), 0);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, Object->Albedo.Texture_Buffer_ID);
	}

	void Skeletal_Animation_Uniform_Assign_Function(const Shader* Target_Shader, const World_Object* Object, const Scene_Data* Scene)
	{
		Default_Uniform_Assign_Function(Target_Shader, Object, Scene); // Does all the regular stuff,

		// then also assigns skeletal animation data!!

		const Animator_Controller* Animator_Object = (Animator_Controller*)Object->Control;

		glUniformMatrix4fv(glGetUniformLocation(Target_Shader->Program_ID, "Joint_Matrices"), 
			JOINT_BUFFER_COUNT, GL_FALSE, 
			glm::value_ptr(*Animator_Object->Joint_Buffer));

		// This parses the joint buffer to the shader
	}

	void Clear_Render_Pipeline(Render_Pipeline* Target_Pipeline)
	{
		Target_Pipeline->Render_Queues.clear();
		Target_Pipeline->Queue_Table.clear();
	}

	void Push_Render_Pipeline_Queue(Render_Pipeline* Target_Pipeline, Shader Queue_Shader, void(*Shader_Init_Function)(const Shader*, const Scene_Data*), void(*Uniform_Assign_Function)(const Shader*, const World_Object*, const Scene_Data*))
	{
		Render_Queue Queue;
		Queue.Queue_Shader = Queue_Shader;
		Queue.Shader_Init_Function = Shader_Init_Function;
		Queue.Uniform_Assign_Function = Uniform_Assign_Function;

		Target_Pipeline->Queue_Table[Queue_Shader.Program_ID] = Target_Pipeline->Render_Queues.size();	// Tells engine which queue this shader corresponds to
		Target_Pipeline->Render_Queues.push_back(Queue);											// Adds queue to pipeline
	}

	void Draw_Render_Queue(const Render_Queue* Queue, const Scene_Data* Scene)
	{
		Use_Shader(Queue->Queue_Shader); // Activates shader for *this* queue

		Queue->Shader_Init_Function(&Queue->Queue_Shader, Scene); // This function sets lighting uniforms etc for this shader

		for (size_t W = 0; W < Queue->Objects.size(); W++)
		{
			Bind_Vertex_Buffer(Queue->Objects[W]->Mesh);

			Queue->Uniform_Assign_Function(&Queue->Queue_Shader, Queue->Objects[W], Scene); // Assigns object uniforms accordingly

			glDrawArrays(GL_TRIANGLES, 0, Queue->Objects[W]->Mesh.Vertex_Count);
		}
	}

	void Draw_Render_Pipeline(const Render_Pipeline* Pipeline, const Scene_Data* Scene)
	{
		for (size_t W = 0; W < Pipeline->Render_Queues.size(); W++)
			Draw_Render_Queue(&Pipeline->Render_Queues[W], Scene);
	}
}

#endif