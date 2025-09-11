#ifndef TEST_GAME_LOOP_DEFINED
#define TEST_GAME_LOOP_DEFINED

//#include "../OpenGL_Handling/OpenGL_Handling.h"
//#include "../OpenGL_Handling/Scene.h"
//#include "../OpenGL_Handling/Render_Queue.h"
//#include "../OpenGL_Handling/Input_Handling.h"

#include "../Controllers/Jaguar_Engine_Wrapper.h"

enum Controls
{
	Forwards = 0,
	Backwards,
	Left,
	Right,
	Up,
	Down,

	Look_Left,
	Look_Right,

	Look_Up, 
	Look_Down,
	Number_Of_Keys
};

void Set_Input_Keycodes(Jaguar::Input_Data* Inputs)
{
	Inputs->Keys.resize(Controls::Number_Of_Keys);
	Inputs->Keys[Controls::Forwards].Keycode		=		GLFW_KEY_W;
	Inputs->Keys[Controls::Backwards].Keycode		=		GLFW_KEY_S;
	Inputs->Keys[Controls::Left].Keycode		=		GLFW_KEY_A;
	Inputs->Keys[Controls::Right].Keycode = GLFW_KEY_D;


	Inputs->Keys[Controls::Up].Keycode = GLFW_KEY_SPACE;
	Inputs->Keys[Controls::Down].Keycode = GLFW_KEY_LEFT_SHIFT;

	Inputs->Keys[Controls::Look_Left].Keycode = GLFW_KEY_LEFT;
	Inputs->Keys[Controls::Look_Right].Keycode = GLFW_KEY_RIGHT;
	Inputs->Keys[Controls::Look_Up].Keycode = GLFW_KEY_UP;
	Inputs->Keys[Controls::Look_Down].Keycode = GLFW_KEY_DOWN;
}

glm::vec3 Get_Direction_Vector(float X_Direction)
{
	float Z = cos(X_Direction), X = sin(X_Direction);

	return glm::vec3(X, 0, Z);
}

