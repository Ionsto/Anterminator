#include "RenderEngineGPU.h"
#include <algorithm>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/component_wise.hpp>
RenderEngineGPU::RenderEngineGPU(GLFWwindow* window) : RenderEngine(window)
{
	//Create shaders
	std::cout << "Init shaders" << std::endl;
	vertex.Init("quad.vert", GL_VERTEX_SHADER);
	fragment.Init("quad.frag", GL_FRAGMENT_SHADER);
	compute.Init("raytrace.comp", GL_COMPUTE_SHADER);
	std::cout << "Creating program" << std::endl;
	program_render.CreateProgram();
	program_render.AddShader(vertex);
	program_render.AddShader(fragment);
	program_render.LinkProgram();
	program_render.UseProgram();
	std::cout << "Creating compute shader" << std::endl;
	program_compute.CreateProgram();
	program_compute.AddShader(compute);
	program_compute.LinkProgram();

	std::cout << "Init buffers" << std::endl;
	//Create textures
//Gen

	glGenBuffers(1, &FB_VAO);
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
	program_compute.UseProgram();
	GLuint texdest = glGetUniformLocation(program_compute.GetProgram(), "destTex");
	if (texdest == -1)
	{
		std::cout << "Error finding texture destination" << std::endl;
	}
	UniformYaw = glGetUniformLocation(program_compute.GetProgram(), "CameraYaw");
	UniformTilt = glGetUniformLocation(program_compute.GetProgram(), "CameraTilt");
	UniformChunkOffset = glGetUniformLocation(program_compute.GetProgram(), "ChunkOffsets");
	UniformSunTime = glGetUniformLocation(program_compute.GetProgram(), "SunTime");
	UniformRnd = glGetUniformLocation(program_compute.GetProgram(), "RandomTimeSeed");
	UniformEntityCount = glGetUniformLocation(program_compute.GetProgram(), "EntityCount");
	glUniform1i(texdest,0);
	glUniform1i(UniformEntityCount,0);
	glUniform1f(UniformTilt,0);
	glUniform1f(UniformYaw,0);
	glUniform1f(UniformSunTime,0);
	glUniform3f(UniformChunkOffset,0,0,0);
	glGenTextures(1, &TextureFrameBuffer);
	glBindTexture(GL_TEXTURE_2D, TextureFrameBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	std::vector<float>  data(FrameSizeX * FrameSizeY * 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, FrameSizeX, FrameSizeY, 0, GL_RGBA, GL_FLOAT, data.data());
	glBindImageTexture(0,TextureFrameBuffer,0,GL_FALSE,0,GL_WRITE_ONLY,GL_RGBA);
	glGenBuffers(1, &BlockBufferSSBO);
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
}
RenderEngineGPU::~RenderEngineGPU()
{

}
void RenderEngineGPU::CreateQuadTree(World& world)
{
	glm::vec3 PlayerLocation = cam.Position;
	int PosX = floor(PlayerLocation.x / Chunk::Size) - floor(ViewChunkCount/2);
	int PosY = floor(PlayerLocation.y / Chunk::Size) - floor(ViewChunkCount/2);
	CurrentChunkIds.x = PosX;
	CurrentChunkIds.y = PosY;
	CurrentChunkPos.x = PosX * Chunk::Size;
	CurrentChunkPos.y = PosY * Chunk::Size;
	EntityCounter = 0;
	for (int cx = 0; cx < ViewChunkCount; ++cx)
	{
		for (int cy = 0; cy < ViewChunkCount; ++cy)
		{
			GPUChunkBuffer[cy + (cx * ViewChunkCount)].EntityCount = 0;
			for (int x = 0; x < Chunk::Size; ++x)
			{
				for (int y = 0; y < Chunk::Size; ++y)
				{
					static constexpr int WorldSizeX = (RenderGridSize * ViewChunkCount);
					static constexpr int WorldSizeY = (RenderGridSize * ViewChunkCount);
					int id = ((y + (cy * Chunk::Size))) + (WorldSizeY * (x + (cx * Chunk::Size)));
					auto& gpublock = GPUBlockBuffer[id];
					if (world.Chunks.InBounds(CurrentChunkIds.x + cx,CurrentChunkIds.y + cy))
					{
						Chunk & chunk = world.Chunks.GetChunk(CurrentChunkIds.x + cx, CurrentChunkIds.y + cy);
						auto block = chunk.GetBlockLocal(x, y);
						gpublock.RockHeight = block.RockHeight;
						gpublock.SandHeight = block.SandHeight;
						gpublock.SoilHeight = block.SoilHeight;
						gpublock.WaterHeight = block.WaterHeight;
						gpublock.SedimentContent = block.SedimentContent;
						gpublock.Entity = block.Entity ? 1 : 0;
					}
					else
					{
						gpublock.RockHeight = Chunk::MaxHeight;
					}
					//gpublock.z = block.Colour.z;

					//gpublock.y = 0;
				}
			}
			if (world.Chunks.InBounds(CurrentChunkIds.x + cx, CurrentChunkIds.y + cy))
			{
				Chunk& chunk = world.Chunks.GetChunk(CurrentChunkIds.x + cx, CurrentChunkIds.y + cy);
				for (int entity = 0; entity < chunk.EntityCount; ++entity) {
					if (EntityCounter < MaxEntityPrimitiveCount)
					{
						auto & gpuchunk = GPUChunkBuffer[cy + (cx * ViewChunkCount)];
						gpuchunk.EntityIds[gpuchunk.EntityCount++] = EntityCounter;
						auto& gpuentity = GPUEntityBuffer[EntityCounter++];
						auto& worldentity = world.EntityList.GetParticle(chunk.EntityRef[entity]);
						gpuentity.PosX = worldentity.Position.x - CurrentChunkPos.x;
						gpuentity.PosY = worldentity.Position.y - CurrentChunkPos.y;
						gpuentity.PosZ = worldentity.Position.z;
						gpuentity.SizeX = 1;
						gpuentity.ColourR = worldentity.Colour.x;
						gpuentity.ColourG = worldentity.Colour.y;
						gpuentity.ColourB = worldentity.Colour.z;
					}
				}
			}
		}
	}
}
void RenderEngineGPU::RenderWorld(World& world)
{
	CreateQuadTree(world);
 	program_compute.UseProgram();
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
	glUniform1f(UniformSunTime,  world.SunCounter);
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
}
void RenderEngineGPU::DisplayRender() {
//	glBindTexture(GL_TEXTURE_2D, TextureFrameBuffer);
	program_render.UseProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindImageTexture(0, TextureFrameBuffer, 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA32F);
	glBindVertexArray(FB_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
