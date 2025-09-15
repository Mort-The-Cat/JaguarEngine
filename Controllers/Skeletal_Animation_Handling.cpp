#include "Skeletal_Animation_Handling.h"
#include "Jaguar_Engine_Wrapper.h"

namespace Jaguar
{
	
	void Animator_Controller::Control_Function(Jaguar_Engine* Engine)
	{
		// Will fill the Joint_Buffer with the keyframe matrices

		// Then transform according to bone hierarchy

		Time += Engine->Time;

		if (Time > 10.0f)
			Time = 0.0f;

		for (size_t W = 0; W < Animation->Keyframes.size(); W++)
		{
			if (Animation->Keyframes[W].size())
			{
				size_t Keyframe_Index = 0;

				while (Time > Animation->Keyframes[W][Keyframe_Index].Time && Animation->Keyframes[W].size() > Keyframe_Index + 1)
					Keyframe_Index++;

				Joint_Buffer[W] = Animation->Keyframes[W][Keyframe_Index].Transformation_Matrix;
			}
			else
				Joint_Buffer[W] = glm::mat4(1.0f); // Identity matrix for empty animation nodes
		}

		Transform_Skeletal_Animation_Joint_Buffer(Joint_Buffer, Skeleton);
	}


	void Transform_Child_Skeletal_Animation_Joint_Buffer(glm::mat4* Joint_Buffer, const Collada::Collada_Skeleton* Skeleton, size_t Index, glm::mat4 Parent_Matrix)
	{
		// Get parent transformation matrix

		// Multiply it with current Joint_Buffer matrix

		// Iterate through kids

		// When returning, apply the inverse bind matrix

		Joint_Buffer[Index] = Parent_Matrix * Joint_Buffer[Index];

		for (size_t W = 0; W < Skeleton->Joints[Index].Child_Joints.size(); W++)
			Transform_Child_Skeletal_Animation_Joint_Buffer(Joint_Buffer, Skeleton, Skeleton->Joints[Index].Child_Joints[W], Joint_Buffer[Index]);

		Joint_Buffer[Index] = Joint_Buffer[Index] * Skeleton->Joints[Index].Offset_Matrix;
	}

	void Transform_Skeletal_Animation_Joint_Buffer(glm::mat4* Joint_Buffer, const Collada::Collada_Skeleton* Skeleton)
	{
		// Recursively jumps through bone hierarchy, applying transformations

		Transform_Child_Skeletal_Animation_Joint_Buffer(Joint_Buffer, Skeleton, 0u, Skeleton->Bind_Shape_Matrix);
	}
}