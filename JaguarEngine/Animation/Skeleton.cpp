#include "Skeleton.hpp"

namespace Jaguar
{
	int Find_Root_Node(Skeleton* Target_Skeleton)
	{
		uint64_t Flags = -1;
		for (size_t Node = 0; Node < Target_Skeleton->Nodes.size(); Node++)
			for (size_t Child = 0; Child < Target_Skeleton->Nodes[Node].Child_Nodes.size(); Child++)
				Flags &= ~(1ui64 << (unsigned long long)Target_Skeleton->Nodes[Node].Child_Nodes[Child]);					// clears this flag

		return _tzcnt_u64(Flags);	// Finds first node in the list that ISN'T referred to as a 'child' node
	}

	void Create_Skeleton(Skeleton* Target_Skeleton, const char* Filename)
	{
		JSON::JSON_Reader Reader;
		GLTF::GLTF_Object Object;

		JSON::Load_JSON_Object(&Reader.Object, Filename);
		JSON::Load_JSON_Reader_Buffers(&Reader);

		GLTF::Load_GLTF_Object(&Object, &Reader);

		Create_Skeleton(Target_Skeleton, &Object);
	}

	int Get_Skin(GLTF::GLTF_Object* Object)
	{
		for (size_t Index = 0; Index < Object->Nodes.size(); Index++)
			if (Object->Nodes[Index].Skin != -1)
				return Object->Nodes[Index].Skin;

		return -1;
	}

	void Create_Skeleton(Skeleton* Target_Skeleton, GLTF::GLTF_Object* Object)
	{
		Target_Skeleton->Nodes.resize(Object->Nodes.size());

		GLTF::GLTF_Object::Skin* Target_Skin = &Object->Skins[Get_Skin(Object)];

		for (size_t Node = 0; Node < Object->Nodes.size(); Node++)
		{
			Target_Skeleton->Nodes[Node].Matrix = Object->Nodes[Node].Matrix;
			Target_Skeleton->Nodes[Node].Child_Nodes = Object->Nodes[Node].Children;
			Target_Skeleton->Nodes[Node].Joint = -1;
		}

		for (size_t Joint = 0; Joint < Target_Skin->Joints.size(); Joint++)
			Target_Skeleton->Nodes[Target_Skin->Joints[Joint]].Joint = Joint;

		Target_Skeleton->Inverse_Bind_Matrices = Target_Skin->Inverse_Bind_Matrices.Get_Attribute_Buffer<glm::mat4>();
		Target_Skeleton->Root_Node = Find_Root_Node(Target_Skeleton);
	}

}