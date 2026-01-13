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
	m_programID = glCreateProgram();
	ProgramBuilder{ m_programID }
		.ShaderStage(GL_VERTEX_SHADER, "Shaders/Vert_Poly.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "Shaders/Frag_Poly.frag")
		.Link();

	GLint linked = 0;
	glGetProgramiv(m_programID, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		std::cerr << "Shader program link failed!" << std::endl;
		glDeleteProgram(m_programID);
		m_programID = 0;
	}

}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programID);
	m_programID = 0;
}


void CMyApp::InitBuffers()
{
	std::vector<Vertex> verts = MakePolygon(6);

	glCreateVertexArrays(1, &m_vaoID);
	glCreateBuffers(1, &m_vboID);

	glNamedBufferData(
		m_vboID,
		verts.size() * sizeof(Vertex),
		verts.data(),
		GL_STATIC_DRAW
	);

	glVertexArrayVertexBuffer(m_vaoID, 0, m_vboID, 0, sizeof(Vertex));

	// position
	glEnableVertexArrayAttrib(m_vaoID, 0);
	glVertexArrayAttribFormat(
		m_vaoID, 0, 3, GL_FLOAT, GL_FALSE,
		offsetof(Vertex, position)
	);
	glVertexArrayAttribBinding(m_vaoID, 0, 0);

	// normal
	glEnableVertexArrayAttrib(m_vaoID, 1);
	glVertexArrayAttribFormat(
		m_vaoID, 1, 3, GL_FLOAT, GL_FALSE,
		offsetof(Vertex, normal)
	);
	glVertexArrayAttribBinding(m_vaoID, 1, 0);
}


void CMyApp::CleanBuffers()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteVertexArrays(1, &m_vaoID);
}



bool CMyApp::Init()
{
	SetupDebugCallback();

	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	InitShaders();
	InitBuffers();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanBuffers();
}

void CMyApp::Update(const SUpdateInfo&)
{
	// MOST MÉG NINCS LOGIKA
}

void CMyApp::Render()
{

	if (m_programID != 0)
	{
		glUseProgram(m_programID);
		glBindVertexArray(m_vaoID);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 8);
		glBindVertexArray(0);
		glUseProgram(0);
	}


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);
	glBindVertexArray(m_vaoID);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glBindVertexArray(0);
	glUseProgram(0);
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

std::vector<Vertex> CMyApp::MakePolygon(int N)
{
	std::vector<Vertex> v;

	// középpont
	v.push_back({
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 1)
		});

	float step = glm::two_pi<float>() / N;

	for (int i = 0; i <= N; ++i) // <= : hogy bezáródjon a fan
	{
		float a = i * step;
		v.push_back({
			glm::vec3(cos(a), sin(a), 0),
			glm::vec3(0, 0, 1)
			});
	}

	return v;
}
