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
#include "Camera.h"
#include "CameraManipulator.h"
#include "GLUtils.hpp"

struct SUpdateInfo
{
	float ElapsedTimeInSec = 0.0f; // Program indulása óta eltelt idő 
	float DeltaTimeInSec   = 0.0f; // Előző Update óta eltelt idő 
};

struct Ray
{
	glm::vec3 origin;
	glm::vec3 direction;
};

struct Intersection
{
	glm::vec2 uv;
	float t;
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

	float mLegAnimTime = 0.0f;
	bool mDoAnimateLeg = true;

	float mAngleHeadX = 0.0f;
	float mAngleHeadY = 0.0f;
	float mAngleArmUpper = 0.0f;
	float mAngleArmLower = 0.0f;

	glm::mat4 mTableTopTransformation;
	std::vector<glm::mat4> mTableLegTransforms;

	glm::mat4 m_headT;
	glm::mat4 m_torsoT;
	glm::mat4 m_rightUpperArmT;
	glm::mat4 m_rightLowerArmT;
	glm::mat4 m_legT;
	glm::mat4 m_leg2T;

    // Robot

	static constexpr float LEG_SPEED = 1.0;
	static constexpr float LEG_SPREAD = 1.5;
	static constexpr glm::vec3 RELATIVE_LEFT_ARM_LOWER = glm::vec3(0.114, -4.633, -0.005);
	static constexpr glm::vec3 RELATIVE_RIGHT_ARM_LOWER = glm::vec3(-0.114, -4.633, -0.005);
	static constexpr glm::vec3 TORSO_POS = glm::vec3(0, 17.525, 0);

	static constexpr glm::vec3 RELATIVE_LEFT_ARM_UPPER = glm::vec3(5.402, 2.966, -0.878);
	static constexpr glm::vec3 RELATIVE_RIGHT_ARM_UPPER = glm::vec3(-5.402, 2.966, -0.878);
	static constexpr glm::vec3 RELATIVE_HEAD_POS = glm::vec3(0.000, 5.928, -0.822);
	static constexpr glm::vec3 RELATIVE_LEFT_LEG_POS = glm::vec3(2.867, -4.883, -0.771);
	static constexpr glm::vec3 RELATIVE_RIGHT_LEG_POS = glm::vec3(-2.867, -4.883, -0.771);
	static constexpr glm::vec3 AVG_EYE_POS_OFFSET = glm::vec3(0, 5, 3.5);

    // Asztal 
	static constexpr glm::vec3 TABLE_SIZE = glm::vec3(200.0f, 10.0f, 200.0f);
	static constexpr glm::vec3 TABLE_POS = glm::vec3(0, -TABLE_SIZE.y * 0.5f, 0);
	static constexpr glm::vec3 TABLE_LEG_SIZE = glm::vec3(20, 120, 20);

	// Kiválasztás 

	glm::ivec2 m_PickedPixel = glm::ivec2( 0, 0 );
	bool m_IsPicking = false;
	bool m_IsCtrlDown = false;

	glm::uvec2 m_windowSize = glm::uvec2(0, 0);

	Ray CalculatePixelRay(glm::vec2 pickerPos) const;


	// Kamera 
	Camera m_camera;
	CameraManipulator m_cameraManipulator;

	// OpenGL-es dolgok

	// Shaderekhez szükséges változók 
	GLuint m_programID = 0; // shaderek programja 

	// Shaderek inicializálása, és törlése 
	void InitShaders();
	void CleanShaders();

	// Geometriával kapcsolatos változók 

	void SetCommonUniforms();
	void DrawObject(OGLObject& obj, const glm::mat4& world);

	OGLObject m_cubeGPU = {};
	OGLObject m_headGPU = {};
	OGLObject m_torsoGPU = {};
	OGLObject m_rightUpperArmGPU = {};
	OGLObject m_rightLowerArmGPU = {};
	OGLObject m_legGPU = {};
	static glm::vec3 CalculateLegAnim(float time, float animOffset);

	// Geometria inicializálása, és törlése 
	void InitGeometry();
	void CleanGeometry();

	// Textúrázás, és változói 
	GLuint m_SamplerID = 0;

	GLuint m_woodTextureID = 0;
	GLuint m_RobotTextureID = 0;
	GLuint m_shineTextureID = 0;

	void InitTextures();
	void CleanTextures();
};
