#include "JaguarEngine/JaguarEngine.hpp"

struct PNUV_Vertex
{
	glm::vec3 Position;	// There is annoying padding here but we can change this later
	glm::vec3 Normal;
	glm::vec2 UV;

	static constexpr int Attrib_Info[] = // This is used when initialising the vertex attributes
	{
		GL_FLOAT, 3, sizeof(glm::vec3),
		GL_FLOAT, 3, sizeof(glm::vec3),
		GL_FLOAT, 2, sizeof(glm::vec2)
	};
};

Jaguar::Mesh* GLTF_To_Mesh(GLTF::GLTF_Object* Object, bool Init_Vertex_Buffer = true)
{
	Jaguar::Mesh_Data<PNUV_Vertex>* Mesh = new Jaguar::Mesh_Data<PNUV_Vertex>();

	std::vector<glm::vec3> Positions;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> UVs;

	std::vector<glm::vec<1, size_t>> Indices;

	// i.e. if there are 600 'index' bytes and each index is 2-bytes in size,
	// that means there are 300 vertices defined
	// i.e. 100 triangles

	for (size_t Index = 0; Index < Object->Meshes.size(); Index++)
	{
		Positions = Object->Meshes[Index].Attributes["POSITION"].Get_Attribute_Buffer<glm::vec3>();
		Normals = Object->Meshes[Index].Attributes["NORMAL"].Get_Attribute_Buffer<glm::vec3>();
		UVs = Object->Meshes[Index].Attributes["TEXCOORD_0"].Get_Attribute_Buffer<glm::vec2>();

		Indices = Object->Meshes[Index].Indices.Get_Attribute_Buffer<glm::vec<1, size_t>>();

		for (size_t V = 0; V < Indices.size(); V++)
		{
			PNUV_Vertex Vertex;
			Vertex.Position = Positions[Indices[V].x];
			Vertex.Normal = Normals[Indices[V].x];
			Vertex.UV = UVs[Indices[V].x];

			Mesh->Vertices.push_back(Vertex);
		}
	}

	if (Init_Vertex_Buffer)
	{
		Jaguar::Create_Vertex_Buffer(Mesh);
		Jaguar::Bind_Vertex_Buffer(Mesh);
		Jaguar::Initialise_Vertex_Attributes<PNUV_Vertex>(Mesh);
	}

	// After this, create the vertex buffer!

	// TODO: Vertex buffer init

	return Mesh;
}

int main()
{
	Jaguar::JaguarEngine Engine;

	if (Jaguar::Init_Jaguar(&Engine))
	{
		printf(" >> Failed to init JaguarEngine!\n\n");

		return -1;
	}

	if (Jaguar::Init_Window(&Engine))
	{
		printf(" >> Failed to init window!\n\n");

		return -1;
	}

	//JSON::JSON_Object Object;

	JSON::JSON_Reader Reader;

	JSON::Load_JSON_Object(&Reader.Object, "JaguarEngine/JSON_IO/animation.gltf");
	JSON::Load_JSON_Reader_Buffers(&Reader);

	GLTF::GLTF_Object Object;
	GLTF::Load_GLTF_Object(&Object, &Reader);	// This loads the GLTF object

	Jaguar::Mesh* Mesh = GLTF_To_Mesh(&Object);

	//Jaguar::Mesh* Mesh = Jaguar::Pull_Mesh(&Engine, GLTF_To_Mesh, &Object).Mesh;

	//Jaguar::Pull_Mesh(&Engine, )

	while (!glfwWindowShouldClose(Engine.Window_Info.Window))
	{
		glfwPollEvents();
		glfwSwapBuffers(Engine.Window_Info.Window);
	}

	Jaguar::End_Jaguar(&Engine);

	return 0;
}