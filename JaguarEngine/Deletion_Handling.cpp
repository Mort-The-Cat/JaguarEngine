#include "JaguarEngine.hpp"
#include "Deletion_Handling.hpp"

namespace Jaguar
{
	void Handle_UI_Deletions_Helper(std::vector<UI_Element*>& Elements)
	{
		for (size_t Index = 0; Index < Elements.size(); Index++)
			if (Elements[Index]->Flags[UF_TO_BE_DELETED])
				Delete_UI_Element(Elements.data() + Index);				// This can be deleted - no extra work required
			else
				Handle_UI_Deletions_Helper(Elements[Index]->Children);	// Continue to check the children of this node

		Elements.erase(
			std::remove(
				Elements.begin(),
				Elements.end(),
				nullptr				// removes all elements which have a 'null' reference set (i.e. have been deleted)
			),
			Elements.end()
		);
	}

	void Handle_UI_Deletions(JaguarEngine* Engine)
	{
		Handle_UI_Deletions_Helper(Engine->UI.Root);
	}

	//

	void Delete_All(JaguarEngine* Engine)
	{
		for (size_t Index = 0; Index < Engine->Scene.Objects.size(); Index++)
			Engine->Scene.Objects[Index]->Flags[MF_TO_BE_DELETED] = true;
	}

	inline void Handle_Queue_Object_Deletions(Render_Queue* Queue)
	{
		for (size_t Object = 0; Object < Queue->Models.size(); Object++)
		{
			if (Queue->Models[Object].Object->Flags[MF_TO_BE_DELETED])
				Queue->Models[Object].Object = nullptr;
		}
	}

	void Handle_Pipeline_Deletions(JaguarEngine* Engine)
	{
		for (size_t Index = 0; Index < Engine->Pipeline.Queues.size(); Index++)
		{
			if(Engine->Pipeline.Queues[Index].Type == Render_Queue::Model_Type::OBJECT) // UI pipeline is handled differently
			{
				Handle_Queue_Object_Deletions(&Engine->Pipeline.Queues[Index]);

				Engine->Pipeline.Queues[Index].Models.erase(
					std::remove(
						Engine->Pipeline.Queues[Index].Models.begin(),
						Engine->Pipeline.Queues[Index].Models.end(),
						Model_Wrapper()									// search for an empty model wrapper
					),
					Engine->Pipeline.Queues[Index].Models.end()
				);
			}
		}
	}

	void Handle_Scene_Deletions(JaguarEngine* Engine)
	{
		for (size_t Index = 0; Index < Engine->Scene.Objects.size(); Index++)
		{
			if (Engine->Scene.Objects[Index]->Flags[MF_TO_BE_DELETED])
			{
				Delete_World_Object(Engine->Scene.Objects[Index]);				// Deallocates the objects
				Engine->Scene.Objects[Index] = nullptr;
			}
		}

		Engine->Scene.Objects.erase(				// Removes what's left from the scene
			std::remove(
				Engine->Scene.Objects.begin(),
				Engine->Scene.Objects.end(),
				nullptr
			),
			Engine->Scene.Objects.end()
		);
	}

	void Handle_Deletions(JaguarEngine* Engine)
	{
		// Handle deletions for World_Objects.

		Handle_Pipeline_Deletions(Engine);	// Removes those objects from the pipeline

		Handle_Scene_Deletions(Engine);

		Handle_UI_Deletions(Engine);
	}
}