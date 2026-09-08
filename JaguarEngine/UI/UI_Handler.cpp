#include "UI_Handler.hpp"

namespace Jaguar
{
	Mesh* GLTF_To_UI(GLTF::GLTF_Object* Object, bool Init_Vertex_Buffer)
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

	void Delete_UI_Element(UI_Element** Element)
	{
		for (size_t Index = 0; Index < Element[0]->Children.size(); Index++)	// Deletes children
			Delete_UI_Element(Element[0]->Children.data() + Index);

		delete Element[0];														// Deallocates element

		Element[0] = nullptr;													// Sets element pointer to 'null'
	}
}