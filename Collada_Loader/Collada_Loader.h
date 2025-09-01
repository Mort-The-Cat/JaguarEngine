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

std::string Load_File_Contents(std::string Filename);

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
		//uint32_t Joint_ID;
		float Joint_Weight;
	};

	class Collada_Mesh
	{
	public:
		std::vector<Collada_Vertex> Vertices; // I've decided to NOT use an index buffer for now

		// std::vector<uint32_t> Triangle_Indices; // These are the indices of each vertex of each triangle (stored as 32 bits)
	};

	struct Collada_Joint
	{
		glm::vec4 Offset_Matrix;	// This is the matrix that transforms from vertex space to bone space
		size_t Parent_Joint;
		std::vector<size_t> Child_Joints;



		glm::vec4 Final_Matrix;		// This is the final transformation matrix when updating the animation of a bone (likely will be changed later)
	};

	struct Collada_Vertex_Weight
	{
		size_t Joint_Index;
		float Weight;
	};

	class Collada_Skeleton
	{
	public:
		std::vector<Collada_Joint> Joints;

		std::vector<std::vector<Collada_Vertex_Weight>> Vertex_Weights; // Every vertex has a collection of vertex weights std::vector<Collada_Vertex_Weight>

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

	int Load_XML_Document(std::string Filename, XML_Document* Target_Document);

	int Load_Mesh(const XML_Document& Document, Collada_Mesh* Target_Mesh);
	int Load_Skeleton(const XML_Document& Document, Collada_Skeleton* Target_Skeleton);
}

#endif