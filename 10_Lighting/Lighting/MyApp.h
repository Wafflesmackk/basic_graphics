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

	// Adat változók

	float m_ElapsedTimeInSec = 0.0f;

	bool m_IsLampOn = true;

	glm::mat4 m_tableWorldTransform;
	glm::mat4 m_sphereWorldTransform;
	glm::mat4 m_SuzanneWorldTransform;

	//Lamp

	glm::vec3 m_lampPos = glm::vec3(0.0, 1.0, 0.0);

	//Bogár
	glm::vec3 m_bugPos = glm::vec3(0.0, 1.0, 0.0);


	// Asztal paraméterek 

	static constexpr float     TABLE_SIZE = 5.0f;
	static constexpr glm::vec3 TABLE_POS = glm::vec3(0.0f, 0.0f, 0.0f);

	// Suzanne paraméterek 

	static constexpr glm::vec3 SUZANNE_POS = glm::vec3(2.0f, 1.0f, 2.0f);

	// Gömb paraméterek 

	static constexpr glm::vec3 SPHERE_POS = glm::vec3(-2.0f, 1.0f, -2.0f);

	// Kamera 
	Camera m_camera;
	CameraManipulator m_cameraManipulator;

	// Fényforrások 

	static constexpr glm::vec3 BUG_COLOR = glm::vec3(0.53f, 1.0f, 0.3f);

	// OpenGL-es dolgok

	// Shaderekhez szükséges változók 
	GLuint m_programID = 0; // shaderek programja 

	// Fényforrás - ...
	glm::vec4 m_lightPosition = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

	glm::vec3 m_La = glm::vec3(0.0, 0.0, 0.0); //ambient
	glm::vec3 m_Ld = glm::vec3(1.0, 1.0, 1.0); //diffúz szín
	glm::vec3 m_Ls = glm::vec3(1.0, 1.0, 1.0); //csillanás

	//négyzetes fény csillapodáshoz
	float m_lightConstantAttenuation = 1.0;
	float m_lightLinearAttenuation = 0.0;
	float m_lightQuadraticAttenuation = 0.0;

	// ...  és anyagjellemzők 
	glm::vec3 m_Ka = glm::vec3(1.0); //visszaverés
	glm::vec3 m_Kd = glm::vec3(1.0); //diffúz színre
	glm::vec3 m_Ks = glm::vec3(1.0);

	// float m_Shininess = 20.0;

	// Shaderek inicializálása, és törlése 
	void InitShaders();
	void CleanShaders();

	// Geometriával kapcsolatos változók 

	OGLObject m_quadGPU = {};
	OGLObject m_SuzanneGPU = {};
	OGLObject m_sphereGPU = {};

	// Geometria inicializálása, és törlése 
	void InitGeometry();
	void CleanGeometry();

	// Textúrázás, és változói 
	GLuint m_SamplerID = 0;

	GLuint m_woodTextureID = 0;
	GLuint m_SuzanneTextureID = 0;
	GLuint m_sphereTextureID = 0;

	void SetCommonUniforms();

	void DrawObject(OGLObject& obj, const glm::mat4& world);
	void RenderTable();
	void RenderBall();
	void RenderSuzanne();

	void InitTextures();
	void CleanTextures();
};

