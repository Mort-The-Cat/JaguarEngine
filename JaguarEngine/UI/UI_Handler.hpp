#ifndef JAGUAR_UI_HANDLER
#define JAGUAR_UI_HANDLER

#include "../OpenGL_Handling/OpenGL_Declarations.hpp"
#include "../GLTF_Reader/GLTF_Declarations.hpp"
#include "../Scene/World_Object_Declarations.hpp"

// I wonder if this should use the same render pipeline or a separate one...

namespace Jaguar
{
	struct UI_Vertex
	{
		glm::vec2 Position, UV;

		static constexpr int Attribute_Info[] =
		{
			GL_FLOAT, 2, sizeof(Position),
			GL_FLOAT, 2, sizeof(UV)
		};
	};

	Mesh* GLTF_To_UI(GLTF::GLTF_Object* Object, bool Init_Vertex_Buffer = true);

	struct Layout
	{
		float Value;

		enum
		{
			Margin_In,		// distance inside the margin
			Margin_Out,		// distance outside the margin
			Percentage,		// percentage 0.0 to 1.0 of the UI element
			Size			// width/height of object
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
		glm::vec2 Origin, Scale;
		float Layer;
	};

	class UI_Element
	{
	public:
		UI_Transform Transform;			// The transform created by the UI element.

		Layout Left, Right, Up, Down;	// This dictates the layout of the object

		glm::vec4 Colour;	// Test colour

		std::vector<UI_Element*> Children;

		Mesh_Wrapper* Mesh;										// Includes texture / uniform data etc

		Layer Layer = { 0.0f, Layer::Global };					// This determines the current layer of the object

#define UF_TO_BE_DELETED	0				// Set this flag to delete UI element
#define UF_HIDE				1				// This flag is set if the UI element isn't to be displayed (i.e. a simple container)
											// but it can also just be used to temporarily hide a UI element for whatever reason
		bool Flags[2] = { false, false };

		Shader Shader;											// Which shader to use
	};

	void Delete_UI_Element(UI_Element** Element);
}

#endif