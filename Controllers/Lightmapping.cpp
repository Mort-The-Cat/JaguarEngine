#include "Lightmapping.h"
#include "../OpenGL_Handling/Render_Queue.h"
#include "../Controllers/Jaguar_Engine_Wrapper.h"

#include "Lightmap_Chart_Rasteriser.h"

#include<vector>
#include<algorithm>
#include<set>

namespace Jaguar
{
	void Delete_Scene_Lightmap(Lighting_Data* Target_Lighting)
	{
		// Lightmap_Texture needs to be deleted

		glBindTexture(GL_TEXTURE_2D, Target_Lighting->Lightmap_Texture.Texture_Buffer_ID);
		Destroy_Texture_Buffer(&Target_Lighting->Lightmap_Texture);
	}

	glm::vec3 Render_Scene_To_Lightmap_Pixel(Jaguar_Engine* Engine, const Lightmap_Chart* Target_Chart, int Framebuffer, int Depth_Renderbuffer, int Incident_Texture, const Vertex_Buffer& Light_Model, glm::mat4 Projection_Matrices[5], glm::vec3 Position, glm::vec3 Normal, Shader* Lightmap_Shader, unsigned int Incident_Texture_Width, glm::vec3* Pixel_Data) // Returns average pixel value
	{
		glm::vec3 Pixel_Colour = glm::vec3(0.0);

		// Iterates each projection face
			// Sets up uniforms
			// Renders all scene objects
			// Renders all lightsources
			// 
			// Sums up pixel colours
			// Clears buffers to render again

		// Averages out pixel colour

		// returns
		// ---------------

		glClearColor(0.f, 0.f, 0.f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		size_t Pixel_Count = 6 * Incident_Texture_Width * Incident_Texture_Width;

		for (size_t Face = 0; Face < 6; Face++)
		{
			//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

			glViewport(Face * Incident_Texture_Width, 0, Incident_Texture_Width, Incident_Texture_Width);

			glUniformMatrix4fv(glGetUniformLocation(Lightmap_Shader->Program_ID, "Projection_Matrix"), 1, GL_FALSE, glm::value_ptr(Projection_Matrices[Face]));

			glUniform3f(glGetUniformLocation(Lightmap_Shader->Program_ID, "Camera_Position"), Position.x, Position.y, Position.z);

			glUniform3f(glGetUniformLocation(Lightmap_Shader->Program_ID, "Lightmap_Surface_Normal"), Normal.x, Normal.y, Normal.z);

			Bind_Vertex_Buffer(Light_Model);

			glDepthMask(GL_FALSE);

			if constexpr (true) // We want to draw the sun
			{
				glm::mat4 Sun_Matrix = glm::scale(glm::translate(Position), glm::vec3(1.0f));

				glUniformMatrix4fv(glGetUniformLocation(Lightmap_Shader->Program_ID, "Model_Matrix"), 1, GL_FALSE, glm::value_ptr(Sun_Matrix));
				glUniform3f(glGetUniformLocation(Lightmap_Shader->Program_ID, "Light_Colour"),
					3 * 0.85f,
					3 * 0.95f,
					3 * 1.00f
				);

				glDrawArrays(GL_TRIANGLES, 0, Light_Model.Vertex_Count);
			}

			glDepthMask(GL_TRUE);

			for (size_t Light = 0; Light < Engine->Scene.Lighting.Lightsources.size(); Light++)
			{
				glm::mat4 Model_Matrix = glm::scale(glm::translate(Engine->Scene.Lighting.Lightsources[Light]->Position), glm::vec3(Engine->Scene.Lighting.Lightsources[Light]->Radius));

				// glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(Engine->Scene.Lighting.Lightsources[Light]->Radius)), Engine->Scene.Lighting.Lightsources[Light]->Position);

				glUniformMatrix4fv(glGetUniformLocation(Lightmap_Shader->Program_ID, "Model_Matrix"), 1, GL_FALSE, glm::value_ptr(Model_Matrix));
				glUniform3f(glGetUniformLocation(Lightmap_Shader->Program_ID, "Light_Colour"),
					Engine->Scene.Lighting.Lightsources[Light]->Colour.x,
					Engine->Scene.Lighting.Lightsources[Light]->Colour.y,
					Engine->Scene.Lighting.Lightsources[Light]->Colour.z);

				glDrawArrays(GL_TRIANGLES, 0, Light_Model.Vertex_Count);
			}

			glUniform3f(glGetUniformLocation(Lightmap_Shader->Program_ID, "Light_Colour"), 0, 0, 0);

			for (size_t Object = 0; Object < Target_Chart->Pushed_Objects.size(); Object++)
			{
				// Iterates through objects

				// Default_Uniform_Assign_Function(Lightmap_Shader, Target_Chart->Pushed_Objects[Object], &Engine->Scene); // Assigns uniforms 

				glm::mat4 Model_Matrix = Get_Model_Matrix(Target_Chart->Pushed_Objects[Object]);

				glUniformMatrix4fv(glGetUniformLocation(Lightmap_Shader->Program_ID, "Model_Matrix"), 1, GL_FALSE, glm::value_ptr(Model_Matrix));

				Bind_Vertex_Buffer(Target_Chart->Pushed_Objects[Object]->Mesh);

				glDrawArrays(GL_TRIANGLES, 0, Target_Chart->Pushed_Objects[Object]->Mesh.Vertex_Count);
			}

			// then renders lightsources

			glfwSwapBuffers(Engine->Window);
			glfwPollEvents();

			// Now, we sum the pixel data

			// glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// glBindFramebuffer(GL_FRAMEBUFFER, *Framebuffer);

			//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		}

		glBindTexture(GL_TEXTURE_2D, Incident_Texture);

		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, Pixel_Data);

		for (size_t Pixel = 0; Pixel < Pixel_Count; Pixel++)	// Now, we pack all 6 views into one texture and just change the viewport position accordingly
			Pixel_Colour += Pixel_Data[Pixel];

		Pixel_Colour /= 5.5f;	// 5 faces
		Pixel_Colour /= (float)(Incident_Texture_Width * Incident_Texture_Width); // pixel count

		return Pixel_Colour;
	}

	struct Rasterise_Tri_Lightmap_Data
	{
		glm::mat4* Projection_Matrices;
		glm::mat4 Perspective;
		glm::mat4 View_Matrix;

		Vertex_Buffer Light_Model;
		glm::vec3 Normal;

		Jaguar_Engine* Engine;
		Lightmap_Chart* Target_Chart;
		int Framebuffer;
		int Depth_Renderbuffer;
		int Incident_Texture;
		unsigned int Incident_Texture_Width;
		Shader* Lightmap_Shader;
		glm::vec3* Lightmap_Texture_Data;
		glm::vec3* Pixel_Data;
	};

	bool Perpixel_Rasterise_Tri_Lightmap(size_t X, size_t Y, glm::vec3 Position, void* Datap)
	{
		Rasterise_Tri_Lightmap_Data Data = *(Rasterise_Tri_Lightmap_Data*)Datap;

		// Generate projection matrices

		// Call render function

		// Get index

		// Write index

		// "Position" is interpolated by the rasteriser function

		glm::mat4 Local_Projection_Matrices[6];

		Position = Position + Data.Normal * glm::vec3(0.005f); //Get_Model_Matrix(Data.Target_Chart->Pushed_Objects[0]) * glm::vec4(Position + Data.Normal * glm::vec3(0.001), 1);

		// glm::mat4 Local_View = Data.View_Matrix; // glm::translate(View_Matrix, -Position);

		for (size_t Face = 0; Face < 6; Face++)
			Local_Projection_Matrices[Face] = Data.Perspective * glm::translate(Data.Projection_Matrices[Face], -Position);

		glm::vec3 Colour;
		
		// Position / glm::vec3(2);
		
		if constexpr (false)
		{
			Colour = 
			glm::vec3(
				glm::max(0.0f, glm::dot(glm::normalize(Position - Data.Engine->Scene.Lighting.Lightsources[0]->Position), -Data.Normal))
			);
		}
		else
		{
			Colour = Render_Scene_To_Lightmap_Pixel(Data.Engine, Data.Target_Chart, Data.Framebuffer, Data.Depth_Renderbuffer, Data.Incident_Texture, Data.Light_Model, Local_Projection_Matrices, Position, Data.Normal, Data.Lightmap_Shader, Data.Incident_Texture_Width, Data.Pixel_Data);
		}

		size_t Index = X + Y * Data.Target_Chart->Sidelength;

		Data.Lightmap_Texture_Data[Index] = Colour; // Sets colour

		return false;	// No hit, just continue as normal
	}

	void Rasterise_Tri_Lightmap(Jaguar_Engine* Engine, size_t Tri, Lightmap_Chart* Target_Chart, int Framebuffer, int Depth_Renderbuffer, int Incident_Texture, unsigned int Incident_Texture_Width, glm::vec3* Lightmap_Texture_Data, Shader* Lightmap_Shader, glm::vec3* Pixel_Data)
	{
		Vertex_Buffer Light_Model = Pull_Mesh(&Engine->Asset_Cache, "Collada_Loader/Sphere.dae", 0).Buffer;

		glm::vec3 Points[3];

		for (size_t Point = 0; Point < 3; Point++)
			Points[Point] = Get_Model_Matrix(Target_Chart->Pushed_Objects[0]) * glm::vec4(Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index + Point].Position, 1);

		glm::vec3 Tangent = glm::normalize(Points[1] - Points[0]);
		glm::vec3 Bitangent = glm::normalize(Points[2] - Points[0]);
		glm::vec3 Normal = glm::normalize(glm::cross(Bitangent, Tangent));

		Bitangent = glm::normalize(glm::cross(Tangent, Normal));

		glm::mat4 Projection_Matrices[6];

		glm::mat4 Perspective = glm::perspective(glm::radians(90.0f), 1.0f, 0.000001f, 100.0f);

		// glm::mat4 View_Matrix = glm::lookAt(glm::vec3(0.0f), Normal, Tangent);

		// Create all view matrices matrices

		// Make projection matrix for every direction

		Projection_Matrices[0] = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		Projection_Matrices[1] = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		Projection_Matrices[2] = glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		Projection_Matrices[3] = glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		Projection_Matrices[4] = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		Projection_Matrices[5] = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));


		Rasterise_Tri_Lightmap_Data Data; // This is messy but it's whatever

		Data.Light_Model = Light_Model;
		Data.Perspective = Perspective;
		//Data.View_Matrix = View_Matrix;
		Data.Projection_Matrices = Projection_Matrices;
		Data.Engine = Engine;
		Data.Target_Chart = Target_Chart;
		Data.Framebuffer = Framebuffer;
		Data.Depth_Renderbuffer = Depth_Renderbuffer;
		Data.Incident_Texture = Incident_Texture;
		Data.Incident_Texture_Width = Incident_Texture_Width;
		Data.Lightmap_Texture_Data = Lightmap_Texture_Data;
		Data.Lightmap_Shader = Lightmap_Shader;
		Data.Pixel_Data = Pixel_Data;
		Data.Normal = Normal;


		Lightmap_Chart_Rasterise_Function<true, glm::vec3, Perpixel_Rasterise_Tri_Lightmap>(
			Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index].Lightmap_UV,
			Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index + 1].Lightmap_UV,
			Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index + 2].Lightmap_UV,
			Points[0], Points[1], Points[2],
			Target_Chart->Sidelength,
			&Data
		);
	}

	void Create_Lightmap_From_Chart(Jaguar_Engine* Engine, Texture* Lightmap_Texture, Lightmap_Chart* Target_Chart, const char* File_Output)
	{
		// This will allocate buffer for lightmap texture data

		// (Note: Needs to create framebuffer and renderbuffer as well as texture for this process)
		// Also load and create shader for this rendering

		// Iterate through tris in chart
		// Iterate through pixels in tri
		// Begin the rendering of the incident light cubemaps
		// Get average of incident light across cubemap faces

		// Once complete, create lightmap texture and delete buffer

		// (Deallocate framebuffer and renderbuffer and cubemap texture)

		// ---------------------

		const unsigned int Incident_Texture_Width = 256; // renders at 256x256

		glm::vec3* Lightmap_Texture_Data = new glm::vec3[Target_Chart->Sidelength * Target_Chart->Sidelength]; // Sidelength x Sidelength

		for (size_t W = 0; W < Target_Chart->Sidelength * Target_Chart->Sidelength; W++)
			Lightmap_Texture_Data[W] = 0.0f * glm::vec3(1.0f, 0.0f, 1.0f);	// zeroes out values

		glm::vec3* Pixel_Data = new glm::vec3[Incident_Texture_Width * Incident_Texture_Width * 6]; // [Incident_Texture_Width * Incident_Texture_Width] ;

		// Change this to render all 6 views to one buffer

		// Note: I initially was going to render to a cubemap using the geometry shader, but I'll just render to each face separately

		unsigned int Framebuffer, Depth_Renderbuffer, Incident_Texture;
		glGenFramebuffers(1, &Framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);

		glGenTextures(1, &Incident_Texture);
		glBindTexture(GL_TEXTURE_2D, Incident_Texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Incident_Texture_Width * 6, Incident_Texture_Width, 0, GL_RGB, GL_FLOAT, Pixel_Data);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Incident_Texture, 0);

		glGenRenderbuffers(1, &Depth_Renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, Depth_Renderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Incident_Texture_Width * 6, Incident_Texture_Width);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Depth_Renderbuffer);

		Shader Lightmap_Shader;
		Create_Shader("Shaders/Produce_Lightmap_Shader.frag", "Shaders/Test_Shader.vert", &Lightmap_Shader);

		//

		Use_Shader(Lightmap_Shader);

		glViewport(0, 0, Incident_Texture_Width, Incident_Texture_Width);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glDisable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);

		for (size_t Tri = 0; Tri < Target_Chart->Pushed_Tris.size(); Tri++)
		{
			// Iterate every tri

			Rasterise_Tri_Lightmap(Engine, Tri, Target_Chart, Framebuffer, Depth_Renderbuffer, Incident_Texture, Incident_Texture_Width, Lightmap_Texture_Data, &Lightmap_Shader, Pixel_Data);
		
			printf("Tri %lu complete\n", Tri);
		}

		// Create_Texture_Buffer(Lightmap_Texture, GL_RGB32F, Target_Chart->Sidelength, Target_Chart->Sidelength, GL_RGB, GL_FLOAT, Lightmap_Texture_Data, true);

		printf(" >> Successfully created lightmap of %lu dimensions!\n", Target_Chart->Sidelength);

		if(File_Output)
			Write_Lightmap_To_File(File_Output, Lightmap_Texture_Data, Target_Chart->Sidelength);

		Engine->Scene.Lighting.Lightmap_Texture = *Lightmap_Texture;
		Engine->Scene.Lighting.Inverse_Lightmap_Scale = 1.0f / (float)Target_Chart->Sidelength;

		//

		Destroy_Shader(&Lightmap_Shader);

		glDeleteFramebuffers(1, &Framebuffer);
		glDeleteRenderbuffers(1, &Depth_Renderbuffer);
		glDeleteTextures(1, &Incident_Texture);

		delete[] Lightmap_Texture_Data;

		delete[] Pixel_Data;
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
				if(Lightmap_Chart_Rasterise_Function<false, int, Perpixel_Rasterise_Check>(
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

	const float Luxel_Scale = 10.0f; // 1 unit squared equals 5x5 pixels of area

	void Assemble_Lightmap_Chart(Lightmap_Chart* Target_Chart)
	{
		std::sort(Target_Chart->Pushed_Tris.begin(), Target_Chart->Pushed_Tris.end(), Lightmap_Tri_Sort_Compare); // Sorts them accordingly

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

			//Fill_Chart_Square_Area(Square_Size, X, Y, Target_Chart);

			for (size_t Point = 0; Point < 3; Point++)
				Mesh_Info.Mesh->Vertices[Triangle++].Lightmap_UV = glm::vec2(X, Y) + Projected_Points[Point];

			/*Mesh_Info.Mesh->Vertices[Triangle].Lightmap_UV.x = X;
			Mesh_Info.Mesh->Vertices[Triangle++].Lightmap_UV.y = Y;

			Mesh_Info.Mesh->Vertices[Triangle].Lightmap_UV.x = X + Square_Size;
			Mesh_Info.Mesh->Vertices[Triangle++].Lightmap_UV.y = Y;

			Mesh_Info.Mesh->Vertices[Triangle].Lightmap_UV.x = X;
			Mesh_Info.Mesh->Vertices[Triangle++].Lightmap_UV.y = Y + Square_Size;*/
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
		
		// This won't use any fancy algorithm for now, just square areas for each tri

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

				int Square_Size = std::ceilf(std::sqrtf(
					Luxel_Scale * glm::length(
						glm::cross(Points[1], Points[2])
					)
				));

				Target_Chart->Pushed_Tris.push_back(Lightmap_Tri(Mesh_Info, Triangle, Square_Size));

				// Square_Size is just the side-length of the square used here

				/*int X, Y;

				while (!Find_New_Location_Lightmap_Chart(Square_Size, &X, &Y, Target_Chart))
					Upsize_Chart(Target_Chart); // Doubles size until there's room somewhere on chart

				Fill_Chart_Square_Area(Square_Size, X, Y, Target_Chart);

				Mesh_Info.Mesh->Vertices[Triangle].Lightmap_UV.x = X;
				Mesh_Info.Mesh->Vertices[Triangle++].Lightmap_UV.y = Y;

				Mesh_Info.Mesh->Vertices[Triangle].Lightmap_UV.x = X + Square_Size;
				Mesh_Info.Mesh->Vertices[Triangle++].Lightmap_UV.y = Y;

				Mesh_Info.Mesh->Vertices[Triangle].Lightmap_UV.x = X;
				Mesh_Info.Mesh->Vertices[Triangle++].Lightmap_UV.y = Y + Square_Size;*/
			}

			// Update vertex buffer

			// Update_Vertex_Buffer_Data(Mesh_Info.Mesh, &Mesh_Info.Buffer);
		}
	}
}