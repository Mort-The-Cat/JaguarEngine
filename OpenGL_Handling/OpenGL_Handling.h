#ifndef JAGUAR_OPENGL_HANDLING
#define JAGUAR_OPENGL_HANDLING

#define CRT_SECURE_NO_WARNINGS 1

#include "../Collada_Loader/Collada_Loader.h"

namespace Jaguar
{

	class Shader
	{
	public:
		GLuint Program_ID;
	};

	/*template<typename Vertex_Attributes, typename Uniform_Attributes>
	class Shader_Type : public Shader
	{

	};*/

	void Create_Shader(const char* Fragment, const char* Vertex, Shader* Target_Shader);
	void Destroy_Shader(Shader* Target_Shader);
	void Use_Shader(const Shader Target_Shader);

	void Throw_Error(const char* Formatted_String);

	// For now, I'll just do a test shader to see if I've got things loading correctly

	struct Vertex_Buffer
	{
		//const void* Data;
		//size_t Size;

		// This means that another object/wrapper needs to keep track of the allocated memory. (This just uses a reference to it)

		GLuint Vertex_Buffer_ID;
		GLuint Vertex_Attribute_ID;

		size_t Vertex_Count;
	};

	void Create_Vertex_Buffer(const Collada::Collada_Mesh& Mesh, Vertex_Buffer* Target_Buffer);
	void Bind_Vertex_Buffer(const Vertex_Buffer& Buffer);
	void Destroy_Vertex_Buffer(Vertex_Buffer* Target_Buffer);

}

#endif