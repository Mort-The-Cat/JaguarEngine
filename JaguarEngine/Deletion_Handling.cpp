#include "JaguarEngine.hpp"
#include "Deletion_Handling.hpp"

namespace Jaguar
{
	void Delete_All(JaguarEngine* Engine)
	{
		for (size_t Index = 0; Index < Engine->Scene.Objects.size(); Index++)
			Engine->Scene.Objects[Index]->Flags[MF_TO_BE_DELETED] = true;
	}

	void Handle_Pipeline_Deletions(JaguarEngine* Engine)
	{
		for (size_t Index = 0; Index < Engine->Pipeline.Queues.size(); Index++)
		{
			for (size_t Object = 0; Object < Engine->Pipeline.Queues[Index].Models.size(); Object++)
			{
				if (Engine->Pipeline.Queues[Index].Models[Object].Object->Flags[MF_TO_BE_DELETED])
					Engine->Pipeline.Queues[Index].Models[Object].Object = nullptr;
			}

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

	void Handle_Deletions(JaguarEngine* Engine)
	{
		// Handle deletions for World_Objects.

		Handle_Pipeline_Deletions(Engine);	// Removes those objects from the pipeline

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
}