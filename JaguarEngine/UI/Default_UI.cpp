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

	Jaguar::Use_Shader(&Queue->Shader);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glFrontFace(GL_CCW);
	
}

void Demo_UI_Init_Model(Jaguar::JaguarEngine* Engine, Jaguar::Render_Queue* Queue, Jaguar::Model_Wrapper Model)
{
	// TODO

	// Use Element->Transform property and whatnot to update the uniforms required

	Jaguar::Mesh_Wrapper_Data<Demo_UI_Uniform>* Mesh_Wrapper = (Jaguar::Mesh_Wrapper_Data<Demo_UI_Uniform>*)Model.Mesh_Wrapper;

	Mesh_Wrapper->Uniforms.Transform =
		Model.Element->Transform.To_Matrix(
			Model.Element->Transform.End -
			Model.Element->Transform.Origin
	);
	Mesh_Wrapper->Uniforms.Colour = Model.Element->Colour;

	glUniform4f(glGetUniformLocation(Queue->Shader.Program_ID, "Colour"),
		Mesh_Wrapper->Uniforms.Colour.x,
		Mesh_Wrapper->Uniforms.Colour.y,
		Mesh_Wrapper->Uniforms.Colour.z,
		Mesh_Wrapper->Uniforms.Colour.w);

	glUniformMatrix3fv(glGetUniformLocation(Queue->Shader.Program_ID, "Transform"), 1, GL_FALSE,
		glm::value_ptr(Mesh_Wrapper->Uniforms.Transform));
}

void Demo_UI_Render_Model(Jaguar::JaguarEngine* Engine, Jaguar::Render_Queue* Queue, Jaguar::Model_Wrapper Model)
{
	// TODO

	// bind vertices + simple draw call

	Jaguar::Bind_Vertex_Buffer(Model.Element->Mesh->Mesh);
	glDrawArrays(GL_TRIANGLES, 0, Model.Element->Mesh->Mesh->Buffer.Vertex_Count);
}