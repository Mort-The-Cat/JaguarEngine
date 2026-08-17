#ifndef JAGUAR_INPUTS
#define JAGUAR_INPUTS

namespace Jaguar
{
	// Note that this should include keyboard/mouse + gamepad/analogue

	// This means we need buttons and some floats

	struct Button_Flags
	{
		GLuint Keycode;		// keycode for glfw getkey or gamepad.buttons
		bool Pressed;
		bool Changed;
	};

	struct Analogue_Flags
	{
		GLuint Code;		// axes on the glfw gamepad 
		float Value;
		float Previous;
	};

	struct Gamepad
	{
		bool Detected;		// set if this gamepad is actually detected
		GLuint Joystick;	// GLFW_JOYSTICK_1, GLFW_JOYSTICK_2, etc etc

		std::vector<Button_Flags> Buttons;		// buttons on controller		NOTE: (these need to be initialised)
		std::vector<Analogue_Flags> Analogues;	// joystick axes, triggers, etc	NOTE: (these need to be initialised)
	};

	struct Inputs_Data
	{
		// NOTE: Game may have multiple joypads connected.

		std::u32string Text_Input;	// This is whatever text the user has written for this frame (note this includes dead-keys etc)
									// note that this is also in UTF32 format.

		std::vector<Button_Flags> Keys;		// keyboard inputs

		std::vector<Gamepad> Controllers;	// different connected controllers
	};

	void Read_Input_Config(JaguarEngine* Engine, const char* String);	// This gets config from a JSON string
	void Load_Input_Config(JaguarEngine* Engine, const char* Filename);	// This loads the JSON config file and reads it

	void Get_User_Inputs(JaguarEngine* Engine);	// This gets user inputs
}

#endif