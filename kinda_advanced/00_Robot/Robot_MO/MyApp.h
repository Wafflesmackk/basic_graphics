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

struct RobotState
{
	static const int ROBOT_STATE_STAND    = 0; // Nem mozog, 
	static const int ROBOT_STATE_ROTATING = 1; // forog, 
	static const int ROBOT_STATE_MOVING   = 2; // mozog 

	int state = ROBOT_STATE_STAND; 

	glm::vec3 position = glm::vec3(0, 0, 0);
	float rotation = 0;

	float movementSpeed = 20.0f;
	float rotationSpeed = 90.0f;

	glm::vec3 desiredPosition = glm::vec3(0, 0, 0);
	float desiredRotation = 0;
	
	// a láb csak akkor mozog, ha a robot nem áll 
	float legTime = 0.0f;
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

	//
	// Adat változók
	//

	float m_ElapsedTimeInSec = 0.0f;

    // Robot
	float m_robotLeftArmUpperRot = 0.0;
	float m_robotLeftArmLowerRot = 0.0;
	float m_robotRightArmUpperRot = 0.0;
	float m_robotRightArmLowerRot = 0.0;

	float m_robotHeadUpDownRot = 0.0;
	float m_robotHeadLeftRightRot = 0.0;

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

	glm::mat4 m_robotTransform;
	glm::mat4 m_robotBaseTransform;
    glm::mat4 m_robotHeadTransform;
    glm::mat4 m_robotLeftArmUpperTransform;
    glm::mat4 m_robotLeftArmLowerTransform;
    glm::mat4 m_robotRightArmUpperTransform;
    glm::mat4 m_robotRightArmLowerTransform;
    glm::mat4 m_robotLeftLegTransform;
    glm::mat4 m_robotRightLegTransform;

	bool m_isTopCamera = false;

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

	//
	// OpenGL-es dolgok
	// 

	const int SHADER_STATE_DEFAULT = 0;
	const int SHADER_STATE_ROBOT   = 1;
	const int SHADER_STATE_TABLE   = 2;

	const int SHADER_DEBUG_NONE = 0;
	const int SHADER_DEBUG_NORMALS = 1;
	const int SHADER_DEBUG_SHADING = 2;
	const int SHADER_DEBUG_DIFFUSE_TEXTURE = 3;
	const int SHADER_DEBUG_SHINE_TEXTURE = 4;

	int m_shaderDebugState = SHADER_DEBUG_NONE;

	// Shaderekhez szükséges változók 
	GLuint m_programID = 0; // shaderek programja 

	// Fényforrás - ...
	glm::vec4 m_lightPosition = glm::vec4(1.0f, 2.0f, 2.0f, 0.0f);

	glm::vec3 m_La = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 m_Ld = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 m_Ls = glm::vec3(1.0, 1.0, 1.0);


	glm::vec4 m_lightPosition2 = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);

	glm::vec3 m_La2 = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 m_Ld2 = glm::vec3(1.0, 0.0, 0.0);
	glm::vec3 m_Ls2 = glm::vec3(1.0, 0.0, 0.0);

	float m_lightConstantAttenuation = 1.0f;
	float m_lightLinearAttenuation = 0.025f;
	float m_lightQuadraticAttenuation = 0.001f;

	// ...  és anyagjellemzők 
	glm::vec3 m_Ka = glm::vec3(1.0);
	glm::vec3 m_Kd = glm::vec3(1.0);
	glm::vec3 m_Ks = glm::vec3(1.0);

	float m_Shininess = 16.0;

	// Shaderek inicializálása, és törlése 
	void InitShaders();
	void CleanShaders();

	// Geometriával kapcsolatos változók 

	void SetCommonUniforms();
	void DrawObject(OGLObject& obj, const glm::mat4& world);
	void RenderTable();
	OGLObject m_cubeGPU = {};

	void CalculateRobotMovementData(glm::vec3 desiredPosition);
	void CalculateRobotPosition(float deltaTime);

	static glm::vec3 CalculateLegAnim(float time, float animOffset);
	Ray CalculateRobotHeadRay() const;

	void RenderRobot();

	RobotState m_robotState;
	OGLObject m_robotHeadGPU = {};
	OGLObject m_robotTorsoGPU = {};
	OGLObject m_robotRightArmUpperGPU = {};
	OGLObject m_robotRightArmLowerGPU = {};
	OGLObject m_robotLeftArmUpperGPU = {};
	OGLObject m_robotLeftArmLowerGPU = {};
	OGLObject m_robotLegGPU = {};

	// Geometria inicializálása, és törlése 
	void InitGeometry();
	void CleanGeometry();


	bool m_drawRobotGoal = false;
	float m_robotCubeGoalSize = 10.0f;

	// Textúrázás, és változói 
	GLuint m_SamplerID = 0;

	GLuint m_woodTextureID = 0;
	GLuint m_robotTextureID = 0;
	GLuint m_shineTextureID = 0;

	void InitTextures();
	void CleanTextures();

	static bool intersectTableTop( const Ray& ray, const glm::vec3& tablePos, const glm::vec3& tableSize, glm::vec3& result );
};