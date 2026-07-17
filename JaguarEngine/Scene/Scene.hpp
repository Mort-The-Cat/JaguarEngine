#include "../Jaguar_Config.hpp"
#include "../OpenGL_Handling/OpenGL_Declarations.hpp"


namespace Jaguar
{
	struct Camera_Data	// Might get rid of this later, but it's nice to have it defined
	{
		glm::vec3 Position;
		glm::vec3 Orientation;
		glm::vec3 Orientation_Up;

		float FOV;
		float Aspect;	// Update these values later

		glm::mat4 Matrix;
	};

	struct Scene_Data
	{
		Camera_Data Camera;	// This is the main camera currently used (perhaps I'll change this later)

		std::vector<World_Object*> Scene_Objects;	// All the objects currently in the scene

		// Will also add collision/physics structures here too

		// perhaps also add lighting structures here? We'll see
	};

}