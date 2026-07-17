#include "OpenGL_Declarations.hpp"
#include "../JaguarEngine.hpp"

namespace Jaguar
{
	void Draw_Render_Queue(JaguarEngine* Engine, Render_Queue* Queue)
	{
		if(Queue->Init_Queue_Uniforms_Function)
			Queue->Init_Queue_Uniforms_Function(Engine, Queue);

		for (size_t Index = 0; Index < Queue->Models.size(); Index++)	// Since render queues are so modular and user-defined, this function is really simple
		{
			if(Queue->Init_Model_Uniforms_Function)
				Queue->Init_Model_Uniforms_Function(Engine, Queue, Queue->Models[Index]);

			Queue->Render_Model_Function(Engine, Queue, Queue->Models[Index]);	// The render pipeline needs a render function of some sort. The other two aren't *required*
		}
	}


	void Draw_Render_Pipeline(JaguarEngine* Engine)
	{
		for (size_t Index = 0; Index < Engine->Pipeline.Queues.size(); Index++)
			Draw_Render_Queue(Engine, &Engine->Pipeline.Queues[Index]);
	}

	void Push_Render_Pipeline_Queue(
		JaguarEngine* Engine,
		Shader Shader,
		Render_Queue::Queue_Function Init_Queue_Function,
		Render_Queue::Model_Function Init_Model_Function,
		Render_Queue::Model_Function Render_Model_Function
	)
	{
		Render_Queue New_Queue;
		New_Queue.Shader = Shader;
		New_Queue.Init_Queue_Uniforms_Function = Init_Queue_Function;
		New_Queue.Init_Model_Uniforms_Function = Init_Model_Function;
		New_Queue.Render_Model_Function = Render_Model_Function;
		
		Engine->Pipeline.Queue_Table[Shader.Program_ID] = Engine->Pipeline.Queues.size();
		Engine->Pipeline.Queues.push_back(New_Queue);
	}
}