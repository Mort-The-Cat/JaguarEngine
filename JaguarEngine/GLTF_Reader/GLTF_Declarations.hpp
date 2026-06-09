#include "../JSON_IO/JSON_Declarations.hpp"
#include "../OpenGL_Handling/OpenGL_Declarations.hpp"

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

		// This object will be used primarily for loading and converting .gltf object files into the engine
		// The model and animation data will be converted to a more OpenGL-friendly format when loaded for graphical use

		struct Node
		{
			std::string Name;
			std::vector<uint16_t> Children;	// There shouldn't ever be more than 65,535 nodes anyways
			int Mesh = -1;						// optional mesh index, default is -1
			int Skin = -1;						// an optional skin index, default is -1
			glm::mat4 Matrix = glm::mat4(1.0f);					// This node's transformation matrix
		};

		struct Mesh
		{
			std::string Name;

			std::map<std::string, Attribute> Attributes;	// These are the binaries for the attributes. Typepunning is used here to read the values themselves

			Attribute Indices;				// Index data (type-punning is used here because different datatypes can be specified)
											// index can either be GL_UNSIGNED_BYTE or GL_UNSIGNED_SHORT or GL_UNSIGNED_INT depending on the type of the bufferview
		};

		struct Skin
		{
			std::string Name;
			Attribute Inverse_Bind_Matrices;
			std::vector<uint16_t> Joints;
		};

		std::vector<Node> Nodes;
		std::vector<Mesh> Meshes;
		std::vector<Skin> Skins;

		struct Animation
		{
			enum Path_Type
			{
				Undefined = 0,
				Scale,
				Translation,
				Rotation
			};

			std::map<int, Attribute> Sampler_IO;	// All the input/outputs

			struct Sampler
			{
				int Input;				// Almost always 'time'
				int Output;				// some scale, offset, or a quarternion vector
				int Interpolation;		// 0 step, 1 linear, 2 quadratic, 3 cubic, etc
			};

			struct Channel
			{
				int Sampler;
				int Target_Node;
				Path_Type Path;				// scale, translation, or rotation
			};

			std::string Name;
		};
		
		std::vector<Animation> Animations;
	}; 

	void Load_GLTF_Object(GLTF_Object* Target_Object, JSON::JSON_Reader* Reader);

}