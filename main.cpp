#include "JaguarEngine/JaguarEngine.hpp"

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

	while (!glfwWindowShouldClose(Engine.Window_Info.Window))
	{
		glfwPollEvents();
		glfwSwapBuffers(Engine.Window_Info.Window);
	}

	Jaguar::End_Jaguar(&Engine);

	return 0;
}