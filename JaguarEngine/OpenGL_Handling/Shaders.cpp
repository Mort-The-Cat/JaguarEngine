#include "../JaguarEngine.hpp"

namespace Jaguar
{
	void Use_Shader(Shader* Shader)
	{
		glUseProgram(Shader->Program_ID);
	}


	void Delete_Shader(Shader* Shader)
	{
		glDeleteShader(Shader->Program_ID);

		Shader->Program_ID = -1;
	}
}