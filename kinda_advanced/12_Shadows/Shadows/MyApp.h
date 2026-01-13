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
#include "Camera.h"
#include "CameraManipulator.h"
#include "GLUtils.hpp"

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
	void SetupDebugCallback();
	void RenderGeometry(const glm::mat4&, GLuint, bool = false);

	//
	// Variables
	//
	float m_ElapsedTimeInSec = 0.0f;
	int m_width = 640, m_height = 480;

	glm::mat4 m_suzanneWorldTransform[9] = {};

	// Camera
	Camera m_camera;
	CameraManipulator m_cameraManipulator;

	//
	// OpenGL
	//

	void DrawAxes();

	// Shader variables
	GLuint m_programID = 0;				// Shader of the objects
	GLuint m_programAxesID = 0;			// Program showing X,Y,Z directions
	GLuint m_programPostprocessID = 0;	// Postprocess program

	// Shader initialization and termination
	void InitShaders();
	void CleanShaders();
	void InitAxesShader();
	void CleanAxesShader();

	// Geometry variables
	OGLObject m_Suzanne = {};
	OGLObject m_plane = {};

	// Geometry initialization and termination
	void InitGeometry();
	void CleanGeometry();

	// Texture variables
	GLuint m_SamplerID = 0;
	GLuint m_metalTextureID = 0;

	// Texture initialization and termination
	void InitTextures();
	void CleanTextures();

	// Light source
	glm::mat4 m_light_mvp = {};
	glm::vec4 m_lightPosition = glm::normalize(glm::vec4(0, 1, -1, 0.0f));
	glm::mat4 m_light_mvp2 = {};
	glm::vec4 m_lightPosition2 = glm::normalize(glm::vec4(-1, -1, 0, 0.0f));

	// Framebuffer variables
	int m_bufferResolution = 1024;

	GLuint m_frameBufferID = 0;
	GLuint m_shadowTextureID = 0;
	GLuint m_frameBufferID2 = 0;
	GLuint m_shadowTextureID2 = 0;

	// Framebuffer initialization and termination
	void InitFrameBufferObject();
	void CleanFrameBufferObject();
	void InitResolutionDependentResources(int, int);
	void CleanResolutionDependentResources();
};