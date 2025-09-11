#ifndef JAGUAR_SKELETAL_ANIMATION_HANDLING
#define JAGUAR_SKELETAL_ANIMATION_HANDLING

#include "../Collada_Loader/Collada_Loader.h"
#include "../OpenGL_Handling/Scene.h"

namespace Jaguar
{

#define JOINT_BUFFER_COUNT 64

	struct Jaguar_Engine;
	class Shader;

	void Skeletal_Animation_Uniform_Assign_Function(const Shader* Target_Shader, const World_Object* Object, const Scene_Data* Scene);

	void Transform_Child_Skeletal_Animation_Joint_Buffer(glm::mat4* Joint_Buffer, const Collada::Collada_Skeleton* Skeleton, size_t Index, size_t Parent_Index);
	void Transform_Skeletal_Animation_Joint_Buffer(glm::mat4* Joint_Buffer, const Collada::Collada_Skeleton* Skeleton);

	class Animator_Controller : public Controller
	{
	public:
		glm::mat4 Joint_Buffer[JOINT_BUFFER_COUNT];	// This is the buffer of all the joints' matrix transformations 
		// that will be parsed to the GPU

		Collada::Collada_Animation* Animation;	// The animation we're playing
		Collada::Collada_Skeleton* Skeleton;	// The skeleton for the mesh we're using

		float Time = 0.0f; // This is the current time in seconds of the animation

		Animator_Controller(World_Object* Objectp, Collada::Collada_Animation* Animationp, Collada::Collada_Skeleton* Skeletonp)
		{
			Object = Objectp;
			Animation = Animationp;
			Skeleton = Skeletonp;
		}

		void Control_Function(Jaguar_Engine* Engine) override;
	};
}

#endif