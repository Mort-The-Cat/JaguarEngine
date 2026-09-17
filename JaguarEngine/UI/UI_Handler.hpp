#ifndef JAGUAR_UI_HANDLER
#define JAGUAR_UI_HANDLER

#include "../OpenGL_Handling/OpenGL_Declarations.hpp"
#include "../GLTF_Reader/GLTF_Declarations.hpp"
#include "../Scene/World_Object_Declarations.hpp"

// I wonder if this should use the same render pipeline or a separate one...

namespace Jaguar
{
	struct Layout
	{
		float Value;

		enum
		{
			Margin_In,		// distance inside the margin
			Margin_Out,		// distance outside the margin
			Percentage,		// percentage 0.0 to 1.0 of the UI element
			Lay_Primary,	// column (i.e. left-right on text)
			Lay_Secondary,	// row (i.e. which line of the text it is

			Size,			// width/height of object					(note that the viewport is, by default, (-1,-1) to (1,1) )

			Fill_Parent,	// fill the space of a parent according to some ratio (and accounting for sibling elements)
			// this is analogous to doing 'size'
			// can be combined with lay_primary to pack a row
			Grow_To_Contain // grow to contain flagged internal child nodes

		} Type;
	};

	struct Layer
	{
		float Value;

		enum
		{
			Global,
			Parent
		} Type;
	};

	struct UI_Transform
	{
		glm::vec2 Origin, End;		// These are the global applied properties

		glm::vec2 Right, Down;		// These are the down/right vectors

		float Layer;				// Which layer this element is on (used for sorting elements)
	};

	struct UI_Parent_Layout
	{
		glm::vec2 Min, Max;			// These are the local positions from the local right/down vectors for x/y coordinates

		float Inverse_Fill_Factor;	// This is used for handling the 'fill_parent' ratios for child elements

		glm::vec2 Lay;				// Current layout of child nodes (in local)

		float Lay_Secondary_Offset;
	};

	class UI_Element
	{
	public:
		UI_Transform Transform;			// The transform created by the UI element.

		Layout 
			Left = { 0.0f, Layout::Margin_In },
			Right = { 0.0f, Layout::Margin_In },
			Top = { 0.0f, Layout::Margin_In },
			Bottom = { 0.0f, Layout::Margin_In };	// This dictates the layout of the object

		float Angle = 0.0f;				// Angle around origin

		std::string Name;				// The name/id of this UI element

		glm::vec4 Colour = glm::vec4(1.0f);	// Test colour

		Shader Shader;						// Which shader to use

		Mesh_Wrapper* Mesh = nullptr;							// Includes texture / uniform data etc

		Layer Layer = { 0.0f, Layer::Global };					// This determines the current layer of the object

#define UF_TO_BE_DELETED	0u				// Set this flag to delete UI element
#define UF_HIDE				1u				// This flag is set if the UI element isn't to be displayed (i.e. a simple container)
#define UF_AFFECT_PARENT	2u
											// but it can also just be used to temporarily hide a UI element for whatever reason
		union
		{
			bool Flags[3] = { false, false, true };

			struct
			{
				bool To_Be_Deleted;
				bool Hide;
				bool Affect_Parent;
			} UF;
		};



		std::vector<UI_Element*> Children;
	};

//#define Create_Element(Element, ...)\
//	__Create_Element(new UI_Element Element, __VA_ARGS__)

//	UI_Element* __Create_Element(UI_Element* Element, Shader Shader, Mesh* Mesh);

	void Delete_UI_Element(UI_Element** Element);
}

#endif