#include "RenderEngine.h"
#include <iostream>
#include <GL/GL.h>
#include <glm/ext.hpp>


RenderEngine::RenderEngine(GLFWwindow* window)
{
	Window = window;
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	std::cout << "Renderer supported:" << renderer << std::endl;
	std::cout << "OpenGL version supported:" << version << std::endl;
	//glPointSize(2);
	std::cout << "Init shaders" << renderer << std::endl;
	shader_entityvert.Init("render_entity.vert", GL_VERTEX_SHADER);
	shader_entityfrag.Init("render_entity.frag", GL_FRAGMENT_SHADER);
	shaderquad_vertex.Init("render_quad.vert", GL_VERTEX_SHADER);
	shaderquad_fragment.Init("render_quad.frag", GL_FRAGMENT_SHADER);
	shaderrendermask_vertex.Init("render_drawmask.vert", GL_VERTEX_SHADER);
	shaderrendermask_fragment.Init("render_drawmask.frag", GL_FRAGMENT_SHADER);
	shaderrenderterrain_vertex.Init("render_terrain.vert", GL_VERTEX_SHADER);
	shaderrenderterrain_fragment.Init("render_terrain.frag", GL_FRAGMENT_SHADER);
	Compute_DrawMask.Init("drawmaskupdate.comp", GL_COMPUTE_SHADER);
	Compute_BlurMask.Init("drawmaskblur.comp", GL_COMPUTE_SHADER);
	std::cout << "Creating program" << std::endl;
	program_entity.CreateProgram();
	program_entity.AddShader(shader_entityvert);
	program_entity.AddShader(shader_entityfrag);
	program_entity.LinkProgram();
	program_quad.CreateProgram();
	program_quad.AddShader(shaderquad_vertex);
	program_quad.AddShader(shaderquad_fragment);
	program_quad.LinkProgram();
	program_renderdraw.CreateProgram();
	program_renderdraw.AddShader(shaderrendermask_vertex);
	program_renderdraw.AddShader(shaderrendermask_fragment);
	program_renderdraw.LinkProgram();

	program_renderterrain.CreateProgram();
	program_renderterrain.AddShader(shaderrenderterrain_vertex);
	program_renderterrain.AddShader(shaderrenderterrain_fragment);
	program_renderterrain.LinkProgram();

	Program_DrawMaskUpdate.CreateProgram();
	Program_DrawMaskUpdate.AddShader(Compute_DrawMask);
	Program_DrawMaskUpdate.LinkProgram();

	Program_DrawMaskBlur.CreateProgram();
	Program_DrawMaskBlur.AddShader(Compute_BlurMask);
	Program_DrawMaskBlur.LinkProgram();

	program_entity.UseProgram();
	UniformEntityCameraPosition = glGetUniformLocation(program_entity.GetProgram(), "CameraPosition");
	UniformEntityScale = glGetUniformLocation(program_entity.GetProgram(), "Scale");
	UniformPixelScale = glGetUniformLocation(program_entity.GetProgram(), "PixelScale");
	UniformWorldSize = glGetUniformLocation(program_entity.GetProgram(), "WorldSize");
	UniformEntityUseMask = glGetUniformLocation(program_entity.GetProgram(), "UseDrawMask");
	UniformEntityMaskTexture = glGetUniformLocation(program_entity.GetProgram(), "DrawMask");
	UniformEntityMVP = glGetUniformLocation(program_entity.GetProgram(), "MVP");

	UniformDrawMaskAffiliation = glGetUniformLocation(Program_DrawMaskUpdate.GetProgram(), "Affiliation");

	UniformQuadTexture = glGetUniformLocation(program_quad.GetProgram(), "DestTexture");
	UniformQuadCameraPosition = glGetUniformLocation(program_quad.GetProgram(), "CameraPosition");
	UniformQuadScale = glGetUniformLocation(program_quad.GetProgram(), "Scale");
	UniformQuadWorldSize = glGetUniformLocation(program_quad.GetProgram(), "WorldSize");
	UniformQuadAffiliation = glGetUniformLocation(program_quad.GetProgram(), "Affiliation");
	UniformQuadMVP = glGetUniformLocation(program_quad.GetProgram(), "MVP");

	UniformRenderMaskTexture = glGetUniformLocation(program_renderdraw.GetProgram(), "DestTexture");
	UniformRenderMaskCameraPosition = glGetUniformLocation(program_renderdraw.GetProgram(), "CameraPosition");
	UniformRenderMaskScale = glGetUniformLocation(program_renderdraw.GetProgram(), "Scale");
	UniformRenderMaskWorldSize = glGetUniformLocation(program_renderdraw.GetProgram(), "WorldSize");
	UniformRenderMaskMVP = glGetUniformLocation(program_renderdraw.GetProgram(), "MVP");


	UniformTerrainMVP = glGetUniformLocation(program_renderterrain.GetProgram(), "MVP");
	UniformTerrainWorldSize = glGetUniformLocation(program_renderterrain.GetProgram(), "WorldSize");
	UniformTerrainTexture = glGetUniformLocation(program_renderterrain.GetProgram(), "DestTexture");

	program_quad.UseProgram();
	glGenVertexArrays(1, &FB_VAO);
	glGenBuffers(1, &FB_EBO);
	glGenBuffers(1, &FB_VBO);
	//init
	glBindVertexArray(FB_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, FB_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fb_verts), fb_verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FB_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fb_ebo), fb_ebo, GL_STATIC_DRAW);

	glGenVertexArrays(1, &Terrain_VAO);
	glGenBuffers(1, &Terrain_EBO);
	glGenBuffers(1, &Terrain_VBO);
	static constexpr float TerrainResolution = 4;
	static constexpr int TerrainCount = 2 * WorldSize / TerrainResolution;
	Terrain_Verts.resize(TerrainCount * TerrainCount * 4);
	Terrain_Elements.resize((TerrainCount - 1)  * (TerrainCount - 1) * 6);
	int id = 0;
	int eid = 0;
	for (int x = 0; x < TerrainCount; ++x) {
		for (int y = 0; y < TerrainCount; ++y) {
			float u = x/float(TerrainCount-1);
			float v = y/float(TerrainCount-1);
			Terrain_Verts[id++] = (u-0.5)*2*WorldSize;
			Terrain_Verts[id++] = (v-0.5)*2*WorldSize;
			Terrain_Verts[id++] = u;
			Terrain_Verts[id++] = v;
		}
	}
	for (int x = 0; x < TerrainCount-1; ++x) {
		for (int y = 0; y < TerrainCount-1; ++y) {
			Terrain_Elements[eid++] = (y + (x * TerrainCount));
			Terrain_Elements[eid++] = (y + ((x+1) * TerrainCount));
			Terrain_Elements[eid++] = ((y+1) + (x * TerrainCount));

			Terrain_Elements[eid++] = (y + ((x+1) * TerrainCount));
			Terrain_Elements[eid++] = ((y+1) + ((x+1) * TerrainCount));
			Terrain_Elements[eid++] = ((y+1) + (x * TerrainCount));
		}
	}
	//init
	glBindVertexArray(Terrain_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, Terrain_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Terrain_Verts.size(), Terrain_Verts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Terrain_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * Terrain_Elements.size(), Terrain_Elements.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenTextures(1, &TextureDrawMask);
	glBindTexture(GL_TEXTURE_2D, TextureDrawMask);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, DrawMaskSize, DrawMaskSize, 0, GL_RGBA, GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	GLfloat sizes[2];
	glGetFloatv(GL_POINT_SIZE_RANGE, sizes);
	std::cout << "Point size range:(" << sizes[0] << "," << sizes[1] << ")\n";
}
void RenderEngine::Init()
{
	program_entity.UseProgram();
	glGenVertexArrays(1, &VertexArrays);
	glBindVertexArray(VertexArrays);
	glBindBuffer(GL_ARRAY_BUFFER, RenderPositionBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, RenderColourBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, RenderSizeBuffer);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), 0);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}
