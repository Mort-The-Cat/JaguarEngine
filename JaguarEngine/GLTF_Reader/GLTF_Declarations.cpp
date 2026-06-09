#include "GLTF_Declarations.hpp"

namespace GLTF
{
	template<typename T = float>
	void Value_Array_To_Vector(std::vector<JSON::Value>& Values, T Vector)
	{
		for (size_t Index = 0; Index < Values.size(); Index++)
			Vector[Index] = Values[Index].Float;
	}
	template<typename T = glm::mat4>
	void Value_Array_To_Matrix(std::vector<JSON::Value>& Values, T Matrix)
	{
		size_t Index = 0;
		for (size_t Column = 0; Column < Matrix.length(); Column++)	// column-major format
			for (size_t Row = 0; Row < Matrix.length(); Row++, Index++)
					Matrix[Column][Row] = Values[Index].Float;
	}

	void Get_Node_Info(GLTF_Object::Node* Target_Node, JSON::JSON_Object& Object)
	{
		glm::vec3 Translation = glm::vec3(0.0f), Scale = glm::vec3(1.0f);	// identity translation/scale
		glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);				// identity quaternion

		if (Object.Fields.count("matrix"))	// if there's a matrix?
		{
			// just use that

			Value_Array_To_Matrix<glm::mat4>(Object["matrix"].Array, Target_Node->Matrix);

			// NOTE that by default glm stores value_ptr matrix in column-major format (and so do .gltf files)
		}
		else
		{
			// Otherwise? Use rotation/scale/translation

			if (Object.Fields.count("rotation"))
				Value_Array_To_Vector<glm::quat>(Object["rotation"].Array, Rotation);

			// NOTE that by default glm stores quaternions as x,y,z,w (and so do .gltf files)

			if (Object.Fields.count("translation"))
				Value_Array_To_Vector<glm::vec3>(Object["translation"].Array, Translation);

			if (Object.Fields.count("scale"))
				Value_Array_To_Vector<glm::vec3>(Object["scale"].Array, Scale);

			Target_Node->Matrix = glm::translate(Translation) * glm::toMat4(Rotation) * glm::scale(Scale);
		}

		if (Object.Fields.count("skin"))
			Target_Node->Skin = Object["skin"].Float;
		if (Object.Fields.count("mesh"))
			Target_Node->Mesh = Object["mesh"].Float;

		if (Object.Fields.count("children"))
			Value_Array_To_Vector<std::vector<uint16_t>>(Object["children"].Array, Target_Node->Children);

		if (Object.Fields.count("name"))
			Target_Node->Name = Object["name"].String;

		// Adds skin index, mesh index, node matrix, node name, and node children indices
	}

	void Get_Mesh_Info(GLTF_Object::Mesh* Target_Mesh, JSON::JSON_Reader& Reader, size_t Mesh_Index)
	{
		if (Reader["meshes"][Mesh_Index].Object.Fields.count("name"))
			Target_Mesh->Name = Reader["meshes"][Mesh_Index]["name"].String;

		// from there, we want to load the primitives and indices

		if (Reader["meshes"][Mesh_Index].Object.Fields.count("primitives"))
		{

			if (Reader["meshes"][Mesh_Index]["primitives"][0].Object.Fields.count("indices"))
				Target_Mesh->Indices = Reader.Get_Accessor_Buffer(Reader["meshes"][Mesh_Index]["primitives"][0]["indices"].Float);

			for (const auto& Value : Reader["meshes"][Mesh_Index]["primitives"][0]["attributes"].Object.Fields)
			{
				Target_Mesh->Attributes[Value.first] = Reader.Get_Accessor_Buffer(Value.second.Float);
			}

			// Primitives are split up by material

			// I'll have to implement this later

#if DEBUG
			if (Reader["meshes"][Mesh_Index]["primitives"].Array.size() > 1)	// However, if we spot it, make note of it
			{
				printf(" >> More than 1 mesh primitive (currently unsupported) merge or disable materials when exporting\n");
			}
#endif
		}

		// for now, don't store material index or anything
	}

	void Load_GLTF_Object(GLTF_Object* Target_Object, JSON::JSON_Reader* Reader)
	{
		// Load the nodes of this scene

		if (Reader->Object.Fields.count("nodes"))	// if there are nodes? add them
		{
			Target_Object->Nodes.resize(Reader->Object["nodes"].Array.size());

			for (size_t Index = 0; Index < Target_Object->Nodes.size(); Index++)
				Get_Node_Info(&Target_Object->Nodes[Index], Reader->Object["nodes"][Index].Object);
		}

		if (Reader->Object.Fields.count("meshes"))
		{
			Target_Object->Meshes.resize(Reader->Object["meshes"].Array.size());

			for (size_t Index = 0; Index < Target_Object->Meshes.size(); Index++)
				Get_Mesh_Info(&Target_Object->Meshes[Index], *Reader, Index);
		}

		// TODO: skins and animations
	}

}