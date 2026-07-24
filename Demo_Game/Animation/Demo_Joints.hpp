#include "../../JaguarEngine/JaguarEngine.hpp"

namespace Jaguar
{

	struct PNUV_Joint_Vertex
	{
		glm::vec3 Position;	// There is annoying padding here but we can change this later
		glm::vec3 Normal;
		glm::vec2 UV;

		uint32_t Joints;	// 4 bytes
		glm::vec4 Weights;	// 4 floats

		static constexpr int Attrib_Info[] = // This is used when initialising the vertex attributes
		{
			GL_FLOAT, 3, sizeof(Position),
			GL_FLOAT, 3, sizeof(Normal),
			GL_FLOAT, 2, sizeof(UV),
			GL_UNSIGNED_INT, 1, sizeof(Joints),
			GL_FLOAT, 4, sizeof(Weights)
		};
	};

	struct Joint_Uniforms
	{
		glm::mat4 Model_Matrix;

		glm::mat4 Joint_Matrices[32];	// Only 32 for now
	};

	Mesh* GLTF_To_Joint_Mesh(GLTF::GLTF_Object* Object, bool Init_Vertex_Buffer = true)
	{
		// This will create one big mesh, like the normal GLTF_To_Mesh function
		// but this will also store joints/weights.
	}

	// We need an animation controller etc
}