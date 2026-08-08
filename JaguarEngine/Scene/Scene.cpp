#include "../JaguarEngine.hpp"

namespace Jaguar
{

	void Handle_Scene_Objects(JaguarEngine* Engine)
	{
		for (size_t Index = 0; Index < Engine->Scene.Objects.size(); Index++)
		{
			if (Engine->Scene.Objects[Index]->Controller)
				Engine->Scene.Objects[Index]->Controller->Function(Engine, Engine->Scene.Objects[Index]);
		}
	}

}