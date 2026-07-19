#include "../JaguarEngine.hpp"

namespace Jaguar
{

	void Handle_Scene_Objects(JaguarEngine* Engine)
	{
		for (size_t Index = 0; Index < Engine->Scene.Scene_Objects.size(); Index++)
		{
			if (Engine->Scene.Scene_Objects[Index]->Controller)
				Engine->Scene.Scene_Objects[Index]->Controller->Function(Engine, Engine->Scene.Scene_Objects[Index]);
		}
	}

}