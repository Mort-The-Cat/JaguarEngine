#ifndef JAGUAR_SKELETON
#define JAGUAR_SKELETON

#include "../OpenGL_Handling/OpenGL_Declarations.hpp"
#include "../GLTF_Reader/GLTF_Declarations.hpp"

namespace Jaguar
{

	struct Skeleton
	{
		struct Node
		{
			std::vector<uint16_t> Child_Nodes;

			glm::mat4 Matrix;

			int Joint;	// -1 if this node has no associated joint
		};

		std::vector<Node> Nodes;

		uint32_t Root_Node;

		//std::vector<uint16_t> Joints;	// Same as in GLTF_Declarations.hpp

		//

		std::vector<glm::mat4> Inverse_Bind_Matrices;	// Corresponds to each joint (NOT each node)
	};

	void Create_Skeleton(Skeleton* Target_Skeleton, GLTF::GLTF_Object* Object);
	void Create_Skeleton(Skeleton* Target_Skeleton, const char* Filename);
}

#endif