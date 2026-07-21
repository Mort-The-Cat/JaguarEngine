#include "Animation.hpp"

#include<set>

namespace Jaguar
{
	void Get_Timestamps(GLTF::GLTF_Object::Animation* Target_Animation, std::set<float>* Timestamps, int Node)
	{
		std::set<int> Inputs;

		for (size_t Index = 0; Index < Target_Animation->Channels.size(); Index++)
			if(Target_Animation->Channels[Index].Target_Node == Node)
				Inputs.insert(Target_Animation->Samplers[Target_Animation->Channels[Index].Sampler].Input);

		for (const auto& Input : Inputs)
		{
			std::vector<glm::vec1> Input_Times = Target_Animation->Sampler_IO[Input].Get_Attribute_Buffer<glm::vec1>();

			for (size_t Index = 0; Index < Input_Times.size(); Index++)
				Timestamps->insert(Input_Times[Index].x);
		}

		// write 'times' to timestamps
	}

	void Create_Animation(Animation* Target_Animation, const char* Filename, const char* Animation_Name)
	{
		// load the GLTF_Object and use it

		JSON::JSON_Reader Reader;
		GLTF::GLTF_Object Object;

		JSON::Load_JSON_Object(&Reader.Object, Filename);
		JSON::Load_JSON_Reader_Buffers(&Reader);

		GLTF::Load_GLTF_Object(&Object, &Reader);

		Create_Animation(Target_Animation, &Object, Animation_Name);
	}

	void Create_Animation(Animation* Target_Animation, GLTF::GLTF_Object* Object, const char* Animation_Name)
	{
		GLTF::GLTF_Object::Animation* GLTF_Anim = nullptr;

		if (Animation_Name)
		{
			for (size_t Index = 0; Index < Object->Animations.size(); Index++)
				if (
					strcmp(Animation_Name, Object->Animations[Index].Name.c_str()) == 0
					)
				{
					GLTF_Anim = &Object->Animations[Index];
					break;
				}
		}
		else if (Object->Animations.size())
			GLTF_Anim = &Object->Animations[0];

		if (!GLTF_Anim)
		{
			printf(" >> %s animation not found!\n", Animation_Name);
			return;
		}

		// Start producing animation

		std::set<int> Target_Nodes;
		for (size_t Index = 0; Index < GLTF_Anim->Channels.size(); Index++)
			Target_Nodes.insert(GLTF_Anim->Channels[Index].Target_Node);

		for (const auto& Node : Target_Nodes)
		{
			// for each referenced node? add channels

			std::set<float> Timestamps;

			Get_Timestamps(GLTF_Anim, &Timestamps, Node);

			// Gets all the time stamps

			// start generating the actions for this animation

			Animation::Channel Node_Channel;

			Node_Channel.Node = Node;

			for (const float& Time : Timestamps)
			{
				Animation::Action New_Action;

				New_Action.Time = Time;
				New_Action.Translation = GLTF_Anim->Get_Position(Time, Node);
				New_Action.Scale = GLTF_Anim->Get_Scale(Time, Node);
				New_Action.Rotation = GLTF_Anim->Get_Rotation(Time, Node);

				Node_Channel.Actions.push_back(New_Action);
			}

			Target_Animation->Channels.push_back(Node_Channel);
		}
	}

}