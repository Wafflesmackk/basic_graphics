#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// Utils
#include "GLUtils.hpp"
#include "Camera.h"
#include "CameraManipulator.h"

struct SUpdateInfo
{
	float ElapsedTimeInSec = 0.0f; // Program indulása óta eltelt idő 
	float DeltaTimeInSec = 0.0f; // Előző Update óta eltelt idő 
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

	// Kiválasztás 
	void GetRayDirAndOrigin(glm::vec3& rayDir, glm::vec3& rayOrigin);
	int GetPickedObject(const glm::vec3& rayOrigin, const glm::vec3& rayDir, float& t);

	// Adat változók

	float m_ElapsedTimeInSec = 0.0f;

	glm::uvec2 m_windowSize = glm::uvec2(0);

	glm::mat4 m_tableWorldTransform;
	std::vector< glm::mat4 > m_spheresWorldTransforms;

	// Asztal paraméterek 

	static constexpr float     TABLE_SIZE = 15.0f;
	static constexpr glm::vec3 TABLE_POS = glm::vec3(0.0f, 0.0f, 0.0f);

	// Gömb paraméterek 

	struct SphereSetting
	{
		glm::vec3 Position = glm::vec3(0.0f);
		float Radius = 1.0f;
	};

	std::vector< SphereSetting > m_Spheres;

	int m_SelectedObject = -1;

	// Kiválasztás 

	glm::ivec2 m_PickedPixel = glm::ivec2(0, 0);
	bool m_IsPicking = false;
	bool m_IsCtrlDown = false;

	bool m_tableIndicator = false;
	glm::vec2 m_tableIndicatorUV = glm::vec2(0.0f);
	float m_tableIndicatorR = 0.0f;

	// Kamera 
	Camera m_camera;
	CameraManipulator m_cameraManipulator;

	// OpenGL-es dolgok

	// Shaderekhez szükséges változók 
	GLuint m_programID = 0; // shaderek programja 

	// Fényforrás - ...
	glm::vec4 m_lightPosition = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

	glm::vec3 m_La = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 m_Ld = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 m_Ls = glm::vec3(1.0, 1.0, 1.0);

	float m_lightConstantAttenuation = 1.0;
	float m_lightLinearAttenuation = 0.0;
	float m_lightQuadraticAttenuation = 0.0;

	// ...  és anyagjellemzők 
	glm::vec3 m_Ka = glm::vec3(1.0);
	glm::vec3 m_Kd = glm::vec3(1.0);
	glm::vec3 m_Ks = glm::vec3(1.0);

	float m_Shininess = 20.0;

	// Shaderek inicializálása, és törlése 
	void InitShaders();
	void CleanShaders();

	// Geometriával kapcsolatos változók 

	OGLObject m_quadGPU = {};
	OGLObject m_sphereGPU = {};

	// Geometria inicializálása, és törlése 
	void InitGeometry();
	void CleanGeometry();

	// Textúrázás, és változói 
	GLuint m_SamplerID = 0;

	GLuint m_woodTextureID = 0;
	GLuint m_sphereTextureID = 0;

	void InitTextures();
	void CleanTextures();

	void SetCommonUniforms();
	void DrawObject(OGLObject& obj, const glm::mat4& world);
	void RenderTable();
	void RenderSpheres();
};

