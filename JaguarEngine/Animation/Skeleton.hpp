#ifndef JAGUAR_SKELETON
#define JAGUAR_SKELETON

#include "../OpenGL_Handling/OpenGL_Declarations.hpp"

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

		//

		std::vector<glm::mat4> Inverse_Bind_Matrices;	// Corresponds to each joint (not each node)
	};

}

#endif