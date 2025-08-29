#define CRT_SECURE_NO_WARNINGS 1
#include "Collada_Loader/Collada_Loader.h"

int main()
{
	//std::string Contents = Collada::Load_File_Contents("Collada_Loader/Collada_Loader.h");
	Collada::XML_Document Document;
	Collada::Load_XML_Document("Collada_Loader/untitled.dae", &Document);

	Collada::Collada_Mesh Mesh;
	Collada::Load_Mesh(Document, &Mesh); // I'm going to test some shaders and just see if I've loaded the model properly with this method

	return 0;
}