void RenderEngine::SetEntityEngine(WaterEngineGPU & entityengine)
{
	RenderPositionBuffer = entityengine.RenderPositionBuffer;
	RenderColourBuffer = entityengine.RenderColourBuffer;
	RenderSizeBuffer = entityengine.RenderSizeBuffer;
	TexturePheremone = entityengine.TexturePheremone;
	TextureWorldTerrain = entityengine.TextureWorldMap;
}
void RenderEngine::Render()
{
	glfwGetFramebufferSize(Window, &WindowSizePixelsX, &WindowSizePixelsY);
	cam.Update();
	RenderPheremone();
	RenderTerrain();
	RenderEntities();
	RenderDrawMask();
}
void RenderEngine::RenderEntities()
{
	//Update draw mask
	program_entity.UseProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureDrawMask);
	glBindVertexArray(VertexArrays);
	glUniform2f(UniformEntityCameraPosition, cam.Position.x, cam.Position.y);
	glUniform2f(UniformEntityScale, 1.0/cam.CameraXSize, 1.0/cam.CameraYSize);
	glUniform1f(UniformPixelScale, WindowSizePixelsY / (cam.CameraYSize));
	glUniform1f(UniformWorldSize, WorldSize);
	glUniform1i(UniformEntityMaskTexture, 0);
