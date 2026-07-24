#include "Animation.hpp"

namespace Jaguar
{
	void Animation_Rig::Update_Joint_Recursive(glm::mat4* Joints, int Node, glm::mat4 Parent)
	{
		glm::mat4 Local;

		//if (Node_Update_Flag[Node])
		
		Local = Parent * Nodes[Node];
		
		//else
		//	Local = Rig->Nodes[Node].Matrix;	

		//Node_Update_Flag[Node] = false;

		int Joint = Rig->Nodes[Node].Joint;

		if (Joint != -1)
			Joints[Joint] = Local * Rig->Inverse_Bind_Matrices[Joint];

		//Nodes[Node] = Parent * Local;

		for (size_t Index = 0; Index < Rig->Nodes[Node].Child_Nodes.size(); Index++)
			Update_Joint_Recursive(Joints, Rig->Nodes[Node].Child_Nodes[Index], Local);
	}

	void Animation_Rig::Update_Joints(glm::mat4* Joints)
	{
		// This assume that node0 is the single root-node of the object (which could be false)

		// TODO: Find root-node(s) of the object and recursively update each of their joints

		unsigned long long Node;
		while (
			(Node = _tzcnt_u64(Updated_Node_Flag)) != 63	// trailing-zeroes = 63 when there are no more '1' bits (i.e. we've updated everything)
			)
		{
			Updated_Node_Flag ^= (1ui64 << Node);	// this clears the node that we're updating

			Nodes[Node] = Rig->Nodes[Node].Matrix;	// Sets the matrix to its default value
		}

		Update_Joint_Recursive(Joints, Rig->Root_Node, glm::mat4(1.0f));

		// Leaves every node matrix in Nodes[] in local space instead of as their global transform matrix
	}

	void Animator::Animate(Animation_Rig* Rig, float Timestep, bool Overwrite_Rig, bool Update_Rig)
	{
		// TODO: Animation blending etc 
		// i.e. allow for the rig to be overwritten or slightly modified in places
		// e.g. moving a character's head at the same time as they move the rest of their body

		// This will increment the time and get the corresponding channels

		// For each channel,
			// Gets interpolated action at certain time
			// Converts that to a matrix
			// Writes matrix to rig

		Time += Timestep;

		if (!Update_Rig)
			return;

		if (Overwrite_Rig)	// We want to flag all other nodes as NOT up-to-date
			Rig->Updated_Node_Flag = -1i64;	// sets all bits to '1', meaning they need to be updated

		for (const auto& Channel : Animation_Info->Channels)
		{
			Animation::Action Value;

			int Start = 0, End;
			for (; Start + 1 < Channel.Actions.size(); Start++)
				if (Channel.Actions[Start + 1].Time > Time)
					break;

			bool Interp = 
				Channel.Actions[0].Time <= Start &&		// If the animation has even started
				Start + 1 < Channel.Actions.size();		// and if there's an action after this to interpolate into

			if (Interp)
			{
				// interp

				Animation::Action Start_Action = Channel.Actions[Start];
				Animation::Action End_Action = Channel.Actions[Start + 1];

				float Factor = (Time - Start_Action.Time) / (End_Action.Time - Start_Action.Time);

				Value = Animation::Interpolate_Action(
					Start_Action,
					End_Action,
					Factor);
			}
			else
				Value = Channel.Actions[Start];	// Otherwise? Just use this frame

			// convert action to matrix

			Rig->Updated_Node_Flag &= ~(1ui64 << ((unsigned long long)Channel.Node)); 
			// sets this bit to 0, meaning it's been updated

			Rig->Nodes[Channel.Node] = Value.To_Matrix();

			// writes matrix to rig
		}

		return;
	}

}