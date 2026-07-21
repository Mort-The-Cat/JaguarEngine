#include "GLTF_Declarations.hpp"

namespace GLTF
{
	GLTF_Object::Animation::Channel* GLTF_Object::Animation::Get_Channel(float Time, int Node, Path_Type Path)
	{
		Channel* Target = nullptr;
		for (size_t Index = 0; Index < Channels.size(); Index++)
		{
			if (
				Channels[Index].Target_Node == Node &&
				Channels[Index].Path == Path
				)
			{
				// verify sampler inputs

				std::vector<glm::vec1> Times = Sampler_IO[Samplers[Channels[Index].Sampler].Input].Get_Attribute_Buffer<glm::vec1>();

				glm::vec1 Min, Max;

				Jaguar::Get_Min_Max(Times, &Min, &Max);	// Get start and end times

				if (Min.x <= Time)				// If this channel has started,
				{
					Target = &Channels[Index];	// (There might be a better option, but this is the best so far)
					if (Max.x >= Time)			// If this channel also hasn't ended yet,
						break;					// This is the best option we expect to find
				}

				//if (Min.x <= Time && Max.x >= Time)		// If it satisfies these conditions, 
				//{
				//	Target = &Channels[Index];			// use this channel
				//	break;
				//}
			}
		}

		return Target;
	}

	glm::vec3 GLTF_Object::Animation::Get_Position(float Time, int Node)
	{
		return Get_Vec3(Time, Node, Path_Type::Translation, glm::vec3(0.0f));
	}

	glm::vec3 GLTF_Object::Animation::Get_Scale(float Time, int Node)
	{
		return Get_Vec3(Time, Node, Path_Type::Scale, glm::vec3(1.0f));
	}

	glm::vec3 GLTF_Object::Animation::Get_Vec3(float Time, int Node, Path_Type Path, glm::vec3 Default) // position/scale are linearly interpolated
	{
		// This will search all of the channels for one that fits the following criteria:
			// targets the same node
			// is the correct path (i.e. translation, scale, rotation)
				// if so, has inputs such that time is between the min and max spans

			// if those conditions are met? Get the corresponding samples and interpolate accordingly

		Channel* Target_Channel = Get_Channel(Time, Node, Path);

		if (!Target_Channel)
			return Default;	// No good channel? Return nothing

		std::vector<glm::vec1> Times = Sampler_IO[Samplers[Target_Channel->Sampler].Input].Get_Attribute_Buffer<glm::vec1>();
		std::vector<glm::vec3> Vectors = Sampler_IO[Samplers[Target_Channel->Sampler].Output].Get_Attribute_Buffer<glm::vec3>();

		// TODO: Implement different interpolation methods

		// Now, interpolate using 'Time' value

		int Start = 0, End;
		for (; Start + 1 < Times.size(); Start++)
			if (Times[Start + 1].x >= Time)
				break;

		End = Start + ((Start + 1) < Times.size());

		if (End - Start)
		{
			// interpolate

			float Factor = (Time - Times[Start].x) / (Times[End] - Times[Start]).x;

			return Vectors[Start] * (1.0f - Factor) + Vectors[End] * Factor;		// Linear interpolation
		}
		else
			return Vectors[Start];	// nothing to interpolate, just return value
	}


	glm::quat GLTF_Object::Animation::Get_Rotation(float Time, int Node)
	{
		// This one is a lil different because we use slerp instead of a simple scalar-style interpolation

		Channel* Target_Channel = Get_Channel(Time, Node, Path_Type::Rotation);

		if (!Target_Channel)
			return glm::quat();	// Default quaternion

		std::vector<glm::vec1> Times = Sampler_IO[Samplers[Target_Channel->Sampler].Input].Get_Attribute_Buffer<glm::vec1>();
		std::vector<glm::quat> Rotations = Sampler_IO[Samplers[Target_Channel->Sampler].Output].Get_Attribute_Buffer<glm::quat>();

		int Start = 0, End;
		for (; Start + 1 < Times.size(); Start++)
			if (Times[Start + 1].x >= Time)
				break;

		End = Start + ((Start + 1) < Times.size());

		if (End - Start)
		{
			float Factor = (Time - Times[Start].x) / (Times[End] - Times[Start]).x;

			return glm::slerp(Rotations[Start], Rotations[End], Factor);
		}
		else
			return Rotations[Start];
	}
}