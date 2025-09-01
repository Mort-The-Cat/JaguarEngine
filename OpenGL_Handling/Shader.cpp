#include "../OpenGL_Handling/OpenGL_Handling.h"

namespace Jaguar
{

	void Create_Shader(const char* Fragment, const char* Vertex, Shader* Target_Shader)
	{
		std::string Fragment_Code = Load_File_Contents(Fragment);
		std::string Vertex_Code = Load_File_Contents(Vertex);

		const char* Fragment_Code_Pointer = Fragment_Code.c_str();
		const char* Vertex_Code_Pointer = Vertex_Code.c_str();

		GLuint Fragment_Program;
		GLuint Vertex_Program;

		Fragment_Program = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(Fragment_Program, 1, &Fragment_Code_Pointer, NULL);
		glCompileShader(Fragment_Program);

		Vertex_Program = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(Vertex_Program, 1, &Vertex_Code_Pointer, NULL);
		glCompileShader(Vertex_Program);

		Target_Shader->Program_ID = glCreateProgram();
		glAttachShader(Target_Shader->Program_ID, Fragment_Program);
		glAttachShader(Target_Shader->Program_ID, Vertex_Program);
		glLinkProgram(Target_Shader->Program_ID);

		GLint Success;

		glGetProgramiv(Target_Shader->Program_ID, GL_LINK_STATUS, &Success);

		if (!Success)
		{
			char Info_Log[512];

			glGetProgramInfoLog(Target_Shader->Program_ID, 512, NULL, Info_Log);
			printf(" >> Error in shader compilation! \n\t%s\n");
		}

		glDeleteShader(Fragment_Program);
		glDeleteShader(Vertex_Program);
	}

	void Use_Shader(const Shader Target_Shader)
	{
		glUseProgram(Target_Shader.Program_ID); // Not really anything else required.
	}

	void Destroy_Shader(Shader* Target_Shader)
	{
		glDeleteShader(Target_Shader->Program_ID);

		Target_Shader->Program_ID = -1; // Sets to invalid ID
	}

}