#ifndef JAGUAR_UI_PIPELINE
#define JAGUAR_UI_PIPELINE

#include "UI_Handler.hpp"
#include "Default_UI.hpp"

namespace Jaguar
{
	struct UI_Data	// This includes all UI objects as well as the pipeline for rendering the UI
	{
		std::vector<UI_Element*> Root;	// These are the children of the UI 'root node'.

		std::vector<Shader> UI_Shaders;	// This tracks which pipelines are tied to the UI
	};

	void Handle_UI_Elements(JaguarEngine* Engine);	// Traverses data structure sequentially, adding elements to pipeline as well.
	
	// For UI render queues, in the init-queue function, the objects should be sorted according to the UI_Element's global 'layer' property.
	// These will be stored in the uniform buffers or otherwise.

	void Test_UI_Scene(JaguarEngine* Engine, Shader UI_Shader);
}

#endif