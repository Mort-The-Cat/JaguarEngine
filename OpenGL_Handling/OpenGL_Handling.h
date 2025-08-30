#ifndef JAGUAR_OPENGL_HANDLING
#define JAGUAR_OPENGL_HANDLING

#include "../Collada_Loader/Collada_Loader.h"

void Throw_Error(const char* Formatted_String)
{
	fprintf(stderr, Formatted_String, strerror(errno));
}

// For now, I'll just do a test shader to see if I've got things loading correctly

struct Shader
{
	GLuint Program_ID;
};

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
		char Info_Log [512];

		glGetProgramInfoLog(Target_Shader->Program_ID, 512, NULL, Info_Log);
		printf(" >> Error in shader compilation! \n\t%s\n");
	}
}

void Destroy_Shader(Shader* Target_Shader)
{

}

struct Vertex_Buffer
{
	const void* Data;
	size_t Size;

	GLuint Vertex_Buffer_ID;
	GLuint Vertex_Attribute_ID;
};

void Create_Vertex_Buffer(const Collada::Collada_Mesh& Mesh, Vertex_Buffer* Target_Buffer)
{
	Target_Buffer->Data = Mesh.Vertices.data();
	Target_Buffer->Size = Mesh.Vertices.size() * sizeof(Collada::Collada_Vertex);

	// Assigns data and size

	glGenBuffers(1, &Target_Buffer->Vertex_Buffer_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Target_Buffer->Vertex_Buffer_ID);
	glBufferData(GL_ARRAY_BUFFER, Target_Buffer->Size, Target_Buffer->Data, GL_STATIC_DRAW); // The vertex buffer won't be updated once we generate it

	// Handle attribute buffer

	glGenVertexArrays(1, &Target_Buffer->Vertex_Attribute_ID);
	glBindVertexArray(Target_Buffer->Vertex_Attribute_ID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Collada::Collada_Vertex), (void*)0); // Position vectors
		glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Collada::Collada_Vertex), (void*)(3 * 4)); // Normal vectors
		glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Collada::Collada_Vertex), (void*)(6 * 4)); // Texture coordinates
		glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Collada::Collada_Vertex), (void*)(8 * 4)); // Padding
		glEnableVertexAttribArray(3);
}

void Destroy_Vertex_Buffer(Vertex_Buffer* Target_Buffer)
{

}

void Test_Graphics_Loop()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	GLFWwindow* Window;

	Window = glfwCreateWindow(800, 400, "Test window!", NULL, NULL);

	if (Window == NULL)
		Throw_Error(" >> Failed to create OpenGL window!\n\t%s\n");

	glfwMakeContextCurrent(Window);

	if (!gladLoadGL())
		Throw_Error(" >> Failed to initialise glad!\n\t%s\n");

	//

	Collada::XML_Document Document;
	Collada::Load_XML_Document("Collada_Loader/untitled.dae", &Document);

	Collada::Collada_Mesh Mesh;
	Collada::Load_Mesh(Document, &Mesh);

	Vertex_Buffer Buffer;
	Create_Vertex_Buffer(Mesh, &Buffer);

	Shader Test_Shader;
	Create_Shader("Shaders/Test_Shader.frag", "Shaders/Test_Shader.vert", &Test_Shader);
	glUseProgram(Test_Shader.Program_ID);

	//

	//

	glViewport(0, 0, 800, 400);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	while (!glfwWindowShouldClose(Window))
	{
		// Basic graphics loop

		glClearColor(0.3, 0.3, 0.2, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		glDrawArrays(GL_TRIANGLES, 0, Buffer.Size / (sizeof(Collada::Collada_Vertex)));

		glfwSwapBuffers(Window);
		glfwPollEvents();
	}

	glfwTerminate();
}

#endif