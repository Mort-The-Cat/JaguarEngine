#include "Lightmapping.h"
#include "../OpenGL_Handling/Render_Queue.h"
#include "../Controllers/Jaguar_Engine_Wrapper.h"

#include "Lightmap_Chart_Rasteriser.h"

#include<vector>
#include<algorithm>
#include<set>

// https://math.stackexchange.com/questions/4902812/uniformly-sample-a-point-in-a-triangle-1-0-0-0-1-0-0-0-1#:~:text=A%20direct%20way%20to%20get%20a%20uniform%20distribution,where%20%24r_1%2Cr_2%2Cr_3%24%20are%20uniformly%20distributed%20on%20%24%20%5B0%2C1%5D%24.

// https://steamcdn-a.akamaihd.net/apps/valve/2004/GDC2004_Half-Life2_Shading.pdf

namespace Jaguar
{
	void Delete_Scene_Lightmap(Lighting_Data* Target_Lighting)
	{
		// Lightmap_Texture needs to be deleted

#if TRIPLE_LIGHTMAPPING
		for (size_t W = 0; W < 3; W++)
		{
			glBindTexture(GL_TEXTURE_2D, Target_Lighting->Lightmap_Textures[W].Texture_Buffer_ID);
			Destroy_Texture_Buffer(&Target_Lighting->Lightmap_Textures[W]);
		}
#else
		glBindTexture(GL_TEXTURE_2D, Target_Lighting->Lightmap_Texture.Texture_Buffer_ID);
		Destroy_Texture_Buffer(&Target_Lighting->Lightmap_Texture);
#endif

	}

	struct Rasterise_Tri_Lightmap_Data
	{
		glm::vec3** Lightmap_Texture_Data;
		const std::vector<Lightsource*>* Lightsources;
		glm::vec3 Normal, Tangent, Bitangent;
		Jaguar_Engine* Engine;
		Lightmap_Chart* Target_Chart;
	};

	float Sawtooth(float Value)
	{
		return 1.0f - std::fabsf(1.0f - std::fmodf(Value, 1.0f));
	}

	template<typename Format>
	glm::vec3 Read_From_Texture(const void* Pixel_Data, size_t Texture_Width, size_t Texture_Height, glm::vec2 UV)
	{
		// Don't bother with interpolation or mipmaps

		size_t X = Sawtooth(UV.x) * Texture_Width;
		size_t Y = Sawtooth(UV.y) * Texture_Height;

		/*struct RGBA
		{
			unsigned R, G, B, A;
		};*/

		Format* Pixels = (Format*)Pixel_Data;

		Format Pixel = Pixels[X + Y * Texture_Width];

		return glm::vec3(Pixel.X, Pixel.Y, Pixel.Z);
	}

