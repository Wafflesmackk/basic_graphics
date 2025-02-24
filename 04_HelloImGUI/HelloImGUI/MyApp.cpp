#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"

#include <imgui.h>

CMyApp::CMyApp()
{
}

CMyApp::~CMyApp()
{
}

void CMyApp::SetupDebugCallback()
{
	// engedélyezzük és állítsuk be a debug callback függvényt ha debug context-ben vagyunk 
	GLint context_flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
	if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		glDebugMessageCallback(SDL_GLDebugMessageCallback, nullptr);
	}
}

bool CMyApp::Init()
{
	SetupDebugCallback();

	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	//
	// egyéb inicializálás
	//

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hátrafelé néző lapok eldobását
	glCullFace(GL_BACK);    // GL_BACK: a kamerától "elfelé" néző lapok, GL_FRONT: a kamera felé néző lapok

	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

	return true;
}

void CMyApp::Clean()
{
}

void CMyApp::Update(const SUpdateInfo& updateInfo)
{
	m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;
}

void CMyApp::Render()
{
	glClearColor(m_col[0], m_col[1], m_col[2], 1.f); //törlési szín fehér lett
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT)...
	// ... és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void CMyApp::RenderGUI()
{
	//ImGui::ShowDemoWindow();
	ImGui::ColorEdit3("bg color", m_col);

	if (ImGui::Button("Invert bg color")) {
		m_col[0] = 1.f - m_col[0];
		m_col[1] = 1.f - m_col[1];
		m_col[2] = 1.f - m_col[2];
	};
}

// https://wiki.libsdl.org/SDL2/SDL_KeyboardEvent
// https://wiki.libsdl.org/SDL2/SDL_Keysym
// https://wiki.libsdl.org/SDL2/SDL_Keycode
// https://wiki.libsdl.org/SDL2/SDL_Keymod

void CMyApp::KeyboardDown(const SDL_KeyboardEvent& key)
{
	if (key.keysym.sym == SDLK_r) {
		m_col[0] = 1.f;
		m_col[1] = 0.f;
		m_col[2] = 0.f;
	}
	else if (key.keysym.sym == SDLK_g) {
		m_col[0] = 0.f;
		m_col[1] = 1.f;
		m_col[2] = 0.f;
	}
	else if (key.keysym.sym == SDLK_b) {
		m_col[0] = 0.f;
		m_col[1] = 0.f;
		m_col[2] = 1.f;
	}
}

void CMyApp::KeyboardUp(const SDL_KeyboardEvent& key)
{
}

// https://wiki.libsdl.org/SDL2/SDL_MouseMotionEvent

void CMyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	if (m_isLeftButtonDown) {
		float r = mouse.x / m_w;
		float g = mouse.y / m_h;

		m_col[0] = r;
		m_col[1] = g;
	}
}

// https://wiki.libsdl.org/SDL2/SDL_MouseButtonEvent

void CMyApp::MouseDown(const SDL_MouseButtonEvent& mouse)
{
	if (mouse.button == SDL_BUTTON_LEFT) {
		m_isLeftButtonDown = true;
	}
}

void CMyApp::MouseUp(const SDL_MouseButtonEvent& mouse)
{
	if (mouse.button == SDL_BUTTON_LEFT) {
		m_col[0] = 1.f;
		m_col[1] = 1.f;
		m_col[2] = 1.f;

		m_isLeftButtonDown = false;
	}
}

// https://wiki.libsdl.org/SDL2/SDL_MouseWheelEvent

void CMyApp::MouseWheel(const SDL_MouseWheelEvent& wheel)
{
}


// a két paraméterben az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	m_w = (float)_w;
	m_h = (float)_h;
	glViewport(0, 0, _w, _h);
}

// Le nem kezelt, egzotikus esemény kezelése
// https://wiki.libsdl.org/SDL2/SDL_Event

void CMyApp::OtherEvent(const SDL_Event& ev)
{

}
