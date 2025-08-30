#include "Collada_Loader.h"

std::string Load_File_Contents(std::string Filename)
{
	std::ifstream File(Filename.c_str()); // Start at end of file

	if (!File.is_open())
	{
		Throw_Error(" >> Fatal error! Unable to load file contents: %s\n");
	}

	// Otherwise, we can continue

	File.seekg(0, std::ios::end);

	std::string Contents;
	size_t Size = File.tellg();

	File.seekg(0, std::ios::beg);

	Contents.resize(Size); // Fits buffer

	File.read((char*)Contents.data(), Size);

	File.close();

	return Contents;
}

namespace Collada
{
	long Search_String_Contents(const char* Characters, const char* Key) // Returns the index of the substring we're looking for, returns -1
	{
		const char* Pointer = strstr(Characters, Key);

		if (Pointer)
			return Pointer - Characters;

		return -1;
	}

	long Next_Nonwhitespace(const char* Characters)
	{
		long Index = 0;
		while (isspace(Characters[Index]))
			Index++;

		return Index;
	}

	void Load_XML_Document_Node(const std::string& Contents, long& Index, XML_Document* Parent_Node)
	{
		// Get node type
		long Delta = 1 + Search_String_Contents(Contents.data() + Index, "<"); // Checks for next node

		if (!Delta) // There's nothing more?
			return;		// Just end

		Index += Delta; // Gets next node

		std::string Node_Type = "";

		while (Contents.data()[Index] != ' ' && Contents.data()[Index] != '>')
		{
			Node_Type += Contents.data()[Index];
			Index++;
		}

		Index += 1 + Search_String_Contents(Contents.data() + Index, ">"); // Passes node

		if (Contents.data()[Index - 2] == '/') // If doesn't have </type> terminator
			return;

		Parent_Node->Nodes[Node_Type].push_back(XML_Document());

		XML_Document* Current_Node = &Parent_Node->Nodes[Node_Type].back();

		Index += Next_Nonwhitespace(Contents.data() + Index); // Gets first non whitespace character

		if (Contents.data()[Index] == '<') // New node bracket!!
		{
			// Add children in loop!
			// return once </ is reached
			while (Contents.data()[Index + 1] != '/') // Check for termination node bracket </type>
			{
				Load_XML_Document_Node(Contents, Index, Current_Node); 
				// Don't worry about pointers because this adds things in a stack
				// there should be no situation in which this pointer is reallocated somewhere else (due to pushing another element to the vector)

				Index += Search_String_Contents(Contents.data() + Index, "<");
			}
			Index++;
			return; // We've reached </, return to the parent node
		}
		else
		{
			// Add data and return once </ is reached

			//

			while (Contents.data()[Index] != '<') // We can infer that this is the end point
			{
				Current_Node->Data_Array.push_back(""); // Gets new empty string as buffer

				while (!isspace(Contents.data()[Index]) && Contents.data()[Index] != '<')
				{
					Current_Node->Data_Array.back() += Contents.data()[Index];
					Index++;
				}

				Index += Next_Nonwhitespace(Contents.data() + Index);
			}

			Index++;

			return;
		}

		// We'll skip ID stuff for now

		// If we encounter numbers, add data array

		// If we instead encounter a <, add child elements until we encounter <\


	}

	int Load_XML_Document(std::string Filename, XML_Document* Target_Document)
	{
		std::string Contents = Load_File_Contents(Filename);

		long Index = 1 + Search_String_Contents(Contents.data(), ">"); // Skips Collada header

		Load_XML_Document_Node(Contents, Index, Target_Document);

		return 0;
	}
}