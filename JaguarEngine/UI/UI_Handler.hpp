#ifndef JAGUAR_UI_HANDLER
#define JAGUAR_UI_HANDLER

// I wonder if this should use the same render pipeline or a separate one...

namespace Jaguar
{
	struct Layout
	{
		enum
		{
			Margin_In,		// distance inside the margin
			Margin_Out,		// distance outside the margin
			Percentage,		// percentage 0.0 to 1.0 of the UI element
			Size			// width/height of object
		} Type;

		float Value;
	};

	class UI_Element
	{
		Layout Left, Right, Up, Down;	// This dictates the layout of the object

		
	};
}

#endif