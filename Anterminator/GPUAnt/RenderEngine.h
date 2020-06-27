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
	Shader Compute_DrawMask;
	Shader Compute_BlurMask;
	ShaderProgram program_entity;
	ShaderProgram program_quad;
	ShaderProgram program_renderdraw;
	ShaderProgram Program_DrawMaskUpdate;
	ShaderProgram Program_DrawMaskBlur;
	GLuint UniformEntityCameraPosition;
	GLuint UniformEntityScale;
	GLuint UniformEntityMaskTexture;
	GLuint UniformEntityUseMask;
	GLuint UniformPixelScale;
	GLuint UniformWorldSize;
	GLuint UniformQuadTexture;
	GLuint UniformQuadCameraPosition;
	GLuint UniformQuadScale;
	GLuint UniformQuadWorldSize;
	GLuint UniformQuadAffiliation;
	GLuint UniformDrawMaskAffiliation;

	GLuint UniformRenderMaskTexture;
	GLuint UniformRenderMaskCameraPosition;
	GLuint UniformRenderMaskScale;
	GLuint UniformRenderMaskWorldSize;
	GLuint UniformRenderMaskAffiliation;
	
	GLuint TextureDrawMask;

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
	static constexpr const int MaxEntityCount = WaterEngineGPU::MaxParticleCount;
	static constexpr const int FactionCount = WaterEngineGPU::FactionCount;
	static constexpr const float WorldSize = WaterEngineGPU::WorldSize;
	static constexpr const int DrawMaskResolution = 8;
	static constexpr const int DrawMaskSize = 2 * WorldSize / DrawMaskResolution;
	int EntityCount = 0;
	RenderEngine(GLFWwindow* Window);
	void Init();
	void SetEntityEngine(WaterEngineGPU & entityengine);
	void Render();
	void RenderEntities();
	void RenderPheremone();
	void RenderDrawMask();
};

