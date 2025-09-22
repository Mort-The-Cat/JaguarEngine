#include "Lightmapping.h"
#include "../OpenGL_Handling/Render_Queue.h"
#include "../Controllers/Jaguar_Engine_Wrapper.h"

#include<vector>
#include<algorithm>
#include<set>

namespace Jaguar
{

	glm::vec3 Render_Scene_To_Lightmap_Pixel(Jaguar_Engine* Engine, const Lightmap_Chart* Target_Chart, const Vertex_Buffer& Light_Model, glm::mat4 Projection_Matrices[5], glm::vec3 Position, glm::vec3 Normal, Shader* Lightmap_Shader, unsigned int Incident_Texture_Width, glm::vec3* Pixel_Data) // Returns average pixel value
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

		size_t Pixel_Count = Incident_Texture_Width * Incident_Texture_Width;

		for (size_t Face = 0; Face < 5; Face++)
		{
			glUniformMatrix4fv(glGetUniformLocation(Lightmap_Shader->Program_ID, "Projection_Matrix"), 1, GL_FALSE, glm::value_ptr(Projection_Matrices[Face]));

			glUniform3f(glGetUniformLocation(Lightmap_Shader->Program_ID, "Camera_Position"), Position.x, Position.y, Position.z);

			glUniform3f(glGetUniformLocation(Lightmap_Shader->Program_ID, "Lightmap_Surface_Normal"), Normal.x, Normal.y, Normal.z);

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

			Bind_Vertex_Buffer(Light_Model);

			for (size_t Light = 0; Light < Engine->Scene.Lighting.Lightsources.size(); Light++)
			{
				glm::mat4 Model_Matrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(Engine->Scene.Lighting.Lightsources[Light]->Radius)), Engine->Scene.Lighting.Lightsources[Light]->Position);

				glUniformMatrix4fv(glGetUniformLocation(Lightmap_Shader->Program_ID, "Model_Matrix"), 1, GL_FALSE, glm::value_ptr(Model_Matrix));
				glUniform3f(glGetUniformLocation(Lightmap_Shader->Program_ID, "Light_Colour"),
					Engine->Scene.Lighting.Lightsources[Light]->Colour.x,
					Engine->Scene.Lighting.Lightsources[Light]->Colour.y,
					Engine->Scene.Lighting.Lightsources[Light]->Colour.z);