//	glUniform1i(UniformEntityUseMask, this->RenderConfig.DrawMask ? 1 : 0);
	glUniform1i(UniformEntityUseMask, 0);
	auto mvp = cam.MVPMat;
	glUniformMatrix4fv(UniformEntityMVP, 1, GL_FALSE, &mvp[0][0]);
	glDrawArrays(GL_POINTS, 0, EntityCount);
	glBindVertexArray(0);
}
void RenderEngine::RenderPheremone()
{
	program_quad.UseProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(FB_VAO);
	if (this->RenderConfig.RenderPheremone)
	{
		glBindTexture(GL_TEXTURE_3D, TexturePheremone[this->RenderConfig.Pheremone]);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,PheremoneSizeX, PheremoneSizeY, 0, GL_RGBA, GL_FLOAT, PheremoneTexture.data());
//		auto WorldPosition = glm::vec3(ChunkPos.x, ChunkPos.y,0) - cam.Position;
		//glUniform2f(UniformOffset, WorldPosition.x / cam.CameraXSize, WorldPosition.y/ cam.CameraYSize);
		int xmin = cam.Position.x < (-WorldSize * ViewSizeThreshold) ? -1 : 0;
		int xmax = cam.Position.x > (WorldSize * ViewSizeThreshold) ? 1 : 0;
		int ymin = cam.Position.y < (-WorldSize * ViewSizeThreshold) ? -1 : 0;
		int ymax = cam.Position.y > (WorldSize * ViewSizeThreshold) ? 1 : 0;
		for (int x = xmin; x <= xmax; ++x)
		{
			for (int y = ymin; y <= ymax; ++y)
			{
				//glUniform3f(UniformQuadCameraPosition,((2*WorldSize*x)+ cam.Position.x) / WorldSize,((2*WorldSize*y) + cam.Position.y) / WorldSize,cam.Position.z);
				//glUniform2f(UniformQuadScale, WorldSize / cam.CameraXSize, WorldSize / cam.CameraYSize);
				glUniform1f(UniformQuadWorldSize, WorldSize);
				auto mvp = cam.MVPMat * glm::translate(glm::mat4(1.f),glm::vec3(2*x*WorldSize,2*y*WorldSize,0));
				glUniformMatrix4fv(UniformQuadMVP, 1, GL_FALSE, &mvp[0][0]);
				//		glUniform1i(UniformQuadTexture, TexturePheremone[0]);
				float RenderAff = static_cast<float>(this->RenderConfig.RenderAffiliation) / static_cast<float>(FactionCount-1);
				//Scent
				if (this->RenderConfig.Pheremone == 3)
				{
					RenderAff = -1.0 / static_cast<float>(FactionCount-1);
				}
				glUniform1f(UniformQuadAffiliation, RenderAff);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}
	}
	glBindVertexArray(0);
}
void RenderEngine::RenderDrawMask() 
{
	if (this->RenderConfig.DrawMask && this->RenderConfig.RenderAffiliation != -1)
	{
		//Update the draw mask
		Program_DrawMaskUpdate.UseProgram();
		static constexpr int LocalSize = 16;
		glClearTexImage(TextureDrawMask, 0, GL_RGBA, GL_FLOAT,NULL);
		glBindImageTexture(1, TextureDrawMask, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glUniform1i(UniformDrawMaskAffiliation, this->RenderConfig.RenderAffiliation);
		for (int i = 0; i < 1; ++i)
		{
			glBindImageTexture(0, TexturePheremone[i], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG16F);
			glDispatchCompute(static_cast<int>(ceil(float(DrawMaskSize) / LocalSize)), static_cast<int>(ceil(float(DrawMaskSize) / LocalSize)), 1);
			//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
		}
		Program_DrawMaskBlur.UseProgram();
		for (int blursteps = 0; blursteps < 2; ++blursteps) {
			glDispatchCompute(static_cast<int>(ceil(float(DrawMaskSize) / LocalSize)), static_cast<int>(ceil(float(DrawMaskSize) / LocalSize)), 1);
			//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
		}

		program_renderdraw.UseProgram();
		glBindVertexArray(FB_VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureDrawMask);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,PheremoneSizeX, PheremoneSizeY, 0, GL_RGBA, GL_FLOAT, PheremoneTexture.data());
//		auto WorldPosition = glm::vec3(ChunkPos.x, ChunkPos.y,0) - cam.Position;
		//glUniform2f(UniformOffset, WorldPosition.x / cam.CameraXSize, WorldPosition.y/ cam.CameraYSize);
		int xmin = cam.Position.x < (-WorldSize * ViewSizeThreshold) ? -1 : 0;
		int xmax = cam.Position.x > (WorldSize * ViewSizeThreshold) ? 1 : 0;
		int ymin = cam.Position.y < (-WorldSize * ViewSizeThreshold) ? -1 : 0;
		int ymax = cam.Position.y > (WorldSize * ViewSizeThreshold) ? 1 : 0;
		for (int x = xmin; x <= xmax; ++x)
		{
			for (int y = ymin; y <= ymax; ++y)
			{
				glUniform2f(UniformRenderMaskCameraPosition,((2*WorldSize*x)+ cam.Position.x) / WorldSize,((2*WorldSize*y) + cam.Position.y) / WorldSize);
				glUniform2f(UniformRenderMaskScale, WorldSize / cam.CameraXSize, WorldSize / cam.CameraYSize);
				glUniform1f(UniformRenderMaskWorldSize, WorldSize);
				glUniform1i(UniformRenderMaskTexture, 0);
				auto mvp = cam.MVPMat * glm::translate(glm::mat4(1.f),glm::vec3(2*x*WorldSize,2*y*WorldSize,0));
				glUniformMatrix4fv(UniformRenderMaskMVP, 1, GL_FALSE, &mvp[0][0]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}
	}
	glBindVertexArray(0);
}

void RenderEngine::RenderTerrain() 
{
	//Update the draw mas
	program_renderterrain.UseProgram();
	glBindVertexArray(Terrain_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureWorldTerrain);
	int xmin = cam.Position.x < (-WorldSize * ViewSizeThreshold) ? -1 : 0;
	int xmax = cam.Position.x > (WorldSize * ViewSizeThreshold) ? 1 : 0;
	int ymin = cam.Position.y < (-WorldSize * ViewSizeThreshold) ? -1 : 0;
	int ymax = cam.Position.y > (WorldSize * ViewSizeThreshold) ? 1 : 0;
	for (int x = xmin; x <= xmax; ++x)
	{
		for (int y = ymin; y <= ymax; ++y)
		{
			//glUniform2f(UniformRenderMaskCameraPosition,((2*WorldSize*x)+ cam.Position.x) / WorldSize,((2*WorldSize*y) + cam.Position.y) / WorldSize);
			//glUniform2f(UniformRenderMaskScale, WorldSize / cam.CameraXSize, WorldSize / cam.CameraYSize);
			glUniform1f(UniformTerrainWorldSize, WorldSize);
			glUniform1i(UniformTerrainTexture, 0);
			auto mvp = cam.MVPMat * glm::translate(glm::mat4(1.f),glm::vec3(2*x*WorldSize,2*y*WorldSize,0));
			glUniformMatrix4fv(UniformTerrainMVP, 1, GL_FALSE, &mvp[0][0]);
			glDrawElements(GL_TRIANGLES, Terrain_Elements.size(), GL_UNSIGNED_INT, 0);
		}
	}
	glBindVertexArray(0);
}

