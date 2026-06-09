#ifndef JAGUAR_OPENGL_DECLARATIONS
#define JAGUAR_OPENGL_DECLARATIONS

#include<vector>

#include "../Libraries/Include/glm/glm.hpp"

#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "../Libraries/Include/glad/glad.h"
#include "../Libraries/Include/GLFW/glfw3.h"

namespace Jaguar
{

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

	struct JaguarEngine;
}

#endif