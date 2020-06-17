#include "RenderEngineGPU.h"
#include <algorithm>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/component_wise.hpp>
RenderEngineGPU::RenderEngineGPU(GLFWwindow* window) : RenderEngine(window)
{
	//Create shaders
	std::cout << "Init shaders" << std::endl;
	//vertex.Init("quad.vert", GL_VERTEX_SHADER);
	//fragment.Init("quad.frag", GL_FRAGMENT_SHADER);
	//compute.Init("raytrace.comp", GL_COMPUTE_SHADER);
	shaderentity_vertex.Init("render_entity.vert", GL_VERTEX_SHADER);
	shaderentity_fragment.Init("render_entity.frag", GL_FRAGMENT_SHADER);
	shaderquad_vertex.Init("render_quad.vert", GL_VERTEX_SHADER);
	shaderquad_fragment.Init("render_quad.frag", GL_FRAGMENT_SHADER);
	std::cout << "Creating program" << std::endl;
	program_entity.CreateProgram();
	program_entity.AddShader(shaderentity_vertex);
	program_entity.AddShader(shaderentity_fragment);
	program_entity.LinkProgram();
	program_quad.CreateProgram();
	program_quad.AddShader(shaderquad_vertex);
	program_quad.AddShader(shaderquad_fragment);
	program_quad.LinkProgram();
	//std::cout << "Creating compute shader" << std::endl;
	//program_compute.CreateProgram();
	//program_compute.AddShader(compute);
	//program_compute.LinkProgram();

	std::cout << "Init buffers" << std::endl;
	//Create textures
//Gen

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
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	program_entity.UseProgram();
	glEnable(GL_PROGRAM_POINT_SIZE);
	glGenVertexArrays(1, &Entity_VAO);
	glGenBuffers(1, &Entity_VBO);
	glBindVertexArray(Entity_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, Entity_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * EntityVertexBuffer.size(), EntityVertexBuffer.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	//program_compute.UseProgram();
	program_quad.UseProgram();
	GLuint texdest = glGetUniformLocation(program_quad.GetProgram(), "DestTexture");
	if (texdest == -1)
	{
		std::cout << "Error finding texture destination" << std::endl;
	}
	UniformOffset = glGetUniformLocation(program_quad.GetProgram(), "Offset");
	UniformScale = glGetUniformLocation(program_quad.GetProgram(), "Scale");
	glUniform2f(UniformOffset, 0, 0);
	glUniform2f(UniformScale, 1, 1);
//	UniformYaw = glGetUniformLocation(program_quad.GetProgram(), "CameraYaw");
//	UniformTilt = glGetUniformLocation(program_quad.GetProgram(), "CameraTilt");
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
//	glDepthFunc(GL_LESS);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glBindVertexArray(FB_VAO);
//	PheremoneTexture = std::vector<float>(PheremoneSizeX * PheremoneSizeY);
	PheremoneTexture.resize(4 * PheremoneSizeX * PheremoneSizeY);
	std::fill(PheremoneTexture.begin(), PheremoneTexture.end(), 1);
	glGenTextures(1, &TextureFrameBuffer);
	glBindTexture(GL_TEXTURE_2D, TextureFrameBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,PheremoneSizeX, PheremoneSizeY, 0, GL_RGBA, GL_FLOAT, PheremoneTexture.data());
//	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(texdest,0);
	//glBindImageTexture(0,TextureFrameBuffer,0,GL_FALSE,0,GL_WRITE_ONLY,GL_RGBA);

/*	glGenBuffers(1, &BlockBufferSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER,BlockBufferSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GPUBlock) * GPUBlockBuffer.size(), GPUBlockBuffer.data(), GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, BlockBufferSSBO);
	glGenBuffers(1, &EntityBufferSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER,EntityBufferSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GPUEntityPrimitive) * GPUEntityBuffer.size(), GPUEntityBuffer.data(), GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, EntityBufferSSBO);
	glGenBuffers(1, &ChunkBufferSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER,ChunkBufferSSBO);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GPUChunk) * GPUChunkBuffer.size(), GPUChunkBuffer.data(), GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ChunkBufferSSBO);
//	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
*/

	GLfloat sizes[2];
	glGetFloatv(GL_POINT_SIZE_RANGE, sizes);
	std::cout << "Point size range:(" << sizes[0] << "," << sizes[1] << ")\n";
}
RenderEngineGPU::~RenderEngineGPU()
{

}
void RenderEngineGPU::CreateQuadTree(World& world)
{
	for (int i = 0; i < world.EntityList.ElementCount; ++i)
	{
		auto& e = world.EntityList.GetElement(i);
		e.Rendered = false;
	}
	ViewChunkCount = 2 + ceilf(cam.CameraXSize / Chunk::Size)*2;
	glm::vec3 PlayerLocation = cam.Position;
	int PosX = floor(PlayerLocation.x / Chunk::Size) - floor(ViewChunkCount/2);
	int PosY = floor(PlayerLocation.y / Chunk::Size) - floor(ViewChunkCount/2);
	CurrentChunkIds.x = PosX;
	CurrentChunkIds.y = PosY;
	CurrentChunkPos.x = cam.Position.x;//PosX * Chunk::Size;
	CurrentChunkPos.y = cam.Position.y;//PosY * Chunk::Size;
	EntityCounter = 0;
	for (int cx = 0; cx < ViewChunkCount; ++cx)
	{
		for (int cy = 0; cy < ViewChunkCount; ++cy)
		{
			//GPUChunkBuffer[cy + (cx * ViewChunkCount)].EntityCount = 0;
			//if (world.Chunks.InBounds(CurrentChunkIds.x + cx, CurrentChunkIds.y + cy))
			{
				glm::vec2 ChunkPos = glm::vec2(CurrentChunkIds.x + cx, CurrentChunkIds.y + cy) * Chunk::Size;
				Chunk& chunk = world.Chunks.GetChunk(CurrentChunkIds.x + cx, CurrentChunkIds.y + cy);
				for (int entity = 0; entity < chunk.EntityCount; ++entity) {
					if (EntityCounter < MaxEntityCount)
					{
						//auto & gpuchunk = GPUChunkBuffer[cy + (cx * ViewChunkCount)];
						//gpuchunk.EntityIds[gpuchunk.EntityCount++] = EntityCounter;
						//auto& gpuentity = GPUEntityBuffer[EntityCounter];
						GPUEntityPrimitive gpuentity = GPUEntityPrimitive();
						auto& worldentity = world.EntityList.GetElement(chunk.EntityIDList[entity]);
						if (!worldentity.Rendered)
						{
							//worldentity.Rendered = true;
							gpuentity.PosY = (ChunkPos.y + world.WrapOther(worldentity.Position.y,chunk.Y) - CurrentChunkPos.y) / cam.CameraYSize;
							gpuentity.PosX = (ChunkPos.x + world.WrapOther(worldentity.Position.x,chunk.X) - CurrentChunkPos.x) / cam.CameraXSize;
							//gpuentity.PosY = worldentity.Position.y / cam.CameraYSize;
							//gpuentity.PosX = worldentity.Position.x / cam.CameraXSize;
							gpuentity.SizeX = WindowSizePixelsX * worldentity.Size / (2 * cam.CameraXSize);
							gpuentity.ColourR = worldentity.Colour.x;
							gpuentity.ColourG = worldentity.Colour.y;
							gpuentity.ColourB = worldentity.Colour.z;
							EntityVertexBuffer[(EntityCounter*6) + 0] = gpuentity.PosX;
							EntityVertexBuffer[(EntityCounter*6) + 1] = gpuentity.PosY;
							EntityVertexBuffer[(EntityCounter*6) + 2] = gpuentity.ColourR;
							EntityVertexBuffer[(EntityCounter*6) + 3] = gpuentity.ColourG;
							EntityVertexBuffer[(EntityCounter*6) + 4] = gpuentity.ColourB;
							EntityVertexBuffer[(EntityCounter*6) + 5] = gpuentity.SizeX;
							++EntityCounter;
						}

					}
				}
			}
		}
	}
}
void RenderEngineGPU::RenderWorld(World& world)
{

 	/*program_compute.UseProgram();
//	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, BlockBufferSSBO);
 //	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GPUBlock) * GPUBlockBuffer.size(), GPUBlockBuffer.data(), GL_DYNAMIC_STORAGE_BIT);
 //	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, BlockBufferSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GPUBlock) * GPUBlockBuffer.size(), GPUBlockBuffer.data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, EntityBufferSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GPUEntityPrimitive) * GPUEntityBuffer.size(), GPUEntityBuffer.data());
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ChunkBufferSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GPUChunk) * GPUChunkBuffer.size(), GPUChunkBuffer.data());
	glUniform1f(UniformTilt,cam.tilt);
	glUniform1f(UniformYaw,cam.yaw);
	glUniform3f(UniformChunkOffset,cam.Position.x - CurrentChunkPos.x,cam.Position.y - CurrentChunkPos.y,cam.Position.z);
	glUniform1f(UniformRnd,(rand() % 12003) / 1000);
	glUniform1i(UniformEntityCount, EntityCounter);
 	glActiveTexture(GL_TEXTURE0);
 	glBindImageTexture(0, TextureFrameBuffer, 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
 	glDispatchCompute(FrameSizeX/LocalSize, (GLuint)FrameSizeY/LocalSize, 1);
//	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	//display render
	DisplayRender();
	//kick off compute shader
	*/
	glfwGetFramebufferSize(Window, &WindowSizePixelsX, &WindowSizePixelsY);
	CreateQuadTree(world);
	RenderEntities(world);
	RenderPheremone(world);
}
void RenderEngineGPU::DisplayRender() {
	program_quad.UseProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindImageTexture(0, TextureFrameBuffer, 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA32F);
	glBindVertexArray(FB_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void RenderEngineGPU::RenderPheremone(World& world)
{
	program_quad.UseProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(FB_VAO);
	glBindTexture(GL_TEXTURE_2D, TextureFrameBuffer);
//	glBindBuffer(GL_ARRAY_BUFFER, FB_VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(fb_verts), fb_verts, GL_STATIC_DRAW);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FB_EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fb_ebo), fb_ebo, GL_STATIC_DRAW);
	//glBindImageTexture(0, TextureFrameBuffer, 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA32F);
	if (this->RenderConfig.RenderPheremone)
	{
		auto& faction = world.FactionArray[this->RenderConfig.RenderAffiliation];
		ViewChunkCount = 2 + ceilf(cam.CameraXSize / PheremoneChunk::ChunkSize)*2;
		glm::vec3 PlayerLocation = cam.Position;
		int PosX = floor(PlayerLocation.x / PheremoneChunk::ChunkSize) - floor(ViewChunkCount/2);
		int PosY = floor(PlayerLocation.y / PheremoneChunk::ChunkSize) - floor(ViewChunkCount/2);
		CurrentChunkIds.x = PosX;
		CurrentChunkIds.y = PosY;
		CurrentChunkPos.x = cam.Position.x;//PosX * Chunk::Size;
		CurrentChunkPos.y = cam.Position.y;//PosY * Chunk::Size;
		EntityCounter = 0;
		for (int cx = 0; cx < ViewChunkCount; ++cx)
		{
			for (int cy = 0; cy < ViewChunkCount; ++cy)
			{
				glm::vec2 ChunkPos = glm::vec2(CurrentChunkIds.x + cx, CurrentChunkIds.y + cy) * PheremoneChunk::ChunkSize;
				auto & phc = faction.PheremoneHome.GetChunkGrid(CurrentChunkIds.x + cx, CurrentChunkIds.y + cy);
				auto & phf = faction.PheremoneFood.GetChunkGrid(CurrentChunkIds.x + cx, CurrentChunkIds.y + cy);
				auto & phd = faction.PheremoneDensity.GetChunkGrid(CurrentChunkIds.x + cx, CurrentChunkIds.y + cy);
				auto & phk = faction.PheremoneKilled.GetChunkGrid(CurrentChunkIds.x + cx, CurrentChunkIds.y + cy);
				auto & pha = faction.PheremoneAttack.GetChunkGrid(CurrentChunkIds.x + cx, CurrentChunkIds.y + cy);
				bool Active = false;
				switch (this->RenderConfig.Pheremone)
				{
				case 0:
					Active = phc.Active;
					break;
				case 1:
					Active = phf.Active;
					break;
				case 2:
					Active = phd.Active;
					break;
				case 3:
					Active = phk.Active;
					break;
				case 4:
					Active = pha.Active;
					break;
				}
				if(Active)
				{
					for(int x = 0;x < phc.GridCount;++x)
					{
						for (int y = 0; y < phc.GridCount; ++y)
						{
							int i = y + (x * phc.GridCount);
							float v = 0;
							switch (this->RenderConfig.Pheremone)
							{
							case 0:
								v = phc.raw_data[i].Strength;
								break;
							case 1:
								v = phf.raw_data[i].Strength;
								break;
							case 2:
								v = phd.raw_data[i].Strength;
								break;
							case 3:
								v = phk.raw_data[i].Strength;
								break;
							case 4:
								v = pha.raw_data[i].Strength;
								break;
							}
							PheremoneTexture[i * 4] = std::clamp(v / phc.MaxValue,0.0f,1.0f);
							PheremoneTexture[i * 4 + 1] = 0;
							PheremoneTexture[i * 4 + 2] = 0;
							PheremoneTexture[i * 4 + 3] = 1;// phc.raw_data[i].Strength;
						}
					}
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,PheremoneSizeX, PheremoneSizeY, 0, GL_RGBA, GL_FLOAT, PheremoneTexture.data());
					//Chunk& chunk = world.Chunks.GetChunk(CurrentChunkIds.x + cx, CurrentChunkIds.y + cy);
					//glUniform2f(UniformOffset, phc.X / PheremoneChunk::ChunkSize, phc.Y / PheremoneChunk::ChunkSize);
					auto WorldPosition = glm::vec3(ChunkPos.x, ChunkPos.y,0) - cam.Position;
					glUniform2f(UniformOffset, WorldPosition.x / cam.CameraXSize, WorldPosition.y/ cam.CameraYSize);
					//glUniform2f(UniformOffset, phc.X - cam.Position.x, phc.Y - cam.Position.y);
					//glUniform2f(UniformOffset, 0.5, 0.5);
					//Transform -1,1 to chunk size
					glUniform2f(UniformScale, PheremoneChunk::ChunkSize / cam.CameraXSize, PheremoneChunk::ChunkSize / cam.CameraYSize);
//					glUniform2f(UniformScale, 0.1,0.1);
					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				}
			}
		}
	}
	glBindVertexArray(0);
}
void RenderEngineGPU::RenderEntities(World& world)
{
	program_entity.UseProgram();
	//glActiveTexture(GL_TEXTURE0);
	//glBindImageTexture(0, TextureFrameBuffer, 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA32F);
	glBindVertexArray(Entity_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, Entity_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * EntityCounter, EntityVertexBuffer.data(), GL_DYNAMIC_DRAW);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * EntityVertexBuffer.size(), EntityVertexBuffer.data(), GL_DYNAMIC_DRAW);
	glDrawArrays(GL_POINTS,0, EntityCounter);
	glBindVertexArray(0);
}
void RenderEngineGPU::RenderGrid(World& world)
{

}
