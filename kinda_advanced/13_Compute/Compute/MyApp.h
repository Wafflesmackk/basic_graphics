#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

// Utils
#include "GLUtils.hpp"

#include <array>
struct SUpdateInfo
{
	float ElapsedTimeInSec = 0.0f;	// Elapsed time since start of the program
	float DeltaTimeInSec = 0.0f;	// Elapsed time since last update
};

class CMyApp
{
public:
	CMyApp();
	~CMyApp();

	bool Init();
	void Clean();

	void Update(const SUpdateInfo&);
	void Render();
	void RenderGUI();

	void KeyboardDown(const SDL_KeyboardEvent&);
	void KeyboardUp(const SDL_KeyboardEvent&);
	void MouseMove(const SDL_MouseMotionEvent&);
	void MouseDown(const SDL_MouseButtonEvent&);
	void MouseUp(const SDL_MouseButtonEvent&);
	void MouseWheel(const SDL_MouseWheelEvent&);
	void Resize(int, int);

	void OtherEvent(const SDL_Event&);
protected:
	struct Particle {
		glm::vec2 position;
		glm::vec2 speed;
	};
	void SetupDebugCallback();

	//
	// Variables
	//

	float m_delta_time = 0.0f;
	const int ParticleCount = 128 * 1;
	const int W = 256;
	const int H = 256;

	//
	// OpenGL
	//

	// Shader variables
	GLuint m_programPointID = 0;
	GLuint m_programFullScreenID = 0;
	GLuint m_programLifeID = 0; // Conway's Game of Life
	GLuint m_programParticleID = 0; // Particle simulation

	// Shader initialization and termination
	void InitShaders();
	void CleanShaders();

	// Texture variables
	GLuint m_SamplerID = 0;
	GLuint m_outputTextureID = 0;

	// Texture initialization and termination
	void InitTextures();
	void CleanTextures();

	// Buffer variables
	GLuint m_vaoParticleID = 0;
	GLuint m_bufferParticleID = 0;

	// Buffer initialization and termination
	void InitBuffers();
	void CleanBuffers();
};