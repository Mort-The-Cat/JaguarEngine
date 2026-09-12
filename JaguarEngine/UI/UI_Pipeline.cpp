#include "../JaguarEngine.hpp"
#include "UI_Pipeline.hpp"

namespace Jaguar
{
	// Need a recursive function to handle ui elements sequentially

	void Handle_UI_Elements(JaguarEngine* Engine)
	{
		for (size_t Shader = 0; Shader < Engine->UI.UI_Shaders.size(); Shader++)
			Engine->Pipeline.Queues[Engine->Pipeline.Queue_Table[Engine->UI.UI_Shaders[Shader].Program_ID]].Models.clear();	
		
		// Clears all elements from pipeline
		// Because we're about to repopulate this

		for (size_t Index = 0; Index < Engine->UI.Root.size(); Index++)
		{
			UI_Transform Viewport = {
				.Origin = glm::vec2(-1.0f),
				.Scale = glm::vec2(1.0f),
				.Right = glm::vec2(1.0f, 0.0f),
				.Down = glm::vec2(0.0f, 1.0f),
				.Layer = 0.0f
			};

			UI_Parent_Layout Adam = {
				.Min = glm::vec2(0.0f),
				.Max = glm::vec2(1.0f),
				.Lay = glm::vec2(0.0f)
			};

			// Uses the same Adam transform for all of them

			// Run recursive function to update UI and add to render queues
		}
	}

	//

	void Test_UI_Scene(JaguarEngine* Engine, Shader UI_Shader)
	{
		// This will create a bunch of test UI elements

		Mesh* Default = Pull_Mesh(Engine, GLTF_To_UI, "JaguarEngine/UI/Default_Element.gltf").Mesh;

		Engine->UI.Root = 
		{
			new UI_Element 
			{
				.Left = { 0.20f, Layout::Margin_In },
				.Right = { 0.60f, Layout::Size },
				.Top = { 0.2f, Layout::Margin_In },
				.Bottom = { 0.4, Layout::Size },

				.Colour = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),

				.Shader = UI_Shader,

				.Mesh = UI_Shader.Create_Mesh_Wrapper(Default, {}),	// Each element needs its own wrapper/shader (unless it's hidden)

				.Children = 
				{
					new UI_Element
					{
						.Left = { 0.025f, Layout::Lay_Primary },
						.Right = { 1.0f, Layout::Fill_Parent },

						.Colour = glm::vec4(1.0f, 0.75f, 0.75f, 1.0f),

						.Shader = UI_Shader,
						.Mesh = UI_Shader.Create_Mesh_Wrapper(Default, {})
					},

					new UI_Element
					{
						.Left = { 0.025f, Layout::Lay_Primary },
						.Right = { 1.0f, Layout::Fill_Parent },

						.Colour = glm::vec4(0.75f, 1.0f, 0.75f, 1.0f),

						.Shader = UI_Shader,
						.Mesh = UI_Shader.Create_Mesh_Wrapper(Default, {})
					},

					new UI_Element
					{
						.Left = { 0.025f, Layout::Lay_Primary },
						.Right = { 1.0f, Layout::Fill_Parent },

						.Colour = glm::vec4(0.75f, 0.75f, 1.0f, 1.0f),

						.Shader = UI_Shader,
						.Mesh = UI_Shader.Create_Mesh_Wrapper(Default, {})
					}
				}
			}
		};
	}
}