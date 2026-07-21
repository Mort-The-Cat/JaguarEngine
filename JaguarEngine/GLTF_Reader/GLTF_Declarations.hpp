#ifndef GLTF_DECLARATIONS
#define GLTF_DECLARATIONS

#include "../Jaguar_Config.hpp"

#include "../JSON_IO/JSON_Declarations.hpp"
#include "../OpenGL_Handling/OpenGL_Declarations.hpp"

namespace Jaguar
{
	template<typename T>
	void Get_Min_Max(const std::vector<T>& Array, T* Min, T* Max)
	{
		if (Array.size() == 0)
			return;

		*Min = Array[0];
		*Max = Array[0];

		for (size_t Index = 1; Index < Array.size(); Index++)
		{
			*Min = glm::min(*Min, Array[Index]);
			*Max = glm::max(*Max, Array[Index]);
		}
	}
}

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
		// The format which the object will be stored in will be user-defined and dependent on the shader.
		// Note that a world-object could have more than 1 mesh / shader associated with it.

		struct Node
		{
			std::string Name;
			std::vector<uint16_t> Children;		// There shouldn't ever be more than 65,535 nodes anyways
			int Mesh = -1;						// optional mesh index, default is -1
			int Skin = -1;						// an optional skin index, default is -1
				// having a skin just means that the corresponding mesh 
				// uses the inverse-bind-matrices and joint-indices of THIS specific skin
			glm::mat4 Matrix = glm::mat4(1.0f);	// This node's transformation matrix
		};

		struct Mesh
		{
			std::string Name;

			std::map<std::string, Attribute> Attributes;	// These are the binaries for the attributes. 'Attributes' can be easily converted to any desired glm type vector

			Attribute Indices;
		};

		struct Skin
		{
			std::string Name;
			Attribute Inverse_Bind_Matrices;
			std::vector<uint16_t> Joints;		// Corresponds to each node
		};

		struct Animation
		{
			enum Path_Type
			{
				Undefined = 0,
				Scale,				// uses a vec3 typically
				Translation,		// uses a vec3 typically
				Rotation			// uses a quat typically (i.e. vec4)

				//					These will be ideally converted into a matrix for skeletal animation
			};

			std::map<int, Attribute> Sampler_IO;	// All the input/output values

			struct Sampler
			{
				int Input;				// Almost always 'time'
				int Output;				// some scale, offset, or a quarternion vector
				int Interpolation;		// 0 step, 1 linear, 2 quadratic, 3 cubic, etc
			};

			std::vector<Sampler> Samplers;	// sampler wrappers

			struct Channel
			{
				int Sampler;
				int Target_Node;
				Path_Type Path;				// scale, translation, or rotation
			};

			std::vector<Channel> Channels;	// channels

			std::string Name;

			glm::vec3 Get_Position(float Time, int Node);
			glm::vec3 Get_Scale(float Time, int Node);
			glm::quat Get_Rotation(float Time, int Node);

		private:
			Channel* Get_Channel(float Time, int Node, Path_Type Path);
			glm::vec3 Get_Vec3(float Time, int Node, Path_Type Path, glm::vec3 Default);
		};

		std::string Name;

		size_t Parent_Node;

		std::vector<Node> Nodes;
		std::vector<Mesh> Meshes;
		std::vector<Skin> Skins;
		std::vector<Animation> Animations;
	}; 

	void Load_GLTF_Object(GLTF_Object* Target_Object, JSON::JSON_Reader* Reader);

}

#endif