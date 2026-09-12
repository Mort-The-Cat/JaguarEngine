#ifndef JAGUAR_DEFAULT_UI
#define JAGUAR_DEFAULT_UI

#include "../OpenGL_Handling/OpenGL_Declarations.hpp"
#include "../GLTF_Reader/GLTF_Declarations.hpp"

struct Demo_UI_Uniform
{
	glm::vec4 Colour;
	glm::mat3 Transform;	// Lots of annoying padding- 
	// not a huge problem but look into changing somehow
	float Layer;
};

struct UI_Vertex
{
	glm::vec2 Position, UV;

	static constexpr int Attribute_Info[] =
	{
		GL_FLOAT, 2, sizeof(Position),
		GL_FLOAT, 2, sizeof(UV)
	};
};
Jaguar::Mesh* GLTF_To_UI(GLTF::GLTF_Object* Object, bool Init_Vertex_Buffer = true);

//

void Demo_UI_Init_Queue(Jaguar::JaguarEngine* Engine, Jaguar::Render_Queue* Queue);
void Demo_UI_Init_Model(Jaguar::JaguarEngine* Engine, Jaguar::Render_Queue* Queue, Jaguar::Model_Wrapper Wrapper);
void Demo_UI_Render_Model(Jaguar::JaguarEngine* Engine, Jaguar::Render_Queue* Queue, Jaguar::Model_Wrapper Wrapper);

#endif