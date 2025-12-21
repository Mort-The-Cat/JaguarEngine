#include "Cubemap_Reflection_Generation.h"
#include "../OpenGL_Handling/Render_Queue.h"
#include "Jaguar_Engine_Wrapper.h"

namespace Jaguar
{


	void Generate_Cubemap(Jaguar::Jaguar_Engine* Engine, Cubemap* Target_Cubemap, unsigned int Width, unsigned Height)
	{
		// This will create a special framebuffer which we'll render to using the regular engine render pipeline
		// We'll also set the Camera_Projection_Matrix according to each face of the cubemap

		unsigned int Frame_Buffer;
		glGenFramebuffers(1, &Frame_Buffer);

		glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer);

		unsigned int Render_Buffer; // This is for the depth/stencil buffer
		glGenRenderbuffers(1, &Render_Buffer);

		glBindRenderbuffer(GL_RENDERBUFFER, Render_Buffer);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Render_Buffer);

		glGenTextures(1, &Target_Cubemap->Cubemap_Texture);

		glBindTexture(GL_TEXTURE_CUBE_MAP, Target_Cubemap->Cubemap_Texture);

		for (size_t Face = 0; Face < 6; Face++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

			glTexParameteri(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		glm::vec3 Directions[6] =
		{
			glm::vec3(1, 0, 0),
			glm::vec3(-1, 0, 0),

			glm::vec3(0, 1, 0),
			glm::vec3(0, -1, 0),

			glm::vec3(0, 0, 1),
			glm::vec3(0, 0, -1)
		};

		glm::vec3 Up_Vectors[6] =
		{
			glm::vec3(0, -1, 0),
			glm::vec3(0, -1, 0),

			glm::vec3(0, 0, 1),
			glm::vec3(0, 0, -1),

			glm::vec3(0, -1, 0),
			glm::vec3(0, -1, 0)
		};

		//

		glViewport(0, 0, Width, Height);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CCW);

		//

		for (size_t Face = 0; Face < 6; Face++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer);

			glBindTexture(GL_TEXTURE_CUBE_MAP, Target_Cubemap->Cubemap_Texture);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face, Target_Cubemap->Cubemap_Texture, 0);

			glBindRenderbuffer(GL_RENDERBUFFER, Render_Buffer);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				printf(" >> ERROR! Incomplete framebuffer...\n");

			//

			glClearColor(0.3, 0.3, 0.2, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//

			Engine->Scene.Camera_Projection_Matrix = 
				glm::perspective(glm::radians(90.0f), 1.0f, 0.0001f, 100.0f) * 
				glm::lookAt(Target_Cubemap->Origin, Target_Cubemap->Origin + Directions[Face], Up_Vectors[Face]);

			Draw_Render_Pipeline(&Engine->Pipeline, &Engine->Scene);

			glfwSwapBuffers(Engine->Window);
		}

		//
		
		glDeleteRenderbuffers(1, &Render_Buffer);
		glDeleteFramebuffers(1, &Frame_Buffer);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, Target_Cubemap->Cubemap_Texture);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}


}