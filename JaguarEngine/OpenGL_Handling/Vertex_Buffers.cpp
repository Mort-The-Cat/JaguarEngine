#include "OpenGL_Declarations.hpp"
#include "../Scene/Model_Declarations.hpp"

namespace Jaguar
{
	void Bind_Vertex_Buffer(Mesh* Target_Mesh)
	{
		glBindBuffer(GL_ARRAY_BUFFER, Target_Mesh->Buffer.Vertex_Buffer);
		glBindVertexArray(Target_Mesh->Buffer.Vertex_Attribute_ID);
	}

	void Update_Vertex_Buffer_Data(Mesh* Target_Mesh)
	{
		const void* Data;
		size_t Size;
		Target_Mesh->Get_Vertices(&Data, &Size);

		glBindBuffer(GL_ARRAY_BUFFER, Target_Mesh->Buffer.Vertex_Buffer);
		glBufferData(GL_ARRAY_BUFFER, Size, Data, GL_STATIC_DRAW);	// The vertex buffers will not change
	}

	void Create_Vertex_Buffer(Mesh* Target_Mesh)
	{
		Target_Mesh->Buffer.Vertex_Count = Target_Mesh->Get_Vertex_Count();

		glGenBuffers(1, &Target_Mesh->Buffer.Vertex_Buffer);
		Update_Vertex_Buffer_Data(Target_Mesh);
	}

	void Delete_Vertex_Buffer(Mesh* Target_Mesh)
	{
		glDeleteBuffers(1, &Target_Mesh->Buffer.Vertex_Buffer);
		glDeleteVertexArrays(1, &Target_Mesh->Buffer.Vertex_Attribute_ID);

		Target_Mesh->Buffer.Vertex_Count = 0;
	}
}