void Test_Engine_Loop(Jaguar::Jaguar_Engine* Engine)
{
	float Camera_X_Direction = 0;
	float Camera_Y_Direction = 0;
	glm::vec3 Player_Position = glm::vec3(0.0f);

	glViewport(0, 0, 800, 800);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	while (!glfwWindowShouldClose(Engine->Window))
	{
		// Basic graphics loop

		glClearColor(0.3, 0.3, 0.2, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Jaguar::Get_User_Inputs(Engine->Window, &Engine->User_Inputs);

		glm::mat4 View_Matrix = glm::mat4(1.0f);

		if (Engine->User_Inputs.Keys[Controls::Forwards].Pressed)
			Player_Position += 0.001f * Get_Direction_Vector(Camera_X_Direction);

		if (Engine->User_Inputs.Keys[Controls::Backwards].Pressed)
			Player_Position -= 0.001f * Get_Direction_Vector(Camera_X_Direction);

		if (Engine->User_Inputs.Keys[Controls::Up].Pressed)
			Player_Position.y += 0.001f;

		if (Engine->User_Inputs.Keys[Controls::Down].Pressed)
			Player_Position.y -= 0.001f;

		if (Engine->User_Inputs.Keys[Controls::Left].Pressed)
			Player_Position -= 0.001f * Get_Direction_Vector(Camera_X_Direction + 3.14159 / 2);

		if (Engine->User_Inputs.Keys[Controls::Right].Pressed)
			Player_Position += 0.001f * Get_Direction_Vector(Camera_X_Direction + 3.14159 / 2);

		if (Engine->User_Inputs.Keys[Controls::Look_Left].Pressed)
			Camera_X_Direction -= 0.001f;

		if (Engine->User_Inputs.Keys[Controls::Look_Right].Pressed)
			Camera_X_Direction += 0.001f;

		if (Engine->User_Inputs.Keys[Controls::Look_Up].Pressed)
			Camera_Y_Direction += 0.001f;

		if (Engine->User_Inputs.Keys[Controls::Look_Down].Pressed)
			Camera_Y_Direction -= 0.001f;

		{
			// Test handle some player inputs!

			View_Matrix = glm::rotate(Camera_Y_Direction, glm::vec3(-1, 0, 0)) *
				glm::rotate(Camera_X_Direction, glm::vec3(0, 1, 0)) *
				glm::mat4(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, -1, 0,
					0, 0, 0, 1
				);				// The VIEW matrix is designed for a left-handed coordinate system (i.e. +z = forwards, +y = up, +x = right)

			View_Matrix = glm::translate(View_Matrix, -Player_Position);
		}

		Engine->Scene.Camera_Projection_Matrix = glm::perspective(glm::radians(85.0f), 1.0f, 0.01f, 100.0f) // Sets nice camera projection matrix
			* View_Matrix;

		Jaguar::Handle_Scene_Controllers(Engine);

		Jaguar::Draw_Render_Pipeline(&Engine->Pipeline, &Engine->Scene);

		glfwSwapBuffers(Engine->Window);
		glfwPollEvents();
	}
}

void Run_Scene(Jaguar::Jaguar_Engine* Engine)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	Engine->Window = glfwCreateWindow(800, 800, "Test window!", NULL, NULL);

	if (Engine->Window == NULL)
		Throw_Error(" >> Failed to create OpenGL window!\n\t%s\n");

	glfwMakeContextCurrent(Engine->Window);

	if (!gladLoadGL())
		Throw_Error(" >> Failed to initialise glad!\n\t%s\n");

	Set_Input_Keycodes(&Engine->User_Inputs);

	//
	
	Jaguar::Shader Test_Shader;
	Jaguar::Create_Shader("Shaders/Test_Shader.frag", "Shaders/Test_Shader.vert", &Test_Shader);

	Jaguar::Shader Test_Skeletal_Animation_Shader;
	Jaguar::Create_Shader("Shaders/Test_Shader.frag", "Shaders/Test_Skeletal_Animation.vert", &Test_Skeletal_Animation_Shader);

	Jaguar::Push_Render_Pipeline_Queue(&Engine->Pipeline, Test_Shader);
	Jaguar::Push_Render_Pipeline_Queue(&Engine->Pipeline, Test_Skeletal_Animation_Shader, 
		Jaguar::Default_Shader_Init_Function, Jaguar::Skeletal_Animation_Uniform_Assign_Function);

	Jaguar::World_Object* Object = new Jaguar::World_Object();
	Object->Mesh = Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/untitled.dae").Buffer;
	Object->Albedo = Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick.png").Texture;
	Object->Orientation = glm::vec3(0, 0, 1);
	Object->Orientation_Up = glm::vec3(0, 1, 0);
	Object->Position = glm::vec3(-1, -2, 5); // In front of camera and below slightly, and slightly to the left
	Object->Control = new Jaguar::Animator_Controller(
		Object, 
		Jaguar::Pull_Animation(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/untitled.dae").Animation,
		Jaguar::Pull_Skeleton(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/untitled.dae").Skeleton);
	Object->Flags[MF_ACTIVE] = true; // Set the object as active

	Jaguar::World_Object* Object_2 = new Jaguar::World_Object();
	Object_2->Mesh = Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Viking_Room_Test.dae").Buffer;
	Object_2->Albedo = Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick.png").Texture;
	Object_2->Orientation = glm::vec3(0, 0, 1);
	Object_2->Orientation_Up = glm::vec3(0, 1, 0);
	Object_2->Position = glm::vec3(0, -0.5f, 0);

	// Jaguar::Pull_Animation(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/untitled.dae");

	Jaguar::Add_Scene_Object(&Engine->Scene, Object, &Engine->Pipeline, &Test_Skeletal_Animation_Shader);
	Jaguar::Add_Scene_Object(&Engine->Scene, Object_2, &Engine->Pipeline, &Test_Shader);

	Test_Engine_Loop(Engine);

	Jaguar::Delete_All(&Engine->Scene);
	Jaguar::Handle_Deletions(Engine);

	Jaguar::Destroy_Shader(&Test_Shader);
	Jaguar::Destroy_Shader(&Test_Skeletal_Animation_Shader);

	Jaguar::Clear_Render_Pipeline(&Engine->Pipeline);

	Jaguar::Delete_All_Mesh_Cache(&Engine->Asset_Cache);
	Jaguar::Delete_All_Texture_Cache(&Engine->Asset_Cache);
	Jaguar::Delete_All_Skeleton_Cache(&Engine->Asset_Cache);
	Jaguar::Delete_All_Animation_Cache(&Engine->Asset_Cache);

	glfwTerminate();
}

#endif