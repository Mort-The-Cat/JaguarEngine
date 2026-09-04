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

	Mesh* GLTF_To_UI(GLTF::GLTF_Object* Object, bool Init_Vertex_Buffer = true)
	{
		Mesh_Data<UI_Vertex>* Mesh = new Mesh_Data<UI_Vertex>();

		std::vector<glm::vec2> Positions;
		std::vector<glm::vec2> UVs;

		std::vector<glm::vec<1, size_t>> Indices;

		Positions = Object->Meshes[0].Attributes["POSITION"].Get_Attribute_Buffer<glm::vec2>();
		UVs = Object->Meshes[0].Attributes["TEXCOORD_0"].Get_Attribute_Buffer<glm::vec2>();

		Indices = Object->Meshes[0].Indices.Get_Attribute_Buffer<glm::vec<1, size_t>>();

		Mesh->Vertices.resize(Indices.size());

		for (size_t V = 0; V < Indices.size(); V++)
		{
			Mesh->Vertices[V].Position = Positions[Indices[V].x];
			Mesh->Vertices[V].UV = UVs[Indices[V].x];
		}

		if (Init_Vertex_Buffer)
		{
			Jaguar::Create_Vertex_Buffer(Mesh);
			Jaguar::Bind_Vertex_Buffer(Mesh);
			Jaguar::Initialise_Vertex_Attributes<UI_Vertex>(Mesh);
		}

		return Mesh;
	}

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

	class UI_Element
	{
		Layout Left, Right, Up, Down;	// This dictates the layout of the object

		glm::vec4 Colour;	// Test colour

		std::vector<UI_Element*> Children;

		Mesh_Wrapper* Mesh;

#define UF_TO_BE_DELETED	0
#define UF_HIDE				1
		bool Flags[2] = { false, false };
	};
}

#endif