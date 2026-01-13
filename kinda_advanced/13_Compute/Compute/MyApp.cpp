#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"
#include "ProgramBuilder.h"

#include <imgui.h>
#include <iostream>
#include <array>
#include <random>
#include <cmath>

CMyApp::CMyApp()
{
	// Let's see how big worker groups can we make
	std::cout << "GL_MAX_COMPUTE_WORK_GROUP_COUNT:\n";
	int max;
	for (int i : {0, 1, 2})
	{
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, i, &max);
		std::cout << "dim[" << i << "] = " << max << ", ";
	}
}

CMyApp::~CMyApp()
{
}

void CMyApp::SetupDebugCallback()
{
	// Enable and set the debug callback function if we are in debug context
	GLint context_flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
	if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_FALSE);
		glDebugMessageCallback(SDL_GLDebugMessageCallback, nullptr);
	}
}

void CMyApp::InitShaders()
{
	m_programLifeID = glCreateProgram();
	ProgramBuilder{ m_programLifeID }
		.ShaderStage(GL_COMPUTE_SHADER, "Shaders/Comp_Life.comp")
		.Link();

	m_programFullScreenID = glCreateProgram();
	ProgramBuilder{ m_programFullScreenID }
		.ShaderStage(GL_VERTEX_SHADER, "Shaders/Vert_FullScreen.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "Shaders/Frag_FullScreen.frag")
		.Link();

	m_programParticleID = glCreateProgram();
	ProgramBuilder{ m_programParticleID }
		.ShaderStage(GL_COMPUTE_SHADER, "Shaders/Comp_Particle.comp")
		.Link();

	m_programPointID = glCreateProgram();
	ProgramBuilder{ m_programPointID }
		.ShaderStage(GL_VERTEX_SHADER, "Shaders/Vert_Particle.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "Shaders/Frag_Particle.frag")
		.Link();
}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programLifeID);
	glDeleteProgram(m_programFullScreenID);
	glDeleteProgram(m_programParticleID);
	glDeleteProgram(m_programPointID);
}

void CMyApp::InitTextures()
{
	glCreateSamplers(1, &m_SamplerID);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_outputTextureID);
	glTextureStorage2D(m_outputTextureID, 1, GL_RGBA32F, W, H);

	// Set the texture all black
	const glm::vec4 defaultVal = glm::vec4(0, 0, 0, 1);
	glClearTexImage(m_outputTextureID, 0, GL_RGBA, GL_FLOAT, &defaultVal);
}

void CMyApp::CleanTextures()
{
	glDeleteTextures(1, &m_outputTextureID);

	glDeleteSamplers(1, &m_SamplerID);
}

void CMyApp::InitBuffers()
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> rPosition(-0.5f, 0.5f);
	std::uniform_real_distribution<float> rSpeed(-0.1f, 0.1f);

	// Particle buffer
	glCreateBuffers(1, &m_bufferParticleID);
	glNamedBufferData(m_bufferParticleID, ParticleCount * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);

	// We get back a memory address where our buffer is mapped
	Particle* client_ptr =
		(Particle*)glMapNamedBufferRange(m_bufferParticleID, // Where is the buffer bound
			0, // Offset
			ParticleCount * sizeof(Particle), // Size in bytes
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT); // Flags

	for (int i = 0; i < ParticleCount; ++i)
		client_ptr[i] = Particle{ glm::vec2(rPosition(mt), rPosition(mt)), glm::vec2(rSpeed(mt), rSpeed(mt)) };

	glUnmapNamedBuffer(m_bufferParticleID);

	// VAO for the buffer, we use it to draw the particles
	const std::array<VertexAttributeDescriptor, 2> attribs
	{
		VertexAttributeDescriptor{ 0, offsetof(Particle, position), 2, GL_FLOAT },
		VertexAttributeDescriptor{ 1, offsetof(Particle, speed), 2, GL_FLOAT }
	};

	glCreateVertexArrays(1, &m_vaoParticleID);
	glBindVertexArray(m_vaoParticleID);
	glVertexArrayVertexBuffer(m_vaoParticleID, 0, m_bufferParticleID, 0, sizeof(Particle));

	for (const auto& vertexAttrDesc : attribs)
	{
		glEnableVertexArrayAttrib(m_vaoParticleID, vertexAttrDesc.index); // Enable generic vertex attribute array
		glVertexArrayAttribBinding(m_vaoParticleID, vertexAttrDesc.index, 0); // Bind a generic vertex attribute to a buffer binding point

		glVertexArrayAttribFormat(
			m_vaoParticleID,
			vertexAttrDesc.index,				// Index of the generic vertex attribute
			vertexAttrDesc.numberOfComponents,	// Component count
			vertexAttrDesc.glType,				// Component type
			GL_FALSE,							// Normalize or not
			vertexAttrDesc.strideInBytes // Specifies the offset of the first component
		);
	}
}

