#ifndef COLLADA_LOADER_INCLUDED
#define COLLADA_LOADER_INCLUDED

#include <vector>

#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <ctype.h>

#include <map>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

void Throw_Error(const char* Formatted_String);

std::string Load_File_Contents(const char* Filename);

template<typename T>
void Load_Strings_To_Matrices(const std::vector<std::string>& Strings, std::vector<T>& Matrices) // Change this function to fix the matrices in the animation files
{
	size_t W = 0;
	while (W < Strings.size())
	{
		T Matrix;

		for (size_t Row = 0; Row < Matrix.length(); Row++)
			for (size_t Column = 0; Column < Matrix[0].length(); Column++)	// Column-major format
				Matrix[Column][Row] = std::stof(Strings[W++]);

		// Swap Y and Z components

		/*std::swap(Matrix[1][0], Matrix[2][0]);
		std::swap(Matrix[1][1], Matrix[2][1]);
		std::swap(Matrix[1][2], Matrix[2][2]);
		std::swap(Matrix[1][3], Matrix[2][3]);

		Matrix[2] *= glm::vec4(-1);*/

		Matrices.push_back(Matrix);
	}
}

template<typename T>
void Load_Strings_To_Vectors(const std::vector<std::string>& Strings, std::vector<T>& Vectors)
{
	size_t W = 0;
	while (W < Strings.size()) // number of elements in position data array
	{
		T Vector;

		for (size_t Component = 0; Component < Vector.length(); Component++) // Iterates through components of vector
		{
			Vector[Component] = std::stof(Strings[W++]);
		}

		Vectors.push_back(Vector);
	}
}

// https://www.reddit.com/r/opengl/comments/j8v1iu/parallax_corrected_cube_maps_reflections/

// https://wazim.com/collada-tutorial1/

// https://gamedev.stackexchange.com/questions/31651/trouble-with-collada-bones
// https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
// https://learnopengl.com/code_viewer_gh.php?code=src/8.guest/2020/skeletal_animation/skeletal_animation.cpp
// https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/animator.h

namespace Collada
{
	struct Collada_Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 Texture_Coordinates;

		union 
		{							// A mesh should only use EITHER joints or baked lightmapping, so this will be stored in a union
									// so that the memory can be parsed in basically the same way
									// May change this later... Particles and 2d UI elements will most likely need their own special kind of vertex buffer object
			struct 
			{
				float Joint_Weight;
				uint32_t Joint_ID;
			};

			glm::vec2 Lightmap_UV;		// IMPORTANT: Note that "Lightmap_UV" takes up same amount of memory as Joint_Weight and Joint_ID
										// This is done so that static objects have room in their vertex attribute for the lightmap UVs
										// and so that dynamic objects (with skeletal animations) have room in their vertex attributes for joint info
										// This may need to be changed later, but I think this is okay for most objects
		};
	};

	class Collada_Mesh
	{
	public:
		std::vector<Collada_Vertex> Vertices; // I've decided to NOT use an index buffer for now

		// std::vector<uint32_t> Triangle_Indices; // These are the indices of each vertex of each triangle (stored as 32 bits)
	};

	struct Collada_Joint
	{
		glm::mat4 Offset_Matrix;	// This is the matrix that transforms from vertex space to bone space
		size_t Parent_Joint;
		std::vector<size_t> Child_Joints;



		glm::vec4 Final_Matrix;		// This is the final transformation matrix when updating the animation of a bone (likely will be changed later)
	};

	class Collada_Vertex_Weight
	{
	public:
		size_t Joint_Index;
		float Weight;

		bool operator<(const Collada_Vertex_Weight& Other)
		{
			return Weight < Other.Weight;
		}
	};

	class Collada_Skeleton
	{
	public:
		std::vector<Collada_Joint> Joints;

		std::vector<std::vector<Collada_Vertex_Weight>> Vertex_Weights; // Every vertex has a collection of vertex weights std::vector<Collada_Vertex_Weight>

		glm::mat4 Bind_Shape_Matrix;
		// When loading into game engine, this will be sorted and prioritised most likely
	};

	class Collada_Keyframe
	{
	public:
		//size_t Joint_Index;
		float Time;
		glm::mat4 Transformation_Matrix;
	};

	class Collada_Animation
	{
	public:
		std::vector<std::vector<Collada_Keyframe>> Keyframes; // Index to joint ID and then the keyframe index
	};

	class Collada_Object
	{
	public:
		Collada_Mesh Mesh;
		Collada_Skeleton Skeleton;
		Collada_Animation Animation;
	};

	class XML_Document
	{
	public:
		std::string Id; // 'id' field of node
		std::string Target;

				// Node type, data
		std::map<std::string, std::vector<XML_Document>> Nodes; // How to handle float/int arrays?

		// There can be multiple nodes with the same "node_type" in one context

		std::vector<std::string> Data_Array; // Can have 0 elements without issue

		std::vector<XML_Document> operator[](const char* Key) const
		{
			return Nodes.at(Key);
		}

		std::string operator[](const size_t& Index) const
		{
			return Data_Array[Index];
		}
	};

	int Load_XML_Document(const char* Filename, XML_Document* Target_Document);

	int Load_Mesh(const XML_Document& Document, Collada_Mesh* Target_Mesh, Collada_Skeleton* Skeleton = nullptr);
	int Load_Skeleton(const XML_Document& Document, Collada_Skeleton* Target_Skeleton);


	void Load_Child_Joint_Name_Map(XML_Document Parent_Node, std::map<std::string, unsigned int>& Bone_Map, std::vector<glm::mat4>& Buffer);
	int Load_Animation(const XML_Document& Document, Collada_Animation* Target_Animation);

	void Get_UV_Tangent_Bitangent_Vectors(glm::vec3 Positions[3], glm::vec2 Texture_Coordinates[3], glm::vec3 Normal, glm::vec3* Tangent, glm::vec3* Bitangent)
	{
		glm::vec2 UV_A, UV_B;
		glm::vec3 Edge_A, Edge_B;

		float Inv;

		UV_A = Texture_Coordinates[1] - Texture_Coordinates[0];
		UV_B = Texture_Coordinates[2] - Texture_Coordinates[0];

		Edge_A = Positions[1] - Positions[0];
		Edge_B = Positions[2] - Positions[0];

		Inv = 1.0f / (UV_A.x * UV_B.y - UV_B.x * UV_A.y);

		*Tangent = -glm::normalize(Inv * (Edge_A * UV_B.y - Edge_B * UV_A.y));
		*Bitangent = glm::normalize(glm::cross(*Tangent, Normal));
	}
}

#endif