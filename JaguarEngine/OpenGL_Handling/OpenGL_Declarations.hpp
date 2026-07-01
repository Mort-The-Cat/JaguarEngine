#ifndef JAGUAR_OPENGL_DECLARATIONS
#define JAGUAR_OPENGL_DECLARATIONS

#include<vector>
#include<map>

#include "../Jaguar_Config.hpp"


#include "../Libraries/Include/glad/glad.h"
#include "../Libraries/Include/GLFW/glfw3.h"
#include "../Libraries/Include/glm/glm.hpp"

#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Jaguar
{
	struct JaguarEngine;

	struct Window_Data
	{
		int Width, Height;
		GLFWwindow* Window;
	};

	struct Shader
	{
		GLuint Shader_ID;
	};

	struct Vertex_Buffer	// The data itself will be stored in a separate object
	{
		GLuint Vertex_Attribute_ID;
		GLuint Vertex_Buffer;

		uint32_t Vertex_Count;
	};

	class Mesh;

	void Bind_Vertex_Buffer(Mesh* Target_Mesh);
	void Create_Vertex_Buffer(Mesh* Target_Mesh);
	void Update_Vertex_Buffer_Data(Mesh* Target_Mesh);
	// Initialise_Vertex_Attributes<T>(Mesh*) is declared/defined in Model_Declarations.hpp due to compiler issues
	void Delete_Vertex_Buffer(Mesh* Target_Mesh);

	class Mesh_Wrapper;

	struct Render_Queue;

	void Default_Init_Queue_Uniforms_Function(JaguarEngine* Engine, Render_Queue* Queue);
	void Default_Init_Model_Uniforms_Function(JaguarEngine* Engine, Render_Queue* Queue, Mesh_Wrapper* Model);
	void Default_Render_Model_Function(JaguarEngine* Engine, Render_Queue* Queue, Mesh_Wrapper* Model);

	struct World_Object;

	struct Model_Wrapper
	{
		Mesh_Wrapper* Mesh_Wrapper;
		World_Object* Object;
	};

	struct Render_Queue	// This is a specific render queue
	{
		Shader Shader;	// The shader associated with this render queue

		// Each render-queue typically uses a separate shader but it's possible for them to overlap

		void (*Init_Queue_Uniforms_Function)(JaguarEngine*, Render_Queue*);	// an init function for the whole render queue
		void (*Init_Model_Uniforms_Function)(JaguarEngine*, Render_Queue*, Model_Wrapper);	// a per-object init function

		void (*Render_Model_Function)(JaguarEngine*, Render_Queue*, Model_Wrapper);

		std::vector<Model_Wrapper> Models;

		// This *can* support instanced rendering but it's not easily integrated hm.
		// Perhaps a single 'model object' could be an emitter of some kind
		// The emitter can hold onto the data of the instanced objects
		// From there, it uses a single draw call in the render function for the instanced drawing

		// In any case, I think I'll use a separate system to handle things like particle effects
		// because particles and traditional models are fundamentally so different

		// I think I'll also use a separate (but similar) system to handle the UI as well.
		// Since text-rendering etc is quite far-removed from models/particles
	};

	struct Render_Pipeline
	{
		// Contains all render queues

		std::vector<Render_Queue> Queues;
		std::map<const char*, size_t> Queue_Names;	
		
		// When adding a queue to the pipeline, you also give it a name that can be used later when adding objects to the scene
	};
}

#endif