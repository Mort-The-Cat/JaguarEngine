#ifndef JAGUAR_ENGINE_WRAPPER
#define JAGUAR_ENGINE_WRAPPER

#include "Jaguar_Config.hpp"

//

#ifndef DEBUG
	#define DEBUG 0
#endif


// This is user-defined and game-dependent. Can be replaced using a #define before the #include<JaguarEngine/JaguarEngine.hpp>

#ifndef JAGUAR_GAME_TYPENAME
#define JAGUAR_GAME_TYPENAME char
#endif

//

#include<string>
#include "OpenGL_Handling/OpenGL_Declarations.hpp"

#include "JSON_IO/JSON_Declarations.hpp"
#include "GLTF_Reader/GLTF_Declarations.hpp"
#include "Asset_Cache.hpp"

#include "Scene/Model_Declarations.hpp"

namespace Jaguar
{


	struct JaguarEngine
	{
		Window_Data Window_Info;

		Render_Pipeline Pipeline;		// Collection of render queues

		Asset_Cache_Data Asset_Cache;	// Responsible for handling all assets loaded at runtime (i.e. meshes, textures, audio, etc)

		JAGUAR_GAME_TYPENAME Game_Info;	// This is the relevant data for the game that JaguarEngine is running
	};

	int Init_Jaguar(JaguarEngine* Engine);

	int Init_Window(JaguarEngine* Engine, int Width = 640, int Height = 480, const char* Name = "JaguarEngine Window", GLFWmonitor* Fullscreen = nullptr);

	void End_Jaguar(JaguarEngine* Engine);

	std::vector<char> Load_File_Contents(const char* Filename);
}

#endif