#include "../Controllers/Jaguar_Engine_Wrapper.h"

void Setup_Cornell_Box(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Test_Shader, Jaguar::Shader Test_Skeletal_Animation_Shader)
{
	Jaguar::World_Object* Object;

	Object = new Jaguar::World_Object();
	Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
		Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Nice_Cornell_Box.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Cornell_Box.png").Texture,
		Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default_Normal.png").Texture,
		nullptr);

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.4f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(0.0f, 0.8f, 0.32f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;
}

void Setup_Test_Level(Jaguar::Jaguar_Engine* Engine, Jaguar::Shader Test_Shader, Jaguar::Shader Test_Skeletal_Animation_Shader)
{
	Jaguar::World_Object* Object;

	if constexpr (false)
	{

		Object = new Jaguar::World_Object();
		Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Environment_Bricks.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
			Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick.png").Texture,
			Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick_Normal.png").Texture,	// Normal map
			nullptr
		);

		Object = new Jaguar::World_Object();
		Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Environment_Tiles.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
			Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Tiles.png").Texture,
			Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Default_Normal.png").Texture,	// Normal map
			nullptr
		);
	}
	else
	{

		Object = new Jaguar::World_Object();
		Jaguar::Create_World_Object(Engine, Object, &Test_Shader,
			Jaguar::Pull_Mesh(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Models/Test_Level.dae", LOAD_MESH_HINT_LIGHTMAP_STATIC).Buffer,
			Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick.png").Texture,
			Jaguar::Pull_Texture(&Engine->Asset_Cache, "Test_Game_Loop/Assets/Textures/Brick_Normal.png").Texture,	// Normal map
			nullptr
		);

	}

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.5f, 0.25f, 0.475f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(-1.0f, 1.1f, 4.3f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;

	Engine->Scene.Lighting.Lightsources.push_back(new Jaguar::Lightsource());
	Engine->Scene.Lighting.Lightsources.back()->Colour = glm::vec3(0.25f, 0.25f, 0.475f);
	Engine->Scene.Lighting.Lightsources.back()->Position = glm::vec3(0.0f, 2.1f, 0.0f);
	Engine->Scene.Lighting.Lightsources.back()->Radius = 0.3f;
}