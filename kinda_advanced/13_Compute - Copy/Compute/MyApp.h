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

#include <vector>
#include <string>

struct SUpdateInfo
{
	float ElapsedTimeInSec = 0.0f;	// Elapsed time since start of the program
	float DeltaTimeInSec = 0.0f;	// Elapsed time since last update
};

struct FaceNode
{
	int id;
	int N;
	int parent = -1;
	int parentEdge = 0;
	float pivot = 0.0f;

	std::vector<int> children;

	glm::mat4 model = glm::mat4(1.0f);
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

	// Camera
	Camera m_camera;
	CameraManipulator m_cameraManipulator;


protected:
	void SetupDebugCallback();

	std::vector<FaceNode> m_faces;
	int m_activeFace = -1;

	GLuint m_programID = 0;

	GLuint m_vaoID = 0;
	GLuint m_vboID = 0;


	void InitShaders();
	void CleanShaders();

	void InitBuffers();
	void CleanBuffers();

	bool LoadPoly(const std::string& filename);
	void ComputeTransforms();
	std::vector<Vertex> MakePolygon(int N);
};