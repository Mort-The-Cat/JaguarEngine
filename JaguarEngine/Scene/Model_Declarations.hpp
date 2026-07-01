#ifndef JAGUAR_MODEL_DECLARATIONS
#define JAGUAR_MODEL_DECLARATIONS

#include "../Jaguar_Config.hpp"
#include "../GLTF_Reader/GLTF_Declarations.hpp"

namespace Jaguar
{
	// This defines a basic model

	// Note that a user-defined variation of this can be freely used

	template<typename Vertex>
	class Mesh_Data;

	class Mesh	// Mesh parent class
	{
	public:
		Vertex_Buffer Buffer;

		template<typename Vertex>
		Mesh_Data<Vertex>* Get();	// built-in method to recast the parent class

		virtual void Get_Vertices(const void** Data, size_t* Count)
		{
			*Data = nullptr;
			*Count = 0;
		}

		virtual size_t Get_Vertex_Count()
		{
			return 0u;
		}
	};

	template<typename Vertex>
		class Mesh_Data : public Mesh	// Actual mesh data (depends on mesh <Vertex> type)
		{
		public:
			std::vector<Vertex> Vertices;

			void Get_Vertices(const void** Data, size_t* Count) override
			{
				*Data = Vertices.data();
				*Count = sizeof(Vertex) * Vertices.size();
			}

			size_t Get_Vertex_Count() override
			{
				return Vertices.size();
			}
		};

	template<typename Vertex>
	Mesh_Data<Vertex>* Mesh::Get()
	{
		return reinterpret_cast<Mesh_Data<Vertex>*>(this);
	}

	class Uniform;		// This is a parent class that contains important uniform info etc
						// These are the values parsed to the shader during rendering

	class Mesh_Wrapper
	{
	public:
		Mesh* Mesh;		// This is a pointer to the mesh data, stored in the asset cache
	};

	template<typename Uniform>
	class Mesh_Wrapper_Data : public Mesh_Wrapper
	{
	public:
		Uniform* Uniforms;	// This contains any uniform data that the shader might need (i.e. matrix buffers etc)
							// Animation controllers will change the values here for example when updating joints
	};

	class Model
	{
		std::vector<Mesh_Wrapper*> Meshes;	// Pointer to the specific mesh/meshes we want
											// Since this is a pointer, vertex buffers / meshes can be shared



		// Next to the model, we could perhaps also include textures? Sounds good!
	};


	//

	template<typename Vertex>
	void Initialise_Vertex_Attributes(Mesh* Target_Mesh)
	{
		glGenVertexArrays(1, &Target_Mesh->Buffer.Vertex_Attribute_ID);
		glBindVertexArray(Target_Mesh->Buffer.Vertex_Attribute_ID);

		int Offset = NULL;

		for (
			size_t Index = 0, Count = 0;
			Count < sizeof(Vertex::Attrib_Info);
			Count += sizeof(Vertex::Attrib_Info[0]), Index++)
		{
			int Type = Vertex::Attrib_Info[3 * Index];
			int Size = Vertex::Attrib_Info[3 * Index + 1];

			if (
				Type == GL_BYTE || Type == GL_UNSIGNED_BYTE ||
				Type == GL_SHORT || Type == GL_UNSIGNED_SHORT ||
				Type == GL_INT || Type == GL_UNSIGNED_INT
				)
				glVertexAttribIPointer(Index, Size, Type, sizeof(Vertex), (void*)Offset);			// Integer attribute
			else
				glVertexAttribPointer(Index, Size, Type, GL_FALSE, sizeof(Vertex), (void*)Offset);// Float attribute

			Offset += Vertex::Attrib_Info[3 * Index + 2];
		}

		// the implementation is somewhat ugly but it's quite elegant in function
	}
}

#endif