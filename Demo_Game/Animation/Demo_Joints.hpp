#include "../../JaguarEngine/JaguarEngine.hpp"

struct PNUV_Joint_Vertex
{
	glm::vec3 Position;	// There is annoying padding here but we can change this later
	glm::vec3 Normal;
	glm::vec2 UV;

	uint32_t Joints;	// 4 bytes
	glm::vec4 Weights;	// 4 floats

	static constexpr int Attrib_Info[] = // This is used when initialising the vertex attributes
	{
		GL_FLOAT, 3, sizeof(Position),
		GL_FLOAT, 3, sizeof(Normal),
		GL_FLOAT, 2, sizeof(UV),
		GL_UNSIGNED_INT, 1, sizeof(Joints),
		GL_FLOAT, 4, sizeof(Weights)
	};
};

struct Joint_Uniform
{
	glm::mat4 Model_Matrix;

	glm::mat4 Joint_Matrices[64];	// Only 64 for now
};

struct Control_Demo_Animator
{
	Jaguar::Animator Demo_Animator;
	Jaguar::Animation_Rig Rig;
};

void Demo_Animator_Init(Jaguar::JaguarEngine* Engine, Jaguar::World_Object* Object)
{
	Jaguar::Control_Type<Control_Demo_Animator>* Control = reinterpret_cast<Jaguar::Control_Type<Control_Demo_Animator>*>(Object->Controller);

	Control->Info.Demo_Animator =
		Jaguar::Animator(
			Jaguar::Pull_Animation(Engine, "Demo_Game/Assets/Murderer.gltf", "").Animation_Info
		);


	Control->Info.Rig =
		Jaguar::Animation_Rig(
			Jaguar::Pull_Skeleton(Engine, "Demo_Game/Assets/Murderer.gltf").Rig
		);
}

void Demo_Animator_Function(Jaguar::JaguarEngine* Engine, Jaguar::World_Object* Object)
{
	Jaguar::Control_Type<Control_Demo_Animator>* Control = reinterpret_cast<Jaguar::Control_Type<Control_Demo_Animator>*>(Object->Controller);

	Control->Info.Demo_Animator.Animate(&Control->Info.Rig, Engine->Time);

	if (Control->Info.Demo_Animator.Time > 16.0)
		Control->Info.Demo_Animator.Time = 0.0f;

	// Now, we'll update the object uniforms with the new joint data!!

	Jaguar::Mesh_Wrapper_Data<Joint_Uniform>* Mesh = reinterpret_cast<Jaguar::Mesh_Wrapper_Data<Joint_Uniform>*>(Object->Model.Meshes[0]);

	// it annoys me how often I have to write out these long reinterpret_cast expressions every time I want a specific kind of mesh

	Control->Info.Rig.Update_Joints(Mesh->Uniforms.Joint_Matrices); // Great!
}

void _GLTF_To_Joint_Mesh_Helper(GLTF::GLTF_Object* Object, int Node_Index, Jaguar::Mesh_Data<PNUV_Joint_Vertex>* Mesh)
{
	//glm::mat4 Current_Matrix = Parent_Matrix * Object->Nodes[Node_Index].Matrix;

	GLTF::GLTF_Object::Node* Node = &Object->Nodes[Node_Index];

	if (Node->Mesh != -1)
	{
		std::vector<glm::vec3> Positions;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec2> UVs;

		std::vector<glm::vec<4, uint8_t>> Joints;
		std::vector<glm::vec4> Weights;

		std::vector<glm::vec<1, size_t>> Indices;

		Positions = Object->Meshes[Node->Mesh].Attributes["POSITION"].Get_Attribute_Buffer<glm::vec3>();
		Normals = Object->Meshes[Node->Mesh].Attributes["NORMAL"].Get_Attribute_Buffer<glm::vec3>();
		UVs = Object->Meshes[Node->Mesh].Attributes["TEXCOORD_0"].Get_Attribute_Buffer<glm::vec2>();

		Indices = Object->Meshes[Node->Mesh].Indices.Get_Attribute_Buffer<glm::vec<1, size_t>>();

		Joints = Object->Meshes[Node->Mesh].Attributes["JOINTS_0"].Get_Attribute_Buffer<glm::vec<4, uint8_t>>();
		Weights = Object->Meshes[Node->Mesh].Attributes["WEIGHTS_0"].Get_Attribute_Buffer<glm::vec4>();

		for (size_t Index = 0; Index < Indices.size(); Index++)
		{
			PNUV_Joint_Vertex Vertex;

			Vertex.Position = Positions[Indices[Index].x];
			Vertex.Normal = Normals[Indices[Index].x];
			Vertex.UV = UVs[Indices[Index].x];

			glm::vec<4, uint8_t> Joint_Vector = Joints[Indices[Index].x];

			Vertex.Joints = 
				(uint32_t)Joint_Vector.x | 
				((uint32_t)Joint_Vector.y << 8) | 
				((uint32_t)Joint_Vector.z << 16) |
				((uint32_t)Joint_Vector.w << 24);

			Vertex.Weights = Weights[Indices[Index].x];

			Mesh->Vertices.push_back(Vertex);
		}
	}

	for (size_t Index = 0; Index < Node->Children.size(); Index++)
		_GLTF_To_Joint_Mesh_Helper(Object, Node->Children[Index], Mesh);
}

