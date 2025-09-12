#include "Skeletal_Animation_Handling.h"

namespace Jaguar
{
	
	void Animator_Controller::Control_Function(Jaguar_Engine* Engine)
	{
		// Will fill the Joint_Buffer with the keyframe matrices

		// Then transform according to bone hierarchy

		Time += 0.02f;

		if (Time > 200.0f)
			Time = 0.0f;

		for (size_t W = 0; W < Animation->Keyframes.size(); W++)
		{
			if (Animation->Keyframes[W].size())
			{
				size_t Keyframe_Index = 0;
				while (Animation->Keyframes[W][Keyframe_Index].Time < Time && Animation->Keyframes[W].size() > Keyframe_Index + 1)
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

		// Joint_Buffer[Index][3].z *= -1.0f;

		Joint_Buffer[Index] = Joint_Buffer[Index] * Skeleton->Joints[Index].Offset_Matrix;

		Joint_Buffer[Index] = Skeleton->Bind_Shape_Matrix * Joint_Buffer[Index];

		//Joint_Buffer[Index] = Skeleton->Joints[Index].Offset_Matrix;

		// Joint_Buffer[Index] = glm::rotate(3.14159f / 2.0f, glm::vec3(-1.0f, 0.0f, 0.0f)) * Joint_Buffer[Index];

		/*std::swap(Joint_Buffer[Index][1][0], Joint_Buffer[Index][2][0]);
		std::swap(Joint_Buffer[Index][1][1], Joint_Buffer[Index][2][1]);
		std::swap(Joint_Buffer[Index][1][2], Joint_Buffer[Index][2][2]);

		std::swap(Joint_Buffer[Index][1][3], Joint_Buffer[Index][2][3]);

		Joint_Buffer[Index][1] *= glm::vec4(-1);*/

		// Will need to tweak these matrices
	}

	void Transform_Skeletal_Animation_Joint_Buffer(glm::mat4* Joint_Buffer, const Collada::Collada_Skeleton* Skeleton)
	{
		// Recursively jumps through bone hierarchy, applying transformations

		Transform_Child_Skeletal_Animation_Joint_Buffer(Joint_Buffer, Skeleton, 0u, glm::mat4(1.0f));

		//for (size_t W = 0; W < Skeleton->Joints[0].Child_Joints.size(); W++)
		//{		}
	}
}