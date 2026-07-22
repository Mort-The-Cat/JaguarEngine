#ifndef JAGUAR_ANIMATION
#define JAGUAR_ANIMATION

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

			glm::mat4 To_Matrix()
			{
				return 
					glm::translate(
						glm::scale(
							glm::toMat4(Rotation), 
							Scale
						), 
						Translation
					);
			}
		};

		static Action Interpolate_Action(Action A, Action B, float Factor)
		{
			Action C;
			C.Translation = A.Translation * (1.0f - Factor) + B.Translation * Factor;
			C.Scale = A.Scale * (1.0f - Factor) + B.Scale * Factor;
			C.Rotation = glm::slerp(A.Rotation, B.Rotation, Factor);

			return C;
		}

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

	struct Animation_Rig	// This is the object that is animated
	{
	private:
		void Update_Joint_Recursive(glm::mat4* Joints, int Node, glm::mat4 Parent);
	public:
		const Skeleton* Rig;	// stored in asset-cache


		std::vector<glm::mat4> Nodes;	// This is required to track the matrices of each node so they can be converted to joints
										// with the current design, these are all local to the node's parent node-space

		void Update_Joints(glm::mat4* Joints);
	};

	class Animator			// This object controls and applies an animation to an animation-rig
	{
	public:
		const Animation* Animation_Info;	// stored in asset-cache

		float Time;

		// This will be expanded on later to allow for different properties

		void Animate(Animation_Rig* Rig, float Timestep, bool Update_Rig = true);
	};

	// A controller for an object with animations will contain 
		// 1 animation rig object, 
		// and an animator object for every animation that object uses 
			// (or a single-one if they're all packed somehow into a single track)
}

#endif