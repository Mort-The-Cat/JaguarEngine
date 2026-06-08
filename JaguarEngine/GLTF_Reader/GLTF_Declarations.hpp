#include "../JSON_IO/JSON_Declarations.hpp"
#include "../OpenGL_Handling/OpenGL_Declarations.hpp"
#include "../JaguarEngine.hpp"

namespace GLTF
{
	struct GLTF_Object
	{
		// This will contain:
		// The node hierarchy from the scene
			// as well as each node's children and transformation matrices
		// The meshes
			// With each attribute and all the indices
		// The skins
			// as well as each inverse bind matrix
		// The animations

		struct Node
		{
			std::string Name;
			std::vector<uint16_t> Children;	// There shouldn't ever be more than 65,535 nodes anyways
			int Mesh = -1;						// optional mesh index, default is -1
			int Skin = -1;						// an optional skin index, default is -1
			glm::mat4 Matrix = glm::mat4(1.0f);					// This node's transformation matrix
		};

		struct Attribute
		{
			std::vector<char> Data;
			GLint Type;
		};

		struct Mesh
		{
			std::string Name;

			std::map<std::string, Attribute> Attributes;	// These are the binaries for the attributes. Typepunning is used here to read the values themselves

			Attribute Indices;				// Index data (type-punning is used here because different datatypes can be specified)
											// index can either be GL_UNSIGNED_BYTE or GL_UNSIGNED_SHORT or GL_UNSIGNED_INT depending on the type of the bufferview

			GLint Index_Type;			// type of index stored here
		};

		struct Skin
		{
			std::string Name;
			Attribute Inverse_Bind_Matrices;
			std::vector<uint16_t> Joints;
		};
	};


}