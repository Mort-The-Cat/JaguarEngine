#include "../OpenGL_Handling/OpenGL_Handling.h"

namespace Jaguar
{

	void Create_Shader(const char* Fragment, const char* Vertex, Shader* Target_Shader, const char* Geometry)
	{
		std::string Fragment_Code = Load_File_Contents(Fragment);
		std::string Vertex_Code = Load_File_Contents(Vertex);
		std::string Geometry_Code;

		if (Geometry)
			Geometry_Code = Load_File_Contents(Geometry);

		const char* Fragment_Code_Pointer = Fragment_Code.c_str();
		const char* Vertex_Code_Pointer = Vertex_Code.c_str();
		const char* Geometry_Code_Pointer;

		if (Geometry)
			Geometry_Code_Pointer = Geometry_Code.c_str();

		GLuint Fragment_Program;
		GLuint Vertex_Program;
		GLuint Geometry_Program;

		Fragment_Program = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(Fragment_Program, 1, &Fragment_Code_Pointer, NULL);
		glCompileShader(Fragment_Program);

		Vertex_Program = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(Vertex_Program, 1, &Vertex_Code_Pointer, NULL);
		glCompileShader(Vertex_Program);

		if (Geometry)
		{
			Geometry_Program = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(Geometry_Program, 1, &Geometry_Code_Pointer, NULL);
			glCompileShader(Geometry_Program);
		}

		Target_Shader->Program_ID = glCreateProgram();
		glAttachShader(Target_Shader->Program_ID, Fragment_Program);
		glAttachShader(Target_Shader->Program_ID, Vertex_Program);

		if (Geometry)
			glAttachShader(Target_Shader->Program_ID, Geometry_Program);

		glLinkProgram(Target_Shader->Program_ID);

		GLint Success;

		glGetProgramiv(Target_Shader->Program_ID, GL_LINK_STATUS, &Success);

		if (!Success)
		{
			char Info_Log[512];

			glGetProgramInfoLog(Target_Shader->Program_ID, 512, NULL, Info_Log);

			char Fragment_Info[512];
			char Vertex_Info[512];
			char Geometry_Info[512];

			glGetShaderInfoLog(Fragment_Program, 512, NULL, Fragment_Info);
			glGetShaderInfoLog(Vertex_Program, 512, NULL, Vertex_Info);
			glGetShaderInfoLog(Geometry_Program, 512, NULL, Geometry_Info);
			printf(" >> Error in shader compilation! \n\t%s\n\n%s\n\n%s\n\n%s\n", Info_Log, Fragment_Info, Vertex_Info, Geometry_Info);
		}

		glDeleteShader(Fragment_Program);
		glDeleteShader(Vertex_Program);

		if(Geometry)
			glDeleteShader(Geometry_Program);
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