	float Lightmap_Simple_Area_Of_Triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C)
	{
		glm::vec3 A_B = B - A;
		glm::vec3 A_C = C - A;

		return glm::length(glm::cross(A_B, A_C));
	}

	bool Line_Intersects_Tri(Rasterise_Tri_Lightmap_Data* Data, glm::vec3 Position, glm::vec3 To_Light_Vector, size_t Tri)
	{
		// Get tri transformed points
		// Get tri normal

		// Transform Position/To_Light_Vector into TBN space

		// Check if there is an overlap against the Z axis
		// If there is, check if within triangle at that point

		glm::vec3 Points[3];
		for (size_t W = 0; W < 3; W++)
		{
			Points[W] = Get_Model_Matrix(Data->Target_Chart->Pushed_Objects[Data->Target_Chart->Pushed_Tris[Tri].Model_Index]) *
				glm::vec4(
					Data->Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Data->Target_Chart->Pushed_Tris[Tri].Index + W].Position, 1);
		}

		glm::vec3 A_B = Points[1] - Points[0];
		glm::vec3 A_C = Points[2] - Points[0];
		glm::vec3 Normal = glm::normalize(glm::cross(A_B, A_C)); // It doesn't actually matter if this is facing the right way

		Position -= Points[0];

		glm::vec3 T_Position = Position;
		glm::vec3 T_To_Light_Vector = To_Light_Vector;

		T_Position.z = glm::dot(Position, Normal);
		T_To_Light_Vector.z = glm::dot(To_Light_Vector, Normal);

		if ((T_Position.z < 0) == (T_To_Light_Vector.z + T_Position.z < 0))	// If they don't overlap the Z axis
			return false;													// there isn't an intersect to test

		float Factor = -T_Position.z / T_To_Light_Vector.z;

		if (Factor < 0 || Factor > 1)
			return false;

		// Use the area-method to check if the point lies within the triangle

		T_Position = Position + To_Light_Vector * Factor;

		float Area[4] = {
			Lightmap_Simple_Area_Of_Triangle(glm::vec3(0.0f), A_B, A_C),

			Lightmap_Simple_Area_Of_Triangle(T_Position, A_B, A_C),
			Lightmap_Simple_Area_Of_Triangle(T_Position, A_C, glm::vec3(0.0f)),
			Lightmap_Simple_Area_Of_Triangle(T_Position, A_B, glm::vec3(0.0f))
		};

		return Area[1] + Area[2] + Area[3] <= Area[0] + 0.002f;
	}

	void Get_Lights_Visibility(Rasterise_Tri_Lightmap_Data* Data, glm::vec3 Position, glm::vec3* Colours, const std::vector<Lightsource*>& Lightsources, glm::vec3* Vector_Components)
	{
		for (size_t W = 0; W < Lightsources.size(); W++)
		{
			glm::vec3 To_Light_Vector = Lightsources[W]->Position - Position;

			if (cosf(Lightsources[W]->FOV * 3.14159f / 360.0f) > glm::dot(Lightsources[W]->Direction, glm::normalize(-To_Light_Vector)))
				continue;	// We're not within this light's FOV

			// This will not be a normalized vector because we need this as a line-segment vector
			// Iterate through every tri and test for intersection
			// (slow process but shouldn't be too bad)

			bool Intersect_Found = false;

			for (size_t Tri = 0; Tri < Data->Target_Chart->Pushed_Tris.size(); Tri++)
				Intersect_Found |= Line_Intersects_Tri(Data, Position, To_Light_Vector, Tri);

			if (!Intersect_Found)
			{
				// Add colours!

				float To_Light_Vector_Length = glm::inversesqrt(glm::dot(To_Light_Vector, To_Light_Vector));
				To_Light_Vector_Length *= To_Light_Vector_Length;	// Apply basic distance fall-off

				for (size_t V = 0; V < 3; V++)
				{
					Colours[V] += Lightsources[W]->Colour * (To_Light_Vector_Length * glm::max(0.0f, glm::dot(To_Light_Vector, Vector_Components[V])));
				}
			}
			//else
			//{
			//	for(size_t V = 0;V < 3; V++)
			//		Colours[V] += Lightsources[W]->Colour * (float)(glm::dot(To_Light_Vector, Vector_Components[V]) / (glm::pow(glm::length(To_Light_Vector), 2)));
			//}
		}
	}

	bool Perpixel_Rasterise_Tri_Lightmap(size_t X, size_t Y, glm::vec3 Position, void* Datap)
	{
		// This is called every pixel

		Rasterise_Tri_Lightmap_Data* Data = (Rasterise_Tri_Lightmap_Data*)Datap;

		glm::vec3 Colours[3] = { glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) };

		glm::vec3 Vector_Components[3];

		Jaguar::Get_Triple_Lightmap_Vectors(Data->Normal, Data->Tangent, Data->Bitangent, Vector_Components); // gets 3 components necessary for normal mapping

		// For each light in the scene

		Get_Lights_Visibility(Data, Position, Colours, *Data->Lightsources, Vector_Components);

		// Then, we write the colours to the lightmaps

		size_t Index = Y * Data->Target_Chart->Sidelength + X;

		for (size_t W = 0; W < 3; W++)
			Data->Lightmap_Texture_Data[W][Index] = Colours[W];

		return false;	// No "hit" - just continue as normal
	}

	void Rasterise_Tri_Lightmap3(Jaguar_Engine* Engine, size_t Tri, Lightmap_Chart* Target_Chart, glm::vec3* Lightmap_Texture_Data[3], const std::vector<Lightsource*>& Lightsources)
	{
		// First, we wanna calculate the normals etc of this tri

		glm::vec3 Points[3];
		glm::vec2 Texture_Coordinates[3];

		for (size_t Point = 0; Point < 3; Point++)
		{
			Points[Point] = Get_Model_Matrix(Target_Chart->Pushed_Objects[Target_Chart->Pushed_Tris[Tri].Model_Index]) * glm::vec4(Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index + Point].Position, 1);

			Texture_Coordinates[Point] = Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index + Point].Texture_Coordinates;
		}

		Rasterise_Tri_Lightmap_Data Data;

		Data.Tangent = Points[1] - Points[0];
		Data.Bitangent = Points[2] - Points[0];
		Data.Normal = glm::normalize(glm::cross(Data.Bitangent, Data.Tangent));

		Jaguar::Get_UV_Tangent_Bitangent_Vectors(Points, Texture_Coordinates, Data.Normal, &Data.Tangent, &Data.Bitangent);

		Data.Engine = Engine;
		Data.Target_Chart = Target_Chart;
		Data.Lightmap_Texture_Data = Lightmap_Texture_Data;

		Data.Lightsources = &Lightsources;

		// Then we rasterise this as a triangle to the lightmap texture

		Lightmap_Chart_Rasterise_Function<true, glm::vec3, Perpixel_Rasterise_Tri_Lightmap>(
			Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index].Lightmap_UV,
			Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index + 1].Lightmap_UV,
			Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index + 2].Lightmap_UV,
			Points[0] + 0.005f * Data.Normal,
			Points[1] + 0.005f * Data.Normal,
			Points[2] + 0.005f * Data.Normal,
			Target_Chart->Sidelength,
			&Data
		);
	}

	const float Luxel_Scale = 50.0f; // 1 unit squared equals 5x5 pixels of area

	void Generate_Bounced_Light_Lightsources(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, glm::vec3* Lightmap_Texture_Data3[3], std::vector<Lightsource*>& Target_Lightsources)
	{
		// we want a specific value for the resolution of the lights generated i.e. how many lights per face

		const float Scale = 30.0f;

		for (size_t W = 0; W < Target_Chart->Pushed_Tris.size(); W++)
		{
			float Iterator = Luxel_Scale / (float)(Scale * (float)Target_Chart->Pushed_Tris[W].Size);

			for(float R0 = 0.5f * Iterator; R0 < 1.0f; R0 += Iterator)
				for(float R1 = 0.5f * Iterator; R1 < 1.0f; R1 += Iterator)
				{
					Target_Lightsources.push_back(new Lightsource());

					glm::vec3 Points[3];

					glm::vec2 Texture_Coordinates[3];

					glm::vec2 Lightmap_UV[3];

					for (size_t Point = 0; Point < 3; Point++)
					{
						Points[Point] = Get_Model_Matrix(Target_Chart->Pushed_Objects[Target_Chart->Pushed_Tris[W].Model_Index]) * glm::vec4(Target_Chart->Pushed_Tris[W].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[W].Index + Point].Position, 1);
						
						Texture_Coordinates[Point] = Target_Chart->Pushed_Tris[W].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[W].Index + Point].Texture_Coordinates;
							
						Lightmap_UV[Point] = Target_Chart->Pushed_Tris[W].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[W].Index + Point].Lightmap_UV;
					}

					glm::vec3 Tangent = Points[1] - Points[0];
					glm::vec3 Bitangent = Points[2] - Points[0];
					glm::vec3 Normal = glm::normalize(glm::cross(Bitangent, Tangent));

					glm::vec3 Position;
					glm::vec2 Texture_Coordinate;
					glm::vec2 Lightmap_Coordinate;

					Position = 
						(1.0f - sqrtf(R0)) * Points[0] +
						sqrtf(R0) * R1 * Points[1] + 
						sqrtf(R0) * (1 - R1) * Points[2];

					Texture_Coordinate =
						(1.0f - sqrtf(R0)) * Texture_Coordinates[0] +
						sqrtf(R0) * R1 * Texture_Coordinates[1] +
						sqrtf(R0) * (1 - R1) * Texture_Coordinates[2];

					// Texture_Coordinate.y *= -1;

					Lightmap_Coordinate =
						(1.0f - sqrtf(R0)) * Lightmap_UV[0] +
						sqrtf(R0) * R1 * Lightmap_UV[1] +
						sqrtf(R0) * (1 - R1) * Lightmap_UV[2];

					Lightmap_Coordinate /= glm::vec2(Target_Chart->Sidelength);

					Target_Lightsources.back()->Position = Position + 0.005f * Normal;

					Target_Lightsources.back()->Direction = Normal;
					Target_Lightsources.back()->FOV = 179.0f;

					Jaguar::Texture_Cache_Info Texture_Info = Jaguar::Get_Texture_From_Buffer_ID(&Engine->Asset_Cache, Target_Chart->Pushed_Objects[Target_Chart->Pushed_Tris[W].Model_Index]->Albedo.Texture_Buffer_ID);

					glm::vec3 Albedo_Colour;
					glm::vec3 Lightmap_Value;

					struct RGB
					{
						uint8_t X, Y, Z, A;	// Typical textures are 4-channel images
					};

					struct Lightmap_RGB		// The lightmap is only 3 channel but they're 32-bit floats each
					{
						float X, Y, Z;
					};

					Albedo_Colour = Read_From_Texture<RGB>(Texture_Info.Pixel_Data, Texture_Info.Width, Texture_Info.Height, Texture_Coordinate);

					Lightmap_Value =
						Read_From_Texture<Lightmap_RGB>(Lightmap_Texture_Data3[0], Target_Chart->Sidelength, Target_Chart->Sidelength, Lightmap_Coordinate) +
						Read_From_Texture<Lightmap_RGB>(Lightmap_Texture_Data3[1], Target_Chart->Sidelength, Target_Chart->Sidelength, Lightmap_Coordinate) +
						Read_From_Texture<Lightmap_RGB>(Lightmap_Texture_Data3[2], Target_Chart->Sidelength, Target_Chart->Sidelength, Lightmap_Coordinate);

					float Reflection_Coefficient = 0.07f / (255.0f * Scale);

					Target_Lightsources.back()->Colour = Lightmap_Value * Albedo_Colour * glm::vec3(Reflection_Coefficient); // This will then rewrite the lightmap accordingly
				}
		}
	}

	void Handle_Bounce_Lighting(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, glm::vec3* Lightmap_Texture_Data[3])
	{
		// This will modify the values in Lightmap_Texture_Data according to the bounce lighting

		glm::vec3* Lightmap_Bounce_Data[3];
		for (size_t W = 0; W < 3; W++)
		{
			Lightmap_Bounce_Data[W] = new glm::vec3[Target_Chart->Sidelength * Target_Chart->Sidelength];
			for (size_t V = 0; V < Target_Chart->Sidelength * Target_Chart->Sidelength; V++)
				Lightmap_Bounce_Data[W][V] = glm::vec3(0.0f);
		}

		// This just initialises another buffer for the lightmap data to fill up before we add the new values

		std::vector<Lightsource*> Bounce_Lightsources;

		Generate_Bounced_Light_Lightsources(Engine, Target_Chart, Lightmap_Texture_Data, Bounce_Lightsources);

		for (size_t W = 0; W < Target_Chart->Pushed_Tris.size(); W++)
		{
			Rasterise_Tri_Lightmap3(Engine, W, Target_Chart, Lightmap_Bounce_Data, Bounce_Lightsources);

			printf("Bounce pass tri %d complete\n", W);
		}

		for (size_t W = 0; W < 3; W++)
		{
			for (size_t V = 0; V < Target_Chart->Sidelength * Target_Chart->Sidelength; V++)
				Lightmap_Texture_Data[W][V] += Lightmap_Bounce_Data[W][V];

			delete Lightmap_Bounce_Data[W];
		}

		for (size_t W = 0; W < Bounce_Lightsources.size(); W++)
			delete Bounce_Lightsources[W];

		// remember to deallocate these lightsources at the end of the function
	}

	void Create_Lightmap3_From_Chart(Jaguar_Engine* Engine, Lightmap_Chart* Target_Chart, const char* Filename)
	{
		// This will rasterise each tri to the lightmap texture, applying the according normals and raycast equation to determine if the line intersects something else

		glm::vec3* Lightmap_Texture_Data[3];
		for (size_t W = 0; W < 3; W++)
		{
			Lightmap_Texture_Data[W] = new glm::vec3[Target_Chart->Sidelength * Target_Chart->Sidelength];
			for (size_t V = 0; V < Target_Chart->Sidelength * Target_Chart->Sidelength; V++)
				Lightmap_Texture_Data[W][V] = glm::vec3(0.0f);
		}

		// We'll generate and rasterise the stuff here

		for (size_t W = 0; W < Target_Chart->Pushed_Tris.size(); W++)
		{
			// Apply model matrix to each triangle when doing raycast calculations

			Rasterise_Tri_Lightmap3(Engine, W, Target_Chart, Lightmap_Texture_Data, Engine->Scene.Lighting.Lightsources);

			printf("Tri %d complete\n", W);
		}

		Handle_Bounce_Lighting(Engine, Target_Chart, Lightmap_Texture_Data);

		Write_Lightmap3_To_File(Filename, Lightmap_Texture_Data, Target_Chart->Sidelength);

		delete Lightmap_Texture_Data[0];
		delete Lightmap_Texture_Data[1];
		delete Lightmap_Texture_Data[2];
		// free the allocated memory
	}


	void Init_Lightmap_Chart(Lightmap_Chart* Target_Chart)
	{
		Target_Chart->Sidelength = 1u;
		Target_Chart->Occupied.resize(1);
		Target_Chart->Occupied[0] = { 0 }; // Not occupied
	}

	void Upsize_Chart(Lightmap_Chart* Target_Chart)
	{
		Target_Chart->Sidelength += 8; // Doubles sidelength
		Target_Chart->Occupied.resize(Target_Chart->Sidelength);

		for (size_t W = 0; W < Target_Chart->Sidelength; W++)
			Target_Chart->Occupied[W].resize(Target_Chart->Sidelength);
	}

	bool Perpixel_Rasterise_Check(size_t X, size_t Y, int, void* Data)
	{
		const Lightmap_Chart* Target_Chart = (const Lightmap_Chart*)Data;

		if (Target_Chart->Occupied[X][Y])
			return true;					// Hit!!

		return false;						// No hit
	}

	bool Perpixel_Rasterise_Fill(size_t X, size_t Y, int, void* Data)
	{
		Lightmap_Chart* Target_Chart = (Lightmap_Chart*)Data;

		Target_Chart->Occupied[X][Y] = true;

		return false;	// No hit, so continue as normal!
	}

	bool Find_New_Location_Lightmap_Chart(int Area, int* X, int* Y, const Lightmap_Chart* Target_Chart, glm::vec2* Projected_Points)
	{
		// Find area closest to top-left to place square

		unsigned int Max = Target_Chart->Sidelength - LIGHTMAP_CHART_PADDING;

		int Begin_X = LIGHTMAP_CHART_PADDING;
		int Begin_Y = LIGHTMAP_CHART_PADDING;

		while (Begin_Y < Target_Chart->Sidelength)
		{
			*X = Begin_X;
			*Y = Begin_Y;

			while (*X >= LIGHTMAP_CHART_PADDING && *Y < Max)
			{
				if (Lightmap_Chart_Rasterise_Function<false, int, Perpixel_Rasterise_Check>(
					Projected_Points[0] + glm::vec2(*X, *Y),
					Projected_Points[1] + glm::vec2(*X, *Y),
					Projected_Points[2] + glm::vec2(*X, *Y),
					0, 0, 0, Target_Chart->Sidelength, (void*)Target_Chart
				))
					//if (Check_Chart_Square_Area(Area, *X, *Y, Target_Chart))
					return true;

				(*X)--;
				(*Y)++;
			}

			bool Condition = Begin_X < Max;
			Begin_X += Condition;
			Begin_Y += !Condition;
		}

		return false;
	}

	bool Lightmap_Tri_Sort_Compare(const Lightmap_Tri& A, const Lightmap_Tri& B)
	{
		return A.Size > B.Size;
	}

	void Lightmap_Chart_Get_Projection(Collada::Collada_Mesh* Mesh, size_t Index, glm::vec2* Projected_Points, float Scale)
	{
		glm::vec3 A_B = Mesh->Vertices[Index + 1].Position - Mesh->Vertices[Index].Position;
		glm::vec3 A_C = Mesh->Vertices[Index + 2].Position - Mesh->Vertices[Index].Position;

		glm::vec3 Tangent = glm::normalize(A_B);
		glm::vec3 Bitangent = glm::normalize(A_C);

		glm::vec3 Normal = glm::normalize(glm::cross(Tangent, Bitangent));

		Bitangent = -glm::normalize(glm::cross(Tangent, Normal));

		Projected_Points[1] = glm::vec2(glm::ceil(Scale * glm::dot(Tangent, A_B)), 0.0f);
		Projected_Points[2] = glm::vec2(
			glm::ceil(Scale * glm::dot(Tangent, A_C)),
			glm::ceil(Scale * glm::dot(Bitangent, A_C))
		);
	}

	void Assemble_Lightmap_Chart(Lightmap_Chart* Target_Chart)
	{
		std::sort(Target_Chart->Pushed_Tris.begin(), Target_Chart->Pushed_Tris.end(), Lightmap_Tri_Sort_Compare); // Sorts them accordingly

		// This is because we want to place the largest tris first and fit the smaller ones between them afterwards wherever possible

		std::set<Mesh_Cache_Info> Updated_Meshes;

		glm::vec2 Projected_Points[3];
		Projected_Points[0] = glm::vec2(0.0f);

		for (size_t Tri = 0; Tri < Target_Chart->Pushed_Tris.size(); Tri++)
		{
			//int Square_Size = Target_Chart->Pushed_Tris[Tri].Size;
			Mesh_Cache_Info Mesh_Info = Target_Chart->Pushed_Tris[Tri].Mesh;

			Updated_Meshes.insert(Mesh_Info);

			size_t Triangle = Target_Chart->Pushed_Tris[Tri].Index;

			int X, Y;

			Lightmap_Chart_Get_Projection(Mesh_Info.Mesh, Triangle, Projected_Points, Luxel_Scale);

			while (!Find_New_Location_Lightmap_Chart(0, &X, &Y, Target_Chart, Projected_Points))
				Upsize_Chart(Target_Chart); // Doubles size until there's room somewhere on chart

			Lightmap_Chart_Rasterise_Function<false, int, Perpixel_Rasterise_Fill>(
				Projected_Points[0] + glm::vec2(X, Y),
				Projected_Points[1] + glm::vec2(X, Y),
				Projected_Points[2] + glm::vec2(X, Y),
				0, 0, 0,
				Target_Chart->Sidelength,
				Target_Chart
			);

			for (size_t Point = 0; Point < 3; Point++)
				Mesh_Info.Mesh->Vertices[Triangle++].Lightmap_UV = glm::vec2(X, Y) + Projected_Points[Point];
		}

		for (auto Mesh : Updated_Meshes)											// Updates all of the meshes that need to be updated once
		{
			Bind_Vertex_Buffer(Mesh.Buffer);
			Update_Vertex_Buffer_Data(Mesh.Mesh, &Mesh.Buffer);
		}
	}

	void Push_Queue_Lightmap_Chart(Jaguar_Engine* Engine, const Render_Queue* Queue, Lightmap_Chart* Target_Chart)
	{
		// This will push all of the elements in a specific render queue to a lightmap chart
		// This chart will be used to map model triangles to areas of the lightmap, scaling it dynamically.

		// From there, other functions will be called to create and fill a corresponding texture via a renderbuffer

		// This uses a fancier method than before, fitting accurately projected and scaled triangles onto the chart

		for (size_t Object = 0; Object < Queue->Objects.size(); Object++)
		{
			Target_Chart->Pushed_Objects.push_back(Queue->Objects[Object]);

			Mesh_Cache_Info Mesh_Info = Get_Mesh_From_Buffer_ID(&Engine->Asset_Cache, Queue->Objects[Object]->Mesh.Vertex_Buffer_ID);

			for (size_t Triangle = 0; Triangle < Mesh_Info.Mesh->Vertices.size(); Triangle += 3) // Every 3 verts is a tri
			{
				// Get tri area

				glm::vec3 Points[3];

				for (size_t Vert = 0; Vert < 3; Vert++)
					Points[Vert] = Mesh_Info.Mesh->Vertices[Vert + Triangle].Position;

				Points[1] -= Points[0];
				Points[2] -= Points[0];

				float Square_Size = std::sqrtf(
					Luxel_Scale * glm::length(
						glm::cross(Points[1], Points[2])
					)
				);

				Target_Chart->Pushed_Tris.push_back(Lightmap_Tri(Mesh_Info, Triangle, Square_Size, Target_Chart->Pushed_Objects.size() - 1));

				// Square_Size is just the rough size of the tri here
			}
		}
	}
}