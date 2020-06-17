#pragma once
#include "RenderEngine.h"
#include "ShaderProgram.h"
#include <array>
#include <vector>
#include <ctype.h>

struct GPUBlock{
	float RockHeight = 0;
	float SandHeight = 0;
	float SoilHeight = 0;
	float WaterHeight = 0;
	float SedimentContent = 0;
	float Entity = 0;
//	float padding;
};
struct GPUEntityPrimitive{
	float ColourR;
	float ColourG;
	float ColourB;
	float Colourw;
	float PosX;
	float PosY;
	float PosZ;
	float PosW;
	float SizeX;
	float SizeY;
	float SizeZ;
	float SizeW;
};
struct GPUChunk {
	int EntityCount;
	std::array<int, Chunk::MaxEntities> EntityIds;
};
class RenderEngineGPU : public RenderEngine
{
private:
	int WindowSizePixelsX = 1080;
	int WindowSizePixelsY = 1080;
	static constexpr int FrameSizeX = 128;
	static constexpr int FrameSizeY = 128;
	static constexpr int LocalSize = 32;
	static constexpr int RenderGridSize = 16;
	int ViewChunkCount = 1 + 2;
	GLuint BlockBufferSSBO;
	GLuint EntityBufferSSBO;
	GLuint ChunkBufferSSBO;
	std::vector<GPUBlock> GPUBlockBuffer = std::vector<GPUBlock>(RenderGridSize* RenderGridSize * ViewChunkCount * ViewChunkCount);
	//std::vector<GPUEntityPrimitive> GPUEntityBuffer = std::vector<GPUEntityPrimitive>(MaxEntityPrimitiveCount);
	std::vector<GPUChunk> GPUChunkBuffer = std::vector<GPUChunk>(ViewChunkCount * ViewChunkCount);
	int PheremoneSizeX = PheremoneChunk::GridCount;
	int PheremoneSizeY = PheremoneChunk::GridCount;
	std::vector<float> PheremoneTexture;
	glm::vec3 CurrentChunkPos;
	glm::ivec2 CurrentChunkIds;
	Shader compute;
	ShaderProgram program_compute;
	GLuint UniformTilt;
	GLuint UniformYaw;
	GLuint UniformSunTime;
	GLuint UniformRnd;
	GLuint UniformEntityCount;
	GLuint UniformChunkOffset;
	GLuint UniformOffset;
	GLuint UniformScale;
	float DitherTime = 0;
	Shader shaderentity_vertex;
	Shader shaderentity_fragment;
	Shader shaderquad_vertex;
	Shader shaderquad_fragment;
	ShaderProgram program_render;
	ShaderProgram program_entity;
	ShaderProgram program_quad;
	GLuint TextureFrameBuffer;
	int CurrentTexture = 0;

	GLuint Entity_VBO;
	GLuint Entity_VAO;
	std::vector<float> EntityVertexBuffer = std::vector<float>(MaxEntityCount*6);

	GLuint FB_VBO;
	GLuint FB_VAO;
	GLuint FB_EBO;
	float fb_verts[16] = {
		0,0, 0,0,
		0, 1, 0,1,
		 1, 1, 1,1,
		 1,0, 1,0
	};
	unsigned int fb_ebo[6] = {
		0,1,2,
		0,2,3
	};
public:
	static constexpr int MaxEntityCount = 1'000'000;
	int EntityCounter = 0;
	RenderEngineGPU(GLFWwindow* window);
	virtual ~RenderEngineGPU();
	virtual void RenderWorld(World& world) override;
	void RenderEntities(World& world);
	void RenderGrid(World& world);
	void CreateQuadTree(World& world);
	void RenderPheremone(World& world);
	void DisplayRender();
};

