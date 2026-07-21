#include "Skeleton.hpp"
#include "../GLTF_Reader/GLTF_Declarations.hpp"

namespace Jaguar
{

	struct Animation
	{
		// This will store a bunch of matrices and timestamps for different nodes of a skeleton

		struct Action
		{
			float Time;

			glm::vec3 Translation;
			glm::vec3 Scale;
			glm::quat Rotation;		// Quaternions aren't too intensive to interpolate

			// Doesn't currently support skew etc
		};

		struct Channel
		{
			uint16_t Node;
			std::vector<Action> Actions;
		};

		std::vector<Channel> Channels;

		std::string Name;
	};

	void Create_Animation(Animation* Target_Animation, GLTF::GLTF_Object* Object, const char* Animation_Name = nullptr);
	void Create_Animation(Animation* Target_Animation, const char* Filename, const char* Animation_Name = nullptr);

	class Animator
	{
		Animation* Animation_Info;

		float Time;
	};
}