Jaguar::Mesh* GLTF_To_Joint_Mesh(GLTF::GLTF_Object* Object, bool Init_Vertex_Buffer = true)
{
	Jaguar::Mesh_Data<PNUV_Joint_Vertex>* Mesh = new Jaguar::Mesh_Data<PNUV_Joint_Vertex>();

	// This will create one big mesh, like the normal GLTF_To_Mesh function
	// but this will also store joints/weights.

	_GLTF_To_Joint_Mesh_Helper(Object, Object->Parent_Node, Mesh);

	if (Init_Vertex_Buffer)
	{
		Jaguar::Create_Vertex_Buffer(Mesh);
		Jaguar::Bind_Vertex_Buffer(Mesh);
		Jaguar::Initialise_Vertex_Attributes<PNUV_Joint_Vertex>(Mesh);
	}

	return Mesh;
}

void Demo_Joints_Init_Queue(Jaguar::JaguarEngine* Engine, Jaguar::Render_Queue* Queue)
{
	Jaguar::Use_Shader(&Queue->Shader);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	//glFrontFace(GL_CW);

	//glEnable(GL_DEPTH_TEST);

	// Init camera here

	//Engine->Scene.Camera.Matrix =
	//	Jaguar::Get_View_Matrix(Engine->Scene.Camera.Position, Engine->Scene.Camera.Orientation, Engine->Scene.Camera.Orientation_Up);

	// Then, apply projections

	//Engine->Scene.Camera.Matrix = glm::perspective(Engine->Scene.Camera.FOV, Engine->Scene.Camera.Aspect, 0.01f, 10000.0f) * Engine->Scene.Camera.Matrix;

	glUniformMatrix4fv(glGetUniformLocation(Queue->Shader.Program_ID, "Projection_Matrix"), 1, GL_FALSE, glm::value_ptr(Engine->Scene.Camera.Matrix));

	glUniform3f(glGetUniformLocation(Queue->Shader.Program_ID, "Camera_Position"), Engine->Scene.Camera.Position.x, Engine->Scene.Camera.Position.y, Engine->Scene.Camera.Position.z);

}

void Demo_Joints_Init_Model(Jaguar::JaguarEngine* Engine, Jaguar::Render_Queue* Queue, Jaguar::Model_Wrapper Wrapper)
{
	Jaguar::Mesh_Wrapper_Data<Joint_Uniform>* Model = (Jaguar::Mesh_Wrapper_Data<Joint_Uniform>*)Wrapper.Mesh_Wrapper;

	Model->Uniforms.Model_Matrix = Jaguar::Get_Matrix(Wrapper.Object->Position, Wrapper.Object->Orientation, Wrapper.Object->Orientation_Up);

	glUniformMatrix4fv(glGetUniformLocation(Queue->Shader.Program_ID, "Model_Matrix"), 1, GL_FALSE, glm::value_ptr(Model->Uniforms.Model_Matrix));
	glUniformMatrix4fv(glGetUniformLocation(Queue->Shader.Program_ID, "Joints"), 64, GL_FALSE, glm::value_ptr(Model->Uniforms.Joint_Matrices[0]));

	Jaguar::Attach_Texture(&Model->Textures[0], &Queue->Shader, "Texture_Test", 0);
}

void Demo_Joints_Render_Model(Jaguar::JaguarEngine* Engine, Jaguar::Render_Queue* Queue, Jaguar::Model_Wrapper Wrapper)
{
	Jaguar::Bind_Vertex_Buffer(Wrapper.Mesh_Wrapper->Mesh);
	glDrawArrays(GL_TRIANGLES, 0, Wrapper.Mesh_Wrapper->Mesh->Buffer.Vertex_Count);
}

// We need an animation controller etc