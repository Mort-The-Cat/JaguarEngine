#ifndef JAGUAR_OPENGL_DECLARATIONS
#define JAGUAR_OPENGL_DECLARATIONS

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
	glm::mat4 Get_Matrix(glm::vec3 Position, glm::vec3 Orientation, glm::vec3 Orientation_Up);
	/*
		1, 0, 0 = right
		0, 1, 0 = up
		0, 0, 1 = forwards

		as God intended
	*/

	struct JaguarEngine;

	struct Window_Data
	{
		int Width, Height;
		GLFWwindow* Window;
	};

	//class Uniform;

	class Mesh;

	struct Texture
	{
		// TODO texture
	};

	struct Mesh_Wrapper;

	struct Shader
	{
		GLuint Program_ID;

		Mesh_Wrapper* (*Create_Mesh_Wrapper)(Mesh* Mesh, const std::vector<Texture>& Textures);
	};

	template<typename Uniform>
	Mesh_Wrapper* Create_Mesh_Wrapper_(Mesh* Mesh, const std::vector<Texture>& Textures);

	template<typename Uniform>
	void Create_Shader(Shader* Shader, const char* Vertex_Shader_Filename, const char* Fragment_Shader_Filename, const char* Geometry_Shader_Filename = nullptr) //, void* (*Create_Mesh_Uniforms_Function)())
	{
		std::vector<char> Vertex_Code = Load_File_Contents(Vertex_Shader_Filename);
		std::vector<char> Fragment_Code = Load_File_Contents(Fragment_Shader_Filename);
		std::vector<char> Geometry_Code;

		const char* Fragment_Code_Pointer = Fragment_Code.data();
		const char* Geometry_Code_Pointer = nullptr;
		const char* Vertex_Code_Pointer = Vertex_Code.data();

		if (Geometry_Shader_Filename)
		{
			Geometry_Code = Load_File_Contents(Geometry_Shader_Filename);
			Geometry_Code_Pointer = Geometry_Code.data();
		}

		GLuint Fragment_Program;
		GLuint Vertex_Program;
		GLuint Geometry_Program;

		Fragment_Program = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(Fragment_Program, 1, &Fragment_Code_Pointer, NULL);
		glCompileShader(Fragment_Program);

		Vertex_Program = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(Vertex_Program, 1, &Vertex_Code_Pointer, NULL);
		glCompileShader(Vertex_Program);

		if (Geometry_Shader_Filename)
		{
			Geometry_Program = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(Geometry_Program, 1, &Geometry_Code_Pointer, NULL);
			glCompileShader(Geometry_Program);
		}

		Shader->Program_ID = glCreateProgram();
		glAttachShader(Shader->Program_ID, Fragment_Program);
		glAttachShader(Shader->Program_ID, Vertex_Program);

		if (Geometry_Shader_Filename)
			glAttachShader(Shader->Program_ID, Geometry_Program);

		glLinkProgram(Shader->Program_ID);

		// TODO: Add compile error messages here

		GLint Success;

		glGetProgramiv(Shader->Program_ID, GL_LINK_STATUS, &Success);

		if (!Success)
		{
			char Info_Log[512];
			glGetProgramInfoLog(Shader->Program_ID, sizeof(Info_Log), NULL, Info_Log);

			char Fragment_Info[512];
			char Vertex_Info[512];
			char Geometry_Info[512];

			glGetShaderInfoLog(Fragment_Program, sizeof(Fragment_Info), NULL, Fragment_Info);
			glGetShaderInfoLog(Vertex_Program, sizeof(Vertex_Info), NULL, Vertex_Info);
			glGetShaderInfoLog(Geometry_Program, sizeof(Geometry_Info), NULL, Geometry_Info);

			printf(" >> Error in shader compilation!\n\tProgram:\n\t\t%s\n\tFragment:\n\t\t%s\n\tVertex:\n\t\t%s\n\tGeometry:\n\t\t%s\n", Info_Log, Fragment_Info, Vertex_Info, Geometry_Info);


			printf(" >> %s\n\n\n\n%s\n\n\n\n%s", Vertex_Code_Pointer, Fragment_Code_Pointer, Geometry_Code_Pointer);
		}

		//

		glDeleteShader(Fragment_Program);
		glDeleteShader(Vertex_Program);

		if (Geometry_Shader_Filename)
			glDeleteShader(Geometry_Program);

		//

		Shader->Create_Mesh_Wrapper = Create_Mesh_Wrapper_<Uniform>;
	}

	//template<typename Uniform>
	//void Create_Shader(Shader* Shader, const char* Vertex_Shader_Filename, const char* Fragment_Shader_Filename, const char* Geometry_Shader_Filename = nullptr); //, void* (*Create_Mesh_Uniforms_Function)());

