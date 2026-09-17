#include "../JaguarEngine.hpp"
#include "UI_Pipeline.hpp"

namespace Jaguar
{
	// Need a recursive function to handle ui elements sequentially

	template<const bool Axis = false, const bool Side = false>
	void Evaluate_Axis(Layout* First, Layout* Next, const UI_Transform* Parent_Transform, UI_Parent_Layout* Parent_Layout, float* A, float* NextB_Value)
	{
		// A is min
		// B is max

		if (First == nullptr)	// Prevents infinite recursion
			return;

		float Size;

		switch (First->Type)
		{
			case Layout::Percentage:
				*A =
					Parent_Transform->Origin[Axis] * (1.0f - First->Value) +
					Parent_Transform->End[Axis] * First->Value;
				Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A);
				break;

			case Layout::Margin_Out:
				if constexpr (Side)
					*A =
						Parent_Transform->End + First->Value;
				else
					*A =
						Parent_Transform->Origin - First->Value;
				Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A);
				break;

			case Layout::Margin_In:
				if constexpr (Side)
					*A =
						Parent_Transform->End - First->Value;
				else
					*A =
						Parent_Transform->Origin + First->Value;

				Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A);
				break;

			//

			case Layout::Fill_Parent:
				Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A);
				if constexpr (Side)
					*A = 
					(First->Value * Parent_Layout->Inverse_Fill_Factor)				// the fraction of the space this element takes up
					* (Parent_Transform[Axis].End - Parent_Transform[Axis].Origin)	// the space itself
					+ *B;															// the previous side
				else
					*A =
					(First->Value * Parent_Layout->Inverse_Fill_Factor)				// the fraction of the space this element takes up
					* (Parent_Transform[Axis].Origin - Parent_Transform[Axis].End)	// the space itself
					+ *B;															// the previous side
				break;

			case Layout::Size:
				Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A);
				if constexpr (Side)
					*A = *B + First->Value;
				else
					*A = *B - First->Value;
				break;

			// TODO: Lay Primary / Lay Secondary
		}

		//Evaluate_Axis(Next, nullptr, Parent_Transform, Parent_Layout, Next_Value, First_Value);
	}

	void Recurse_UI_Element(JaguarEngine* Engine, UI_Element* Element, const UI_Transform* Parent_Transform, UI_Parent_Layout* Parent_Layout)
	{
		// calculates this element's transform 
		// updates parent layout if appropriate

		// adds to render pipeline if appropriate


	}

	void Handle_UI_Elements(JaguarEngine* Engine)
	{
		for (size_t Shader = 0; Shader < Engine->UI.UI_Shaders.size(); Shader++)
			Engine->Pipeline.Queues[Engine->Pipeline.Queue_Table[Engine->UI.UI_Shaders[Shader].Program_ID]].Models.clear();	
		
		// Clears all elements from pipeline
		// Because we're about to repopulate them

		for (size_t Index = 0; Index < Engine->UI.Root.size(); Index++)
		{
			UI_Transform Viewport = {
				.Origin = glm::vec2(-1.0f),
				.End = glm::vec2(1.0f),
				.Right = glm::vec2(1.0f, 0.0f),
				.Down = glm::vec2(0.0f, 1.0f),
				.Layer = 0.0f
			};

			UI_Parent_Layout Adam = {
				.Min = glm::vec2(0.0f),
				.Max = glm::vec2(1.0f),
				.Inverse_Fill_Factor = 1.0f,	// Adam doesn't truly support 'fill' operations or layout
				.Lay = glm::vec2(0.0f)
			};

			// Uses the same Adam transform for all of them

			// Adam node won't support layout- a wrapper container element is required

			// Run recursive function to update UI and add to render queues

			Recurse_UI_Element(Engine, Engine->UI.Root[Index], &Viewport, &Adam);
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
						.Right = { 2.0f, Layout::Fill_Parent },

						.Colour = glm::vec4(0.75f, 0.75f, 1.0f, 1.0f),

						.Shader = UI_Shader,
						.Mesh = UI_Shader.Create_Mesh_Wrapper(Default, {})
					}
				}
			}
		};
	}
}