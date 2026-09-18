#include "../JaguarEngine.hpp"
#include "UI_Pipeline.hpp"

namespace Jaguar
{
	// Need a recursive function to handle ui elements sequentially

	template<const bool Axis = false, const bool Side = false>
	void Evaluate_Axis(Layout* First, Layout* Next, const UI_Transform* Parent_Transform, UI_Parent_Layout* Parent_Layout, float* A, float* B, bool Change_Parent_Layout, bool Layout_Wrap)
	{
		// A is min
		// B is max

		if (First == nullptr)	// Prevents infinite recursion
			return;

		float Size;

		switch (First->Type)
		{
			case Layout::Lay_Secondary:
				if constexpr (Side)
					*A = Parent_Transform->End[Axis] - Parent_Layout->Lay[1];
				else
					*A = Parent_Transform->Origin[Axis] + Parent_Layout->Lay[1];
				Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A, Change_Parent_Layout, Layout_Wrap);
			break;

			case Layout::Lay_Primary:
				if constexpr (Side)
				{
					*A = Parent_Transform->End[Axis] - Parent_Layout->Lay[0];
					Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A, Change_Parent_Layout, Layout_Wrap);
					if (*B < Parent_Transform->Origin[Axis] && Parent_Layout->Lay[0] != 0 && Layout_Wrap)
					{
						Parent_Layout->Lay[0] = 0;
						Parent_Layout->Lay[1] += Parent_Layout->Lay_Secondary_Offset;
						*A = Parent_Transform->Origin[Axis] - Parent_Layout->Lay[0];
						Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A, Change_Parent_Layout, Layout_Wrap);
					}
					Parent_Layout->Lay[0] = Parent_Transform->End[Axis] - *B;	// add the size
				}
				else
				{
					*A = Parent_Transform->Origin[Axis] + Parent_Layout->Lay[0];
					Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A, Change_Parent_Layout, Layout_Wrap);
					if (*B > Parent_Transform->End[Axis] && Parent_Layout->Lay[0] != 0 && Layout_Wrap)
					{
						Parent_Layout->Lay[0] = 0;
						Parent_Layout->Lay[1] += Parent_Layout->Lay_Secondary_Offset;
						*A = Parent_Transform->Origin[Axis] + Parent_Layout->Lay[0];
						Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A, Change_Parent_Layout, Layout_Wrap);
					}
					Parent_Layout->Lay[0] = *B - Parent_Transform->Origin[Axis];	// add the size
				}
			break;

			case Layout::Percentage:
				*A =
					Parent_Transform->Origin[Axis] * (1.0f - First->Value) +
					Parent_Transform->End[Axis] * First->Value;
				Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A, Change_Parent_Layout, Layout_Wrap);
				break;

			case Layout::Margin_Out:
				if constexpr (Side)
					*A =
						Parent_Transform->End[Axis] + First->Value;
				else
					*A =
						Parent_Transform->Origin[Axis] - First->Value;
				Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A, Change_Parent_Layout, Layout_Wrap);
				break;

			case Layout::Margin_In:
				if constexpr (Side)
					*A =
						Parent_Transform->End[Axis] - First->Value;
				else
					*A =
						Parent_Transform->Origin[Axis] + First->Value;

				Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A, Change_Parent_Layout, Layout_Wrap);
				break;

			//

			case Layout::Fill_Parent:
				Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A, Change_Parent_Layout, Layout_Wrap);
				if constexpr (Side)
					*A = 
					(First->Value * Parent_Layout->Inverse_Fill_Factor[Axis])				// the fraction of the space this element takes up
					* (Parent_Transform->End[Axis] - Parent_Transform->Origin[Axis])	// the space itself
					+ *B;															// the previous side
				else
					*A =
					(First->Value * Parent_Layout->Inverse_Fill_Factor[Axis])				// the fraction of the space this element takes up
					* (Parent_Transform->Origin[Axis] - Parent_Transform->End[Axis])	// the space itself
					+ *B;															// the previous side
				break;

			case Layout::Size:
				Evaluate_Axis<Axis, !Side>(Next, nullptr, Parent_Transform, Parent_Layout, B, A, Change_Parent_Layout, Layout_Wrap);
				if constexpr (Side)
					*A = *B + First->Value;
				else
					*A = *B - First->Value;
				break;

			// TODO: Lay Primary / Lay Secondary
		}

		if (Change_Parent_Layout)
		{
			if (Side)																	// Expands parent layout if applicable
				Parent_Layout->Max[Axis] = std::fmaxf(Parent_Layout->Max[Axis], *A);
			else
				Parent_Layout->Min[Axis] = std::fminf(Parent_Layout->Min[Axis], *A);
		}

		//Evaluate_Axis(Next, nullptr, Parent_Transform, Parent_Layout, Next_Value, First_Value);
	}

	void Handle_UI_Dimensions(JaguarEngine* Engine, UI_Element* Element, const UI_Transform* Parent_Transform, UI_Parent_Layout* Parent_Layout)
	{
		if (Element->Top.Type == Layout::Lay_Primary || Element->Bottom.Type == Layout::Lay_Primary)
		{
			Evaluate_Axis<1, 0>(
				&Element->Top,
				&Element->Bottom,
				Parent_Transform,
				Parent_Layout,
				&Element->Transform.Origin[1],
				&Element->Transform.End[1],
				Element->UF.Affect_Parent,
				Parent_Layout->Wrap);

			Evaluate_Axis<0, 0>(
				&Element->Left,
				&Element->Right,
				Parent_Transform,
				Parent_Layout,
				&Element->Transform.Origin[0],
				&Element->Transform.End[0],
				Element->UF.Affect_Parent,
				Parent_Layout->Wrap);
		}
		else
		{
			Evaluate_Axis<0, 0>(
				&Element->Left,
				&Element->Right,
				Parent_Transform,
				Parent_Layout,
				&Element->Transform.Origin[0],
				&Element->Transform.End[0],
				Element->UF.Affect_Parent,
				Parent_Layout->Wrap);

			Evaluate_Axis<1, 0>(
				&Element->Top,
				&Element->Bottom,
				Parent_Transform,
				Parent_Layout,
				&Element->Transform.Origin[1],
				&Element->Transform.End[1],
				Element->UF.Affect_Parent,
				Parent_Layout->Wrap);
		}

		//

		Element->Transform.End -= Element->Transform.Origin;
		Element->Transform.Origin -= Parent_Transform->Origin;
		Element->Transform.Origin = (Parent_Transform->To_Matrix() * glm::vec3(Element->Transform.Origin, 1.0f));
		Element->Transform.End += Element->Transform.Origin;
	}

	void Recurse_UI_Element(JaguarEngine* Engine, UI_Element* Element, const UI_Transform* Parent_Transform, UI_Parent_Layout* Parent_Layout)
	{
		// calculates this element's transform 
		// updates parent layout if appropriate

		// adds to render pipeline if appropriate

		// Element->Controller->Function(); or whatever

		UI_Parent_Layout Copy = *Parent_Layout;

		Handle_UI_Dimensions(Engine, Element, Parent_Transform, &Copy);

		if (!(Element->UF.To_Be_Deleted || Element->UF.Hide))
		{
			Model_Wrapper Wrapper;
			Wrapper.Element = Element;
			Wrapper.Mesh_Wrapper = Element->Mesh;

			Engine->Pipeline.Queues[Engine->Pipeline.Queue_Table[Element->Shader.Program_ID]].Models.push_back(Wrapper);
		}

		UI_Parent_Layout Layout;
		Layout.Min = Element->Transform.Origin;
		Layout.Max = Element->Transform.End;
		Layout.Inverse_Fill_Factor = glm::vec2(0.0f);
		for (int Child = 0; Child < Element->Children.size(); Child++)
			Layout.Inverse_Fill_Factor += Element->Children[Child]->Get_Fill_Ratio();
		Layout.Inverse_Fill_Factor = glm::vec2(1.0f) / Layout.Inverse_Fill_Factor;

		for (int Child = 0; Child < Element->Children.size(); Child++)
		{
			Recurse_UI_Element(Engine, Element->Children[Child], &Element->Transform, &Layout);
		}

		Handle_UI_Dimensions(Engine, Element, Parent_Transform, Parent_Layout);
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
				.Inverse_Fill_Factor = glm::vec2(1.0f),	// Adam doesn't truly support 'fill' operations or layout
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
				.Left = { 0.20f / Engine->Scene.Camera.Aspect, Layout::Margin_In },
				.Right = { 0.80f / Engine->Scene.Camera.Aspect, Layout::Size },
				.Top = { 0.2f, Layout::Margin_In },
				.Bottom = { 0.5, Layout::Size },

				.Colour = glm::vec4(0.5f, 0.5f, 0.5f, 0.8f),

				.Shader = UI_Shader,
				.Mesh = UI_Shader.Create_Mesh_Wrapper(Default, {}),	// Each element needs its own wrapper/shader (unless it's hidden)

				.Children =
				{
					new UI_Element{
						.Left = { 0.025f / Engine->Scene.Camera.Aspect, Layout::Margin_In },
						.Right = { 0.025f / Engine->Scene.Camera.Aspect, Layout::Margin_In },
						.Top = { 0.025f, Layout::Margin_In },
						.Bottom = { 0.025f, Layout::Margin_In },

						.UF = {
							.Hide = true
						},

						.Children =
						{
							new UI_Element
							{
								.Left = { 0.025f, Layout::Lay_Primary },
								.Right = { 1.0f, Layout::Fill_Parent },
								.Top = { 0.0, Layout::Percentage },
								.Bottom = { 1.0, Layout::Percentage },

								.Colour = glm::vec4(1.0f, 0.75f, 0.75f, 0.8f),

								.Shader = UI_Shader,
								.Mesh = UI_Shader.Create_Mesh_Wrapper(Default, {})
							},

							new UI_Element
							{
								.Left = { 0.025f, Layout::Lay_Primary },
								.Right = { 1.0f, Layout::Fill_Parent },
								.Top = { 0.0, Layout::Percentage },
								.Bottom = { 1.0, Layout::Percentage },

								.Colour = glm::vec4(0.75f, 1.0f, 0.75f, 0.8f),

								.Shader = UI_Shader,
								.Mesh = UI_Shader.Create_Mesh_Wrapper(Default, {})
							},

							new UI_Element
							{
								.Left = { 0.025f, Layout::Lay_Primary },
								.Right = { 2.0f, Layout::Fill_Parent },
								.Top = { 0.0, Layout::Percentage },
								.Bottom = { 1.0, Layout::Percentage },

								.Colour = glm::vec4(0.75f, 0.75f, 1.0f, 0.8f),

								.Shader = UI_Shader,
								.Mesh = UI_Shader.Create_Mesh_Wrapper(Default, {})
							},

							new UI_Element
							{
								.Left = { 0.025f, Layout::Lay_Primary },
								.Right = { 1.0f, Layout::Fill_Parent },
								.Top = { 0.0, Layout::Percentage },
								.Bottom = { 1.0, Layout::Percentage },

								.Colour = glm::vec4(0.3f, 0.3f, 0.3f, 0.8f),

								.Shader = UI_Shader,
								.Mesh = UI_Shader.Create_Mesh_Wrapper(Default, {})
							}
						}
					}
				}
			}
		};
	}
}