//#define Create_Shader(Shader, Uniform_Type, Vertex_Shader_Filename, Fragment_Shader_Filename) Create_Shader_(Shader, Jaguar::Create_Uniforms<Uniform_Type>, Vertex_Shader_Filename, Fragment_Shader_Filename)


	void Use_Shader(Shader* Shader);
	void Delete_Shader(Shader* Shader);

	struct Vertex_Buffer	// The data itself will be stored in a separate object
	{
		GLuint Vertex_Attribute_ID;
		GLuint Vertex_Buffer;

		uint32_t Vertex_Count;
	};

	void Bind_Vertex_Buffer(Mesh* Target_Mesh);
	void Create_Vertex_Buffer(Mesh* Target_Mesh);
	void Update_Vertex_Buffer_Data(Mesh* Target_Mesh);
	// Initialise_Vertex_Attributes<T>(Mesh*) is declared/defined in Model_Declarations.hpp due to template issues
	void Delete_Vertex_Buffer(Mesh* Target_Mesh);

	class Mesh_Wrapper;

	struct Render_Queue;

	void Default_Init_Queue_Uniforms_Function(JaguarEngine* Engine, Render_Queue* Queue);
	void Default_Init_Model_Uniforms_Function(JaguarEngine* Engine, Render_Queue* Queue, Mesh_Wrapper* Model);
	void Default_Render_Model_Function(JaguarEngine* Engine, Render_Queue* Queue, Mesh_Wrapper* Model);

	struct World_Object;

	struct Model_Wrapper
	{
		Mesh_Wrapper* Mesh_Wrapper;		// The mesh itself
		World_Object* Object;			// Important object information
	};

	struct Render_Queue	// This is a specific render queue
	{

		typedef void(*Queue_Function)(JaguarEngine*, Render_Queue*);
		typedef void(*Model_Function)(JaguarEngine*, Render_Queue*, Model_Wrapper);

		Shader Shader;	// The shader associated with this render queue

		// Each render-queue typically uses a separate shader but it's possible for them to overlap

		//void (*Init_Queue_Uniforms_Function)(JaguarEngine*, Render_Queue*);	// an init function for the whole render queue
		//void (*Init_Model_Uniforms_Function)(JaguarEngine*, Render_Queue*, Model_Wrapper);	// a per-object init function

		Queue_Function Init_Queue_Uniforms_Function;
		Model_Function Init_Model_Uniforms_Function;
		Model_Function Render_Model_Function;

		//void (*Render_Model_Function)(JaguarEngine*, Render_Queue*, Model_Wrapper);

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

		//std::map<const char*, size_t> Queue_Names;
		
		std::map<GLuint, size_t> Queue_Table;

		// TODO: Store/track the shaders somewhere here or in a similar struct? Refer to shaders using single name perhaps
		
		// When adding a queue to the pipeline, you also give it a name that can be used later when adding objects to the scene
	};

	void Push_Render_Pipeline_Queue(
		JaguarEngine* Engine,
		Shader Shader,
		Render_Queue::Queue_Function Init_Queue_Function,
		Render_Queue::Model_Function Init_Model_Function,
		Render_Queue::Model_Function Model_Render_Function
	);

	void Draw_Render_Queue(JaguarEngine* Engine, Render_Queue* Queue);
	void Draw_Render_Pipeline(JaguarEngine* Engine);
}

#endif