#include "RenderEngine.h"
#include <GL/GL.h>
void GLAPIENTRY
MessageCallback_GPU(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}


RenderEngine::RenderEngine(GLFWwindow* window)
{
	Window = window;
	glDebugMessageCallback(MessageCallback_GPU, 0);
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	std::cout << "Renderer supported:" << renderer << std::endl;
	std::cout << "OpenGL version supported:" << version << std::endl;
	//glPointSize(2);
	std::cout << "Init shaders" << renderer << std::endl;
	//vertex.Init("quad.vert", GL_VERTEX_SHADER);
	//fragment.Init("points.frag", GL_FRAGMENT_SHADER);
	std::cout << "Creating program" << std::endl;
	//program_grid.CreateProgram();
	//program_grid.AddShader(vertex);
	//program_grid.AddShader(fragment);
	//program_grid.LinkProgram();
	//program_grid.UseProgram();
	//glGenBuffers(1, &VertexBuffer);
	//glGenBuffers(1, &DensityBuffer);
	//glGenBuffers(1, &IndexBuffer);
	//vertex_points.Init("points.vert", GL_VERTEX_SHADER);
	//fragment_points.Init("points.frag", GL_FRAGMENT_SHADER);
	//std::cout << "Creating program" << std::endl;
	//program_points.CreateProgram();
	//program_points.AddShader(vertex_points);
	//program_points.AddShader(fragment_points);
	//program_points.LinkProgram();
	//program_points.UseProgram();
	//glGenBuffers(1, &VertexBufferPoints);
	//glGenBuffers(1, &DensityBufferPoints);
}
void RenderEngine::RenderWorld(World& world)
{

}
