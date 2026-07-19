#ifndef JAGUAR_CONTROLLER
#define JAGUAR_CONTROLLER

#include "World_Object_Declarations.hpp"

namespace Jaguar
{
	class Control;

	typedef void(*Control_Function)(JaguarEngine*, World_Object*);

	void Init_Control(JaguarEngine* Engine, World_Object* Object);

	class Control // This is the object that controls different world objects etc
	{
	public:
		World_Object* Object;	// The object associated with this controller

		Control_Function Function;
		Control_Function Init;
		Control_Function Delete;

		Control() {}
		Control(Control_Function Functionp, Control_Function Initp = Init_Control, Control_Function Deletep = nullptr)
		{
			Function = Functionp;
			Init = Initp;
			Delete = Deletep;
		}
	};

	template<typename T>
	class Control_Type : public Control
	{
	public:
		T Info;
		Control_Type() {}
		Control_Type(Control_Function Functionp, Control_Function Initp = Init_Control, Control_Function Deletep = nullptr)
		{
			Function = Functionp;
			Init = Initp;
			Delete = Deletep;
		}
	};
}

#endif