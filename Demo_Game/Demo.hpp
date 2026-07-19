#include "../JaguarEngine/JaguarEngine.hpp"

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

struct Demo_Uniform
{
	glm::mat4 Model_Matrix;



	// Texture? Later
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

	for (size_t Node = 0; Node < Object->Nodes.size(); Node++)
		if (Object->Nodes[Node].Mesh != -1)
		{
			size_t Index = Object->Nodes[Node].Mesh;	// Doesn't yet account for child nodes of an object

			Positions = Object->Meshes[Index].Attributes["POSITION"].Get_Attribute_Buffer<glm::vec3>();
			Normals = Object->Meshes[Index].Attributes["NORMAL"].Get_Attribute_Buffer<glm::vec3>();
			UVs = Object->Meshes[Index].Attributes["TEXCOORD_0"].Get_Attribute_Buffer<glm::vec2>();

			Indices = Object->Meshes[Index].Indices.Get_Attribute_Buffer<glm::vec<1, size_t>>();

			for (size_t V = 0; V < Indices.size(); V++)
			{
				PNUV_Vertex Vertex;
				Vertex.Position = Object->Nodes[Node].Matrix * glm::vec4(Positions[Indices[V].x], 1);	// Apply transformation to positions
				Vertex.Normal = glm::mat3(Object->Nodes[Node].Matrix) * Normals[Indices[V].x];			// Apply only rotation to normals
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

	return Mesh;
}

void Demo_Init_Queue(Jaguar::JaguarEngine* Engine, Jaguar::Render_Queue* Queue)
{
	Jaguar::Use_Shader(&Queue->Shader);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);

	// Init camera here

	Engine->Scene.Camera.Matrix =
		Jaguar::Get_View_Matrix(Engine->Scene.Camera.Position, Engine->Scene.Camera.Orientation, Engine->Scene.Camera.Orientation_Up);

	// Then, apply projections

	Engine->Scene.Camera.Matrix = glm::perspective(Engine->Scene.Camera.FOV, Engine->Scene.Camera.Aspect, 0.01f, 10000.0f) * Engine->Scene.Camera.Matrix;

	glUniformMatrix4fv(glGetUniformLocation(Queue->Shader.Program_ID, "Projection_Matrix"), 1, GL_FALSE, glm::value_ptr(Engine->Scene.Camera.Matrix));

	glUniform3f(glGetUniformLocation(Queue->Shader.Program_ID, "Camera_Position"), Engine->Scene.Camera.Position.x, Engine->Scene.Camera.Position.y, Engine->Scene.Camera.Position.z);
}

void Demo_Init_Model(Jaguar::JaguarEngine* Engine, Jaguar::Render_Queue* Queue, Jaguar::Model_Wrapper Wrapper)
{
	Jaguar::Mesh_Wrapper_Data<Demo_Uniform>* Model = (Jaguar::Mesh_Wrapper_Data<Demo_Uniform>*)Wrapper.Mesh_Wrapper;

	Model->Uniforms.Model_Matrix = Jaguar::Get_Matrix(Wrapper.Object->Position, Wrapper.Object->Orientation, Wrapper.Object->Orientation_Up);

	glUniformMatrix4fv(glGetUniformLocation(Queue->Shader.Program_ID, "Model_Matrix"), 1, GL_FALSE, glm::value_ptr(Model->Uniforms.Model_Matrix));

	Jaguar::Attach_Texture(&Model->Textures[0], &Queue->Shader, "Texture_Test", 0);
}

void Demo_Render_Model(Jaguar::JaguarEngine* Engine, Jaguar::Render_Queue* Queue, Jaguar::Model_Wrapper Wrapper)
{
	Jaguar::Bind_Vertex_Buffer(Wrapper.Mesh_Wrapper->Mesh);
	glDrawArrays(GL_TRIANGLES, 0, Wrapper.Mesh_Wrapper->Mesh->Buffer.Vertex_Count);
}

struct Control_Spinner
{
	float Direction = 0;
};

void Spin_Object(Jaguar::JaguarEngine* Engine, Jaguar::World_Object* Object)
{
	Jaguar::Control_Type<Control_Spinner>* Controller = (Jaguar::Control_Type<Control_Spinner>*)Object->Controller;

	Controller->Info.Direction += Engine->Time;

	Controller->Object->Orientation.x = sinf(Controller->Info.Direction);
	Controller->Object->Orientation.z = cosf(Controller->Info.Direction);
}

int Run_Scene(Jaguar::JaguarEngine* Engine)
{
	Jaguar::Shader Demo_Shader;
	Jaguar::Create_Shader<Demo_Uniform>(&Demo_Shader, "Demo_Game/Shaders/PNUV.vert", "Demo_Game/Shaders/PNUV.frag");
	Jaguar::Push_Render_Pipeline_Queue(
		Engine,
		Demo_Shader,
		Demo_Init_Queue,
		Demo_Init_Model,
		Demo_Render_Model
	);

	Jaguar::World_Object World_Object;

	Jaguar::Create_World_Object(
		Engine,
		&World_Object,
		{
			{
				Demo_Shader, Jaguar::Pull_Mesh(Engine, GLTF_To_Mesh, "Demo_Game/Assets/Viking_Room.gltf").Mesh,
				{ Jaguar::Pull_Texture(Engine, "Demo_Game/Assets/Viking_Room.png").Texture }
			}
		},
		{},
		new Jaguar::Control_Type<Control_Spinner>(Spin_Object),
		glm::vec3(0.0f, -2.0f, 3.0f)
	);

	Jaguar::World_Object World_Object_1;

	Jaguar::Create_World_Object(
		Engine,
		&World_Object_1,
		{
			{
				Demo_Shader, Jaguar::Pull_Mesh(Engine, GLTF_To_Mesh, "JaguarEngine/JSON_IO/animation.gltf").Mesh,
				{ Jaguar::Pull_Texture(Engine, "Demo_Game/Assets/Floor_Tiles.png").Texture }
			}
		},
		{},
		nullptr,
		glm::vec3(1.0f, -2.0f, -7.0f)
	);

	float Angle = 0.8f;

	while (!glfwWindowShouldClose(Engine->Window_Info.Window))
	{
		glfwPollEvents();

		Jaguar::Tick(Engine);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Engine->Scene.Camera.Position = glm::vec3(-3.0f, -1.0f, 1.0f);
		Engine->Scene.Camera.Orientation_Up = glm::vec3(0.0f, 1.0f, 0.0f);

		Engine->Scene.Camera.Orientation = glm::vec3(
			sinf(Angle), 0.0f, cosf(Angle)
		);

		Angle += Engine->Time;

		Engine->Scene.Camera.Aspect = 640.0f / 480.0f;
		Engine->Scene.Camera.FOV = glm::radians(90.0f);

		Jaguar::Handle_Scene_Objects(Engine);

		Jaguar::Draw_Render_Pipeline(Engine);

		glfwSwapBuffers(Engine->Window_Info.Window);
	}

	return 0;
}