				glDrawArrays(GL_TRIANGLES, 0, Light_Model.Vertex_Count);
			}

			glfwSwapBuffers(Engine->Window);
			glfwPollEvents();

			// Now, we sum the pixel data

			// glBindFramebuffer(GL_FRAMEBUFFER, 0);

			//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, Pixel_Data);

			// The issue might be here

			glReadPixels(0, 0, Incident_Texture_Width, Incident_Texture_Width, GL_RGB, GL_FLOAT, Pixel_Data);

			for (size_t Pixel = 0; Pixel < Pixel_Count; Pixel++)
				Pixel_Colour += Pixel_Data[Pixel];

			// glBindFramebuffer(GL_FRAMEBUFFER, *Framebuffer);

			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			//glClearColor(0.75f, 0.65f, 0.85f, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		// delete[] Pixel_Data;	// DEALLOCATES PIXEL DATA (very important)

		Pixel_Colour /= 5.0f;	// 5 faces
		Pixel_Colour /= (float)(Incident_Texture_Width * Incident_Texture_Width); // pixel count

		return Pixel_Colour;
	}

	void Rasterise_Tri_Lightmap(Jaguar_Engine* Engine, size_t Tri, Lightmap_Chart* Target_Chart, unsigned int Incident_Texture_Width, glm::vec3* Lightmap_Texture_Data, Shader* Lightmap_Shader, glm::vec3* Pixel_Data)
	{
		// Get tri projection matrices
		// From normal/tangent/bitangent
		
		// Iterates through scanlines of tri
			// For each pixel, 
				// Interpolate to get current position
				// Render accordingly, accumulating pixel values
				// Set pixel colour on Lightmap_Texture_Data buffer
		// ------------------------

		Vertex_Buffer Light_Model = Pull_Mesh(&Engine->Asset_Cache, "Collada_Loader/Sphere.dae", 0).Buffer;

		glm::vec3 Points[3];

		for (size_t Point = 0; Point < 3; Point++)
			Points[Point] = Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index + Point].Position;

		glm::vec3 Tangent = glm::normalize(Points[1] - Points[0]);
		glm::vec3 Bitangent = Points[2] - Points[0];
		glm::vec3 Normal = glm::normalize(glm::cross(Tangent, Bitangent));
		// Bitangent = glm::cross(Tangent, Normal);

		glm::mat4 Projection_Matrices[5];

		glm::mat4 Perspective = glm::perspective(glm::radians(90.0f), 1.0f, 0.000001f, 100.0f);

		glm::mat4 View_Matrix = glm::lookAt(glm::vec3(0.0f), Normal, Tangent);

		// Equilateral triangle is easy to draw

		/*
		
		0,0_____1,0
		 |     /
		 |    /
		 |   /
		 |  /
		 | /
		 |/
		0,1

		*/

		glm::vec3 Left_Interpolation_Values, Right_Interpolation_Values;

		Left_Interpolation_Values.y = 0.0f;
		Right_Interpolation_Values.x = 0.0f;

		float Inverse_Scanline = 1.0f / (float)Target_Chart->Pushed_Tris[Tri].Size;

		for (int Scanline = -1; Scanline <= Target_Chart->Pushed_Tris[Tri].Size; Scanline++)
		{
			int Clamped_Scanline = std::max(std::min(Scanline, Target_Chart->Pushed_Tris[Tri].Size - 1), 0);
			Left_Interpolation_Values.x = Inverse_Scanline * (float)(Target_Chart->Pushed_Tris[Tri].Size - Clamped_Scanline);
			Left_Interpolation_Values.z = 1.0f - Left_Interpolation_Values.x;

			Right_Interpolation_Values.y = Left_Interpolation_Values.x;
			Right_Interpolation_Values.z = Left_Interpolation_Values.z;

			float Inverse_Size = 1.0f / (Target_Chart->Pushed_Tris[Tri].Size - Clamped_Scanline);

			for (int Pixels = -1; Pixels <= (Target_Chart->Pushed_Tris[Tri].Size - Clamped_Scanline); Pixels++)
			{
				int Clamped_Pixel = std::max(std::min(Pixels, (Target_Chart->Pushed_Tris[Tri].Size - Clamped_Scanline)) - 1, 0);
				float Factor = Inverse_Size * (float)Clamped_Pixel;
				glm::vec3 Interpolation_Values = Left_Interpolation_Values * glm::vec3(Factor) + Right_Interpolation_Values * glm::vec3(1.0f - Factor);

				// Init projection matrices per pixel

				glm::vec3 Position = (
					Interpolation_Values.x * Points[1] + 
					Interpolation_Values.y * Points[0] +
					Interpolation_Values.z * Points[2]
				);

				Position = Get_Model_Matrix(Target_Chart->Pushed_Objects[0]) * glm::vec4(Position + Normal * glm::vec3(0.005), 1);

				glm::mat4 Local_View = View_Matrix; // glm::translate(View_Matrix, -Position);

				Projection_Matrices[0] = Local_View;
				Projection_Matrices[1] = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * Local_View;
				Projection_Matrices[2] = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * Local_View;

				Projection_Matrices[3] = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * Local_View;
				Projection_Matrices[4] = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * Local_View;

				for (size_t Face = 0; Face < 5; Face++)
					Projection_Matrices[Face] = Perspective * glm::translate(Projection_Matrices[Face], -Position);

				glm::vec3 Colour = //glm::vec3(1.0f / glm::length(Position));
					
					
					Render_Scene_To_Lightmap_Pixel(Engine, Target_Chart, Light_Model, Projection_Matrices, Position, Normal, Lightmap_Shader, Incident_Texture_Width, Pixel_Data);

				size_t Index = glm::dot(glm::vec2(1.0f, Target_Chart->Sidelength), glm::vec2(Pixels, Scanline) + Target_Chart->Pushed_Tris[Tri].Mesh.Mesh->Vertices[Target_Chart->Pushed_Tris[Tri].Index].Lightmap_UV);

				Lightmap_Texture_Data[Index] = Colour; // Sets colour
			}


		}
	}

	void Create_Lightmap_From_Chart(Jaguar_Engine* Engine, Texture* Lightmap_Texture, Lightmap_Chart* Target_Chart)
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

		glm::vec3* Pixel_Data = new glm::vec3[Incident_Texture_Width * Incident_Texture_Width]; // [Incident_Texture_Width * Incident_Texture_Width] ;

		// Note: I initially was going to render to a cubemap using the geometry shader, but I'll just render to each face separately

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

			Rasterise_Tri_Lightmap(Engine, Tri, Target_Chart, Incident_Texture_Width, Lightmap_Texture_Data, &Lightmap_Shader, Pixel_Data);
		
			printf("Tri %lu complete\n", Tri);
		}

		Create_Texture_Buffer(Lightmap_Texture, GL_RGB, Target_Chart->Sidelength, Target_Chart->Sidelength, GL_RGB, GL_FLOAT, Lightmap_Texture_Data, true);

		printf(" >> Successfully created lightmap of %lu dimensions!\n", Target_Chart->Sidelength);

		Engine->Scene.Lighting.Lightmap_Texture = *Lightmap_Texture;
		Engine->Scene.Lighting.Inverse_Lightmap_Scale = 1.0f / (float)Target_Chart->Sidelength;

		//

		Destroy_Shader(&Lightmap_Shader);

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

	void Fill_Chart_Square_Area(int Area, int X_Origin, int Y_Origin, Lightmap_Chart* Target_Chart)
	{
		Area--;
		int New_X_Origin = X_Origin + Area + 1; // std::fminf(Target_Chart->Sidelength - 1, X_Origin + Area);
		int New_Y_Origin = Y_Origin + Area + 1; // std::fminf(Target_Chart->Sidelength - 1, Y_Origin + Area);

		for (int X = New_X_Origin; X >= X_Origin - 1; X--)
			for (int Y = New_Y_Origin; Y >= Y_Origin - 1; Y--)
				Target_Chart->Occupied[X][Y] = true;			// Sets this block as occupied
	}
	bool Check_Chart_Square_Area(int Area, int X_Origin, int Y_Origin, const Lightmap_Chart* Target_Chart)
	{
		Area--;
		
		int New_X_Origin = X_Origin + Area + 1;
		int New_Y_Origin = Y_Origin + Area + 1;
		if (New_X_Origin >= Target_Chart->Sidelength || 
			New_Y_Origin >= Target_Chart->Sidelength)
			return false;

		//int New_X_Origin = std::fminf(Target_Chart->Sidelength - 1, X_Origin + Area);
		//int New_Y_Origin = std::fminf(Target_Chart->Sidelength - 1, Y_Origin + Area);

		for (int X = New_X_Origin; X >= X_Origin - 1; X--)
			for (int Y = New_Y_Origin; Y >= Y_Origin - 1; Y--)
				if (Target_Chart->Occupied[X][Y])
					return false;

		return true;
	}

	bool Find_New_Location_Lightmap_Chart(int Area, int* X, int* Y, const Lightmap_Chart* Target_Chart)
	{
		// Find area closest to top-left to place square

		unsigned int Max = Target_Chart->Sidelength - 1;

		int Begin_X = 1;
		int Begin_Y = 1;

		while (Begin_Y < Target_Chart->Sidelength)
		{
			*X = Begin_X;
			*Y = Begin_Y;

			while (*X >= 1 && *Y < Max)
			{
				if (Check_Chart_Square_Area(Area, *X, *Y, Target_Chart))
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

	void Assemble_Lightmap_Chart(Lightmap_Chart* Target_Chart)
	{
		std::sort(Target_Chart->Pushed_Tris.begin(), Target_Chart->Pushed_Tris.end(), Lightmap_Tri_Sort_Compare); // Sorts them accordingly

		std::set<Mesh_Cache_Info> Updated_Meshes;

		for (size_t Tri = 0; Tri < Target_Chart->Pushed_Tris.size(); Tri++)
		{
			int Square_Size = Target_Chart->Pushed_Tris[Tri].Size;
			Mesh_Cache_Info Mesh_Info = Target_Chart->Pushed_Tris[Tri].Mesh;

			Updated_Meshes.insert(Mesh_Info);

			size_t Triangle = Target_Chart->Pushed_Tris[Tri].Index;

			int X, Y;

			while (!Find_New_Location_Lightmap_Chart(Square_Size, &X, &Y, Target_Chart))
				Upsize_Chart(Target_Chart); // Doubles size until there's room somewhere on chart

			Fill_Chart_Square_Area(Square_Size, X, Y, Target_Chart);

			Mesh_Info.Mesh->Vertices[Triangle].Lightmap_UV.x = X;
			Mesh_Info.Mesh->Vertices[Triangle++].Lightmap_UV.y = Y;

			Mesh_Info.Mesh->Vertices[Triangle].Lightmap_UV.x = X + Square_Size;
			Mesh_Info.Mesh->Vertices[Triangle++].Lightmap_UV.y = Y;

			Mesh_Info.Mesh->Vertices[Triangle].Lightmap_UV.x = X;
			Mesh_Info.Mesh->Vertices[Triangle++].Lightmap_UV.y = Y + Square_Size;
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
		
		const float Luxel_Scale = 20.0f; // 1 unit squared equals 5x5 pixels of area

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