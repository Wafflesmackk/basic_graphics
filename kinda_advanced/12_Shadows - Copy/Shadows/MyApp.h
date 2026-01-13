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
#include <map>

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
	int parentEdge = -1;
	float pivot = 0.0f;

	std::vector<int> children;

};





struct PolyMeshGPU
{
	GLuint vao = 0;
	GLuint vbo = 0;
	GLsizei vertexCount = 0;

};


struct PolyParseState
{
	int activeFace = -1;
	int lastN = -1;

	std::map<std::pair<int, int>, float> defaultPivot;
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
	void RenderBuilderGUI();

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

	int m_width = 640, m_height = 480;

protected:
	void SetupDebugCallback();

	std::vector<FaceNode> m_faces;
	int m_activeFace = -1;

	bool m_folded = false;
	float m_foldT = 0.0f;
	bool  m_autoFold = false;
	float m_foldSpeed = 0.5f;
	int   m_foldDir = 1;

	PolyParseState m_guiPolyState;

	bool m_started = false;
	int gui_edge = 0;
	int gui_N = 4;
	float gui_pivot = glm::half_pi<float>();

	int gui_pivot_n1 = 4;
	int gui_pivot_n2 = 4;
	int gui_pivot_n3 = 4;
	float gui_pivot_val = glm::half_pi<float>();
	bool gui_useN = true;
	bool gui_usePivot = true;

	GLuint m_programID = 0;

	GLuint m_vaoID = 0;
	GLuint m_vboID = 0;

	GLuint m_SamplerID = 0;
	GLuint m_metalTextureID = 0;
	GLuint m_woodTextureID = 0;




	std::map<int, PolyMeshGPU> m_polyMeshes;

	std::map<std::pair<int, int>, float> m_defaultPivot;

	void InitTextures();
	void CleanTextures();

	void InitShaders();
	void CleanShaders();

	void InitBuffers();
	void CleanBuffers();

	bool LoadPoly(const std::string& filename);

	std::vector<Vertex> MakePolygon(int N);
	void DrawFaceRecursive(
		const std::vector<FaceNode>& faces,
		int id,
		const glm::mat4& parentWorld,
		GLint mvpLoc,
		const glm::mat4& VP
	);

	glm::vec3 EdgePoint(int edge, int N);

	glm::mat4 FoldTransform(const FaceNode& f);
	glm::vec3 EdgeDir(int edge, int N);

	void ComputeTransformsRecursive(
		int id,
		const glm::mat4& parentWorld,
		std::vector<glm::mat4>& outWorld
	);
	static float InteriorAngle(int N);

	glm::mat4 PlaceFaceNextToParent(const FaceNode& parent, const FaceNode& child);

	const PolyMeshGPU& GetOrCreatePolyMesh(int N);

	static float PivotFromVertex(int N1, int N2, int N3);
	float GetDefaultPivot(int N1, int N2, const PolyParseState& state) const;
	void  SetDefaultPivot(int N1, int N2, float pivot);

	void ExecutePolyCommand(
		const std::string& cmd,
		const std::vector<float>& args,
		PolyParseState& state
	);
	float RadiusForSide(int N, float side) const;
};