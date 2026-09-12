#include "../JaguarEngine.hpp"

Jaguar::Mesh* GLTF_To_UI(GLTF::GLTF_Object* Object, bool Init_Vertex_Buffer)
{
	Jaguar::Mesh_Data<UI_Vertex>* Mesh = new Jaguar::Mesh_Data<UI_Vertex>();

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

//

void Demo_UI_Init_Queue(Jaguar::JaguarEngine* Engine, Jaguar::Render_Queue* Queue)
{
	// TODO

	// a little sorting required with respect to 'Element->Transform.Layer'
}

void Demo_UI_Init_Model(Jaguar::JaguarEngine* Engine, Jaguar::Render_Queue* Queue, Jaguar::Model_Wrapper Wrapper)
{
	// TODO

	// Use Element->Transform property and whatnot to update the uniforms required
}

void Demo_UI_Render_Model(Jaguar::JaguarEngine* Engine, Jaguar::Render_Queue* Queue, Jaguar::Model_Wrapper Wrapper)
{
	// TODO

	// bind vertices + simple draw call
}