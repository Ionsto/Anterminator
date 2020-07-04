#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "ShaderProgram.h"
#include "WaterEngineGPU.h"
#include "Camera.h"
struct RenderConfig {
	bool Reflections = false;
	bool Shadows = true;
	bool Refraction = true;
	bool RenderPheremone = false;
	bool DrawMask = true;
	int Pheremone = 0;
	int RenderAffiliation = 0;
};

class RenderEngine
{
protected:
	GLFWwindow* Window;
public:
	int WindowSizePixelsX;
	int WindowSizePixelsY;
	RenderConfig RenderConfig;
	Camera cam;
	GLuint VertexArrays;
	GLuint RenderPositionBuffer;
	GLuint RenderColourBuffer;
	GLuint RenderSizeBuffer;
	std::array<GLuint,4> TexturePheremone;
	GLuint SceneBuffer;
	Shader shader_entityvert;
	Shader shader_entityfrag;
	Shader shaderquad_vertex;
	Shader shaderquad_fragment;
	Shader shaderrendermask_vertex;
	Shader shaderrendermask_fragment;
	Shader shaderrenderterrain_fragment;
	Shader shaderrenderterrain_vertex;
	Shader Compute_DrawMask;
	Shader Compute_BlurMask;
	ShaderProgram program_entity;
	ShaderProgram program_quad;
	ShaderProgram program_renderdraw;
	ShaderProgram program_renderterrain;
	ShaderProgram Program_DrawMaskUpdate;
	ShaderProgram Program_DrawMaskBlur;

	GLuint UniformEntityCameraPosition;
	GLuint UniformEntityScale;
	GLuint UniformEntityMaskTexture;
	GLuint UniformEntityUseMask;
	GLuint UniformEntityMVP;

	GLuint UniformPixelScale;
	GLuint UniformWorldSize;

	GLuint UniformQuadTexture;
	GLuint UniformQuadCameraPosition;
	GLuint UniformQuadScale;
	GLuint UniformQuadWorldSize;
	GLuint UniformQuadAffiliation;
	GLuint UniformQuadMVP;
	GLuint UniformDrawMaskAffiliation;

	GLuint UniformRenderMaskTexture;
	GLuint UniformRenderMaskCameraPosition;
	GLuint UniformRenderMaskScale;
	GLuint UniformRenderMaskWorldSize;
	GLuint UniformRenderMaskMVP;
	GLuint UniformRenderMaskAffiliation;

	GLuint UniformTerrainMVP;
	GLuint UniformTerrainWorldSize;
	GLuint UniformTerrainTexture;
	
	GLuint TextureDrawMask;
	GLuint TextureWorldTerrain;

	GLuint FB_VBO;
	GLuint FB_VAO;
	GLuint FB_EBO;
	float fb_verts[16] = {
		-1,-1, 0,0,
		-1, 1, 0,1,
		 1, 1, 1,1,
		 1,-1, 1,0
	};
	unsigned int fb_ebo[6] = {
		0,1,2,
		0,2,3
	};
	GLuint Terrain_VBO;
	GLuint Terrain_VAO;
	GLuint Terrain_EBO;
	std::vector<float> Terrain_Verts;
	std::vector<int> Terrain_Elements;
	static constexpr const int MaxEntityCount = WaterEngineGPU::MaxParticleCount;
	static constexpr const int FactionCount = WaterEngineGPU::FactionCount;
	static constexpr const float WorldSize = WaterEngineGPU::WorldSize;
	static constexpr const int DrawMaskResolution = 8;
	static constexpr const int DrawMaskSize = 2 * WorldSize / DrawMaskResolution;
	int EntityCount = 0;
	static constexpr const float ViewSizeThreshold = 1000.0 / WorldSize;
	RenderEngine(GLFWwindow* Window);
	void Init();
	void SetEntityEngine(WaterEngineGPU & entityengine);
	void Render();
	void RenderEntities();
	void RenderPheremone();
	void RenderDrawMask();
	void RenderTerrain();
};