void CMyApp::CleanBuffers()
{
	glDeleteBuffers(1, &m_bufferParticleID);
}

bool CMyApp::Init()
{
	SetupDebugCallback();

	// Set a bluish clear color
	// glClear() will use this for clearing the color buffer.
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	InitShaders();
	InitTextures();
	InitBuffers();

	//
	// Other
	//

	glEnable(GL_CULL_FACE);	 // Enable discarding the back-facing faces.
	glCullFace(GL_BACK);     // GL_BACK: facets facing away from camera, GL_FRONT: facets facing towards the camera
	glEnable(GL_DEPTH_TEST); // Enable depth testing. (for overlapping geometry)
	glDisable(GL_PROGRAM_POINT_SIZE); // For now, we use glPointSize() to set the size of points,
	// so disable setting it from the shader (with gl_PointSize)
	glEnable(GL_POINT_SPRITE);

	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanTextures();
	CleanBuffers();
}

void CMyApp::Update(const SUpdateInfo& updateInfo)
{
	m_delta_time = updateInfo.DeltaTimeInSec;


	// 1. Texture
	// Bind a level of a texture to an image unit
	glBindImageTexture(0,					// Binding point (specified in the shader)
		m_outputTextureID,	// Texture ID
		0,					// Which (mip) level of the texture
		GL_FALSE,			// Should attach the every layer of texture array?
		0,					// Which layer, ignored if above is GL_TRUE
		GL_WRITE_ONLY,		// Acces
		GL_RGBA32F);		// Format
	glUseProgram(m_programLifeID);
	glDispatchCompute(W, H, 1);

	// 2. Particle

	glUseProgram(m_programParticleID);
	glUniform1f(0, m_delta_time);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_bufferParticleID);
	glDispatchCompute(ParticleCount / 128, 1, 1);

}

void CMyApp::Render()
{
	// Clear the framebuffer (GL_COLOR_BUFFER_BIT) and the depth buffer (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1. Texture

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glUseProgram(m_programFullScreenID);

	glBindTextureUnit(0, m_outputTextureID);
	glBindSampler(0, m_SamplerID);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// 2. Particle
	/*
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glUseProgram(m_programPointID);
	glBindVertexArray(m_vaoParticleID);
	glPointSize(20.0f);
	glDrawArrays(GL_POINTS, 0, ParticleCount);
	**/

	glUseProgram(0);
	glBindVertexArray(0);
}

void CMyApp::RenderGUI()
{
	// ImGui::ShowDemoWindow();
}

// https://wiki.libsdl.org/SDL3/SDL_KeyboardEvent
// https://wiki.libsdl.org/SDL3/SDL_Keysym
// https://wiki.libsdl.org/SDL3/SDL_Keycode
// https://wiki.libsdl.org/SDL3/SDL_Keymod

void CMyApp::KeyboardDown(const SDL_KeyboardEvent& key)
{
	if (!key.repeat) // Triggers only once when held
	{
		if (key.key == SDLK_F5 && key.mod & SDL_KMOD_CTRL) // CTRL + F5
		{
			CleanShaders();
			InitShaders();
		}
		if (key.key == SDLK_F1) // F1
		{
			GLint polygonModeFrontAndBack[2] = {};
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml
			glGetIntegerv(GL_POLYGON_MODE, polygonModeFrontAndBack); // Query the current polygon mode. It gives the front and back modes separately.
			GLenum polygonMode = (polygonModeFrontAndBack[0] != GL_FILL ? GL_FILL : GL_LINE); // Switch between FILL and LINE
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
			glPolygonMode(GL_FRONT_AND_BACK, polygonMode); // Set the new polygon mode
		}
	}
}

void CMyApp::KeyboardUp(const SDL_KeyboardEvent& key)
{
}

// https://wiki.libsdl.org/SDL3/SDL_MouseMotionEvent

void CMyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
}

// https://wiki.libsdl.org/SDL3/SDL_MouseButtonEvent

void CMyApp::MouseDown(const SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(const SDL_MouseButtonEvent& mouse)
{
}

// https://wiki.libsdl.org/SDL3/SDL_MouseWheelEvent

void CMyApp::MouseWheel(const SDL_MouseWheelEvent& wheel)
{
}

// New window size
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
}

// Other SDL events
// https://wiki.libsdl.org/SDL3/SDL_Event

void CMyApp::OtherEvent(const SDL_Event& ev)
{
}