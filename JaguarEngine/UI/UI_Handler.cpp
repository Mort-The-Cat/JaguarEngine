#include "UI_Handler.hpp"

namespace Jaguar
{
	void Delete_UI_Element(UI_Element** Element)
	{
		for (size_t Index = 0; Index < Element[0]->Children.size(); Index++)	// Deletes children
			Delete_UI_Element(Element[0]->Children.data() + Index);

		delete Element[0];														// Deallocates element

		Element[0] = nullptr;													// Sets element pointer to 'null'
	}
}