#include "MyApp.h"
#include "ObjParser.h"
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
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_FALSE);
		glDebugMessageCallback(SDL_GLDebugMessageCallback, nullptr);
	}
}

void CMyApp::InitShaders()
{
	m_programID = glCreateProgram();
	AttachShader(m_programID, GL_VERTEX_SHADER, "Shaders/Vert_PosNormTex.vert");
	AttachShader(m_programID, GL_FRAGMENT_SHADER, "Shaders/Frag_ZH.frag");
	LinkProgram(m_programID);
}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programID);
}

MeshObject<Vertex> createCube()
{
	MeshObject<Vertex> mesh;

	mesh.vertexArray = {
		// Elülső oldala 
		{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // Bal lent 
		{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // Jobb lent 
		{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},   // Jobb fent 
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // Bal fent 

		// Hátulsó oldala 
		{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // Bal lent 
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},// Jobb lent 
		{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // Jobb fent 
		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},  // Bal fent 

		// Bal oldala 
		{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},// Bal lent 
		{{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // Jobb lent 
		{{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // Jobb fent 
		{{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // Bal fent 

		// Jobb oldala 
		{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // Bal lent 
		{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // Jobb lent 
		{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // Jobb fent 
		{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},   // Bal fent 

		// Felső oldala 
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // Bal lent 
		{{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},   // Jobb lent 
		{{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},  // Jobb fent 
		{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // Bal fent 

		// Alsó oldala 
		{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},// Bal lent 
		{{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // Jobb lent 
		{{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},  // Jobb fent 
		{{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // Bal fent 
	};

	mesh.indexArray = {// Elülső oldala 
					   0, 1, 2, 2, 3, 0,
					   // Hátulsó oldala 
					   4, 5, 6, 6, 7, 4,
					   // Bal oldala 
					   8, 9, 10, 10, 11, 8,
					   // Jobb oldala 
					   12, 13, 14, 14, 15, 12,
					   // Felső oldala 
					   16, 17, 18, 18, 19, 16,
					   // Alsó oldala 
					   20, 21, 22, 22, 23, 20 };

	return mesh;
}

void CMyApp::InitGeometry()
{
	const std::initializer_list<VertexAttributeDescriptor> vertexAttribList =
	{
		{0, offsetof(Vertex, position), 3, GL_FLOAT},
		{1, offsetof(Vertex, normal), 3, GL_FLOAT},
		{2, offsetof(Vertex, texcoord), 2, GL_FLOAT},
	};

	m_cubeGPU = CreateGLObjectFromMesh(createCube(), vertexAttribList);

	MeshObject<Vertex> mesh;
	mesh = ObjParser::parse("Assets/torso.obj");
	m_robotTorsoGPU = CreateGLObjectFromMesh(mesh, vertexAttribList);

	mesh = ObjParser::parse("Assets/head.obj");
	m_robotHeadGPU = CreateGLObjectFromMesh(mesh, vertexAttribList);
	mesh = ObjParser::parse("Assets/leg.obj");
	m_robotLegGPU = CreateGLObjectFromMesh(mesh, vertexAttribList);
	m_rightUpperArmGPU = CreateGLObjectFromMesh(ObjParser::parse("Assets/right_upper_arm.obj"), vertexAttribList);
	m_rightLowerArmGPU = CreateGLObjectFromMesh(ObjParser::parse("Assets/right_lower_arm.obj"), vertexAttribList);
	m_leftUpperArmGPU = CreateGLObjectFromMesh(ObjParser::parse("Assets/left_upper_arm.obj"), vertexAttribList);
	m_leftLowerArmGPU = CreateGLObjectFromMesh(ObjParser::parse("Assets/left_lower_arm.obj"), vertexAttribList);
}

void CMyApp::CleanGeometry()
{
	CleanOGLObject(m_cubeGPU);
	CleanOGLObject(m_robotTorsoGPU);
	CleanOGLObject(m_robotHeadGPU);
	CleanOGLObject(m_robotLegGPU);
	CleanOGLObject(m_rightUpperArmGPU);
	CleanOGLObject(m_rightLowerArmGPU);
	CleanOGLObject(m_leftUpperArmGPU);
	CleanOGLObject(m_leftLowerArmGPU);
}

void CMyApp::InitTextures()
{
	glCreateSamplers(1, &m_SamplerID);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ImageRGBA woodImage = ImageFromFile("Assets/Wood_Table_Texture.png");

	glCreateTextures(GL_TEXTURE_2D, 1, &m_woodTextureID);
	glTextureStorage2D(m_woodTextureID, NumberOfMIPLevels(woodImage), GL_RGBA8, woodImage.width, woodImage.height);
	glTextureSubImage2D(m_woodTextureID, 0, 0, 0, woodImage.width, woodImage.height, GL_RGBA, GL_UNSIGNED_BYTE, woodImage.data());

	glGenerateTextureMipmap(m_woodTextureID);

	ImageRGBA robotImage = ImageFromFile("Assets/Robot_Texture.png");

	glCreateTextures(GL_TEXTURE_2D, 1, &m_robotTextureID);
	glTextureStorage2D(m_robotTextureID, NumberOfMIPLevels(robotImage), GL_RGBA8, robotImage.width, robotImage.height);
	glTextureSubImage2D(m_robotTextureID, 0, 0, 0, robotImage.width, robotImage.height, GL_RGBA, GL_UNSIGNED_BYTE, robotImage.data());

	glGenerateTextureMipmap(m_robotTextureID);

	ImageRGBA shineImage = ImageFromFile("Assets/shine.png");

	glCreateTextures(GL_TEXTURE_2D, 1, &m_shineTextureID);
	glTextureStorage2D(m_shineTextureID, NumberOfMIPLevels(robotImage), GL_RGBA8, robotImage.width, robotImage.height);
	glTextureSubImage2D(m_shineTextureID, 0, 0, 0, robotImage.width, robotImage.height, GL_RGBA, GL_UNSIGNED_BYTE, robotImage.data());



}

void CMyApp::CleanTextures()
{
	glDeleteTextures(1, &m_woodTextureID);
	glDeleteTextures(1, &m_robotTextureID);
	glDeleteTextures(1, &m_shineTextureID);


	glDeleteSamplers(1, &m_SamplerID);
}

static bool HitPlane(const Ray& ray, const glm::vec3& planeQ, const glm::vec3& planeI, const glm::vec3& planeJ, Intersection& result)
{
	// sík parametrikus egyenlete: E: parametric equation of a plane:  palneQ + u * planeI + v * planeJ
	glm::mat3 A(-ray.direction, planeI, planeJ);
	glm::vec3 B = ray.origin - planeQ;

	if (fabsf(glm::determinant(A)) < 1e-6) return false;
	glm::vec3 X = glm::inverse(A) * B;

	if (X.x < 0.0) {
		return false;
	}
	result.t = X.x;
	result.uv.x = X.y;
	result.uv.y = X.z;

	return true;
}

bool CMyApp::intersectTable(const Ray& ray, const glm::vec3& tablePos, const glm::vec3 tableSize, glm::vec3& result)
{
	glm::vec3 planeQ = tablePos + 0.5f * glm::vec3(-tableSize.x, tableSize.y, -tableSize.z);
	glm::vec3 planeI = glm::vec3(tableSize.x, 0.0, 0.0);
	glm::vec3 planeJ = glm::vec3(0.0, 0.0, tableSize.z);
	Intersection inter;
	if (HitPlane(ray, planeQ, planeI, planeJ, inter) &&
		(inter.uv.x >= 0.0f && inter.uv.x <= 1.0f) &&
		(inter.uv.y >= 0.0f && inter.uv.y <= 1.0f)) {


		const glm::vec2& uv = inter.uv;
		result = planeQ + planeI * uv.x + planeJ * uv.y;
		return true;
	}
	return false;
}

bool CMyApp::Init()
{
	SetupDebugCallback();

	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	InitShaders();
	InitGeometry();
	InitTextures();

	// egyéb inicializálás

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hátrafelé néző lapok eldobását 
	glCullFace(GL_BACK);    // GL_BACK: a kamerától "elfelé" néző lapok, GL_FRONT: a kamera felé néző lapok 

	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás) 

	// kamera 
	m_camera.SetView(
		glm::vec3(0.0, 60.0, 150.0),  // honnan nézzük a színteret  - eye
		glm::vec3(0.0, 0.0, 0.0),  // a színtér melyik pontját nézzük  - at
		glm::vec3(0.0, 1.0, 0.0)); // felfelé mutató irány a világban  - up

	m_cameraManipulator.SetCamera(&m_camera);

	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanGeometry();
	CleanTextures();
}

Ray CMyApp::CalculatePixelRay(glm::vec2 pixel) const
{
	// NDC koordináták kiszámítása 
	glm::vec3 pickedNDC = glm::vec3(
		2.0f * (pixel.x + 0.5f) / m_windowSize.x - 1.0f,
		1.0f - 2.0f * (pixel.y + 0.5f) / m_windowSize.y, 0.0f);

	// A világ koordináták kiszámítása az inverz ViewProj mátrix segítségével
	glm::vec4 pickedWorld = glm::inverse(m_camera.GetViewProj()) * glm::vec4(pickedNDC, 1.0f);
	pickedWorld /= pickedWorld.w; // homogén osztás 
	Ray ray;

	// Raycasting kezdőpontja a kamera pozíciója 
	ray.origin = m_camera.GetEye();
	// Iránya a kamera pozíciójából a kattintott pont világ koordinátái felé 
	// FIGYELEM: NEM egység hosszúságú vektor! 
	ray.direction = glm::vec3(pickedWorld) - ray.origin;
	return ray;
}



void CMyApp::Update(const SUpdateInfo& updateInfo)
{
	m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;

	if (m_isTopCamera) {
		glm::vec3 camPosInObjSpace = glm::vec3(0.0, 60.0f, -150.0f);
		glm::mat4 matCam = glm::identity<glm::mat4>();

		glm::vec3 camPos = glm::vec3(matCam * glm::vec4(camPosInObjSpace, 1.0));

		m_camera.SetView(camPos, glm::vec3(TABLE_POS), glm::vec3(0.0, 1.0, 0.0));
	}
	else  m_cameraManipulator.Update(updateInfo.DeltaTimeInSec);


	if (m_IsPicking) {
		// sugár indítása a kattintott pixelen át 
		Ray ray = CalculatePixelRay(glm::vec2(m_PickedPixel.x, m_PickedPixel.y));

		m_IsPicking = false;
	}

	m_robotTransform = glm::identity<glm::mat4>();
	m_robotBaseTransform = m_robotTransform * glm::translate(TORSO_POS);

	m_robotHeadTransform = m_robotBaseTransform *
		glm::translate(RELATIVE_HEAD_POS) *
		glm::rotate(m_robotHeadLeftRightRot, glm::vec3(0.0, -1.0, 0.0)) *
		glm::rotate(m_robotHeadUpDownRot, glm::vec3(1.0, 0.0, 0.0));

	m_robotLeftLegTransform = m_robotBaseTransform *
		glm::translate(RELATIVE_LEFT_LEG_POS) *
		glm::translate(CalculateLegAnim(updateInfo.ElapsedTimeInSec, 0.0f));

	m_robotRightLegTransform = m_robotBaseTransform *
		glm::translate(RELATIVE_RIGHT_LEG_POS) *
		glm::translate(CalculateLegAnim(updateInfo.ElapsedTimeInSec, glm::pi<float>()));
	m_rightUpperArmT = m_robotBaseTransform *
		glm::translate(RELATIVE_RIGHT_ARM_UPPER) *
		glm::rotate(mAngleArmUpper, glm::vec3(1, 0, 0));
	m_rightLowerArmT = m_rightUpperArmT *
		glm::translate(RELATIVE_RIGHT_ARM_LOWER) *
		glm::rotate(mAngleArmLower, glm::vec3(1, 0, 0));
	m_leftUpperArmT = m_robotBaseTransform *
		glm::translate(RELATIVE_LEFT_ARM_UPPER) *
		glm::rotate(mAngleArmUpperL, glm::vec3(1, 0, 0));
	m_leftLowerArmT = m_leftUpperArmT *
		glm::translate(RELATIVE_LEFT_ARM_LOWER) *
		glm::rotate(mAngleArmLowerL, glm::vec3(1, 0, 0));


	Ray headRay = CalculateRobotHeadArray();
	glm::vec3 intersectionPoint;
	if (intersectTable(headRay, TABLE_POS, TABLE_SIZE, intersectionPoint)) {
		m_lightPosition2 = glm::vec4(intersectionPoint + glm::vec3(0.0, 1.0, 0.0), 1.0);
	}
	else {
		m_lightPosition2 = glm::vec4(0.0);
	}



}

void CMyApp::SetCommonUniforms()
{
	// - Uniform paraméterek 

	// - view és projekciós mátrix 
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	// - Fényforrások beállítása 
	glUniform3fv(ul("cameraPosition"), 1, glm::value_ptr(m_camera.GetEye()));
	glUniform4fv(ul("lightPosition"), 1, glm::value_ptr(m_lightPosition));
	glUniform4fv(ul("lightPosition2"), 1, glm::value_ptr(m_lightPosition2));
	glUniform3fv(ul("La"), 1, glm::value_ptr(m_La));
	glUniform3fv(ul("Ld"), 1, glm::value_ptr(m_Ld));
	glUniform3fv(ul("Ls"), 1, glm::value_ptr(m_Ls));
	glUniform1f(ul("lightConstantAttenuation"), m_lightConstantAttenuation);
	glUniform1f(ul("lightLinearAttenuation"), m_lightLinearAttenuation);
	glUniform1f(ul("lightQuadraticAttenuation"), m_lightQuadraticAttenuation);
	if (m_lightPosition2.w != 0.0f) {
		glUniform3fv(ul("La2"), 1, glm::value_ptr(m_La2));
		glUniform3fv(ul("Ld2"), 1, glm::value_ptr(m_Ld2));
		glUniform3fv(ul("Ls2"), 1, glm::value_ptr(m_Ls2));
	}
	else {
		glUniform3fv(ul("La2"), 1, glm::value_ptr(glm::vec3(0.0f)));
		glUniform3fv(ul("Ld2"), 1, glm::value_ptr(glm::vec3(0.0f)));
		glUniform3fv(ul("Ls2"), 1, glm::value_ptr(glm::vec3(0.0f)));
	}
}

void CMyApp::DrawObject(OGLObject& obj, const glm::mat4& world) {
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(ul("worldInvTransp"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(world))));
	glBindVertexArray(obj.vaoID);
	glDrawElements(GL_TRIANGLES, obj.count, GL_UNSIGNED_INT, nullptr);
}

glm::vec3 CMyApp::CalculateLegAnim(float time, float animOffset)
{
	float legAnim = -((time * (glm::two_pi<float>()) * LEG_SPEED) + animOffset);
	return glm::vec3(0.0, glm::clamp(sinf(legAnim), 0.0f, 1.0f), cosf(legAnim) * LEG_SPREAD);
}

void CMyApp::RenderTable()
{
	glUniform1i(ul("textureImage"), 0);
	glUniform1i(ul("state"), SHADER_STATE_TABLE);

	glBindTextureUnit(0, m_woodTextureID);
	glBindSampler(0, m_SamplerID);

	glm::mat4 tableTransform = glm::translate(TABLE_POS);

	DrawObject(m_cubeGPU, tableTransform * glm::scale(TABLE_SIZE));


	glUniform1i(ul("state"), SHADER_STATE_DEFAULT);

	glm::vec3 legPos(60.0f, -(TABLE_LEG_SIZE.y + TABLE_SIZE.y) / 2.0f, 60.0f);

	DrawObject(m_cubeGPU, tableTransform * glm::translate(legPos) * glm::scale(TABLE_LEG_SIZE));

	glm::vec3 legPos2(-60.0f, -(TABLE_LEG_SIZE.y + TABLE_SIZE.y) / 2.0f, 60.0f);

	DrawObject(m_cubeGPU, tableTransform * glm::translate(legPos2) * glm::scale(TABLE_LEG_SIZE));

	glm::vec3 legPos3(60.0f, -(TABLE_LEG_SIZE.y + TABLE_SIZE.y) / 2.0f, -60.0f);

	DrawObject(m_cubeGPU, tableTransform * glm::translate(legPos3) * glm::scale(TABLE_LEG_SIZE));

	glm::vec3 legPos4(-60.0f, -(TABLE_LEG_SIZE.y + TABLE_SIZE.y) / 2.0f, -60.0f);

	DrawObject(m_cubeGPU, tableTransform * glm::translate(legPos4) * glm::scale(TABLE_LEG_SIZE));

}
Ray CMyApp::CalculateRobotHeadArray() {
	Ray headRay = {};
	headRay.origin = glm::vec3(m_robotHeadTransform * glm::vec4(AVG_EYE_POS_OFFSET, 1.0));
	headRay.direction = glm::vec3(m_robotHeadTransform * glm::vec4(0.0, 0.0, 1.0, 0.0));
	return headRay;
}

void CMyApp::RenderRobot()
{
	glUniform1i(ul("textureImage"), 0);
	glUniform1i(ul("textureShining"), 1);
	glUniform1i(ul("state"), SHADER_STATE_ROBOT);

	glBindTextureUnit(0, m_robotTextureID);
	glBindSampler(0, m_SamplerID);

	glBindTextureUnit(1, m_shineTextureID);
	glBindSampler(1, m_SamplerID);

	DrawObject(m_robotTorsoGPU, m_robotBaseTransform);
	DrawObject(m_robotHeadGPU, m_robotHeadTransform);

	DrawObject(m_robotLegGPU, m_robotLeftLegTransform);
	DrawObject(m_robotLegGPU, m_robotRightLegTransform);

	DrawObject(m_rightUpperArmGPU, m_rightUpperArmT);
	DrawObject(m_rightLowerArmGPU, m_rightLowerArmT);
	DrawObject(m_leftUpperArmGPU, m_leftUpperArmT);
	DrawObject(m_leftLowerArmGPU, m_leftLowerArmT);

}

void CMyApp::Render()
{
	// töröljük a framepuffert (GL_COLOR_BUFFER_BIT)...
	// ... és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);

	SetCommonUniforms();

	RenderTable();
	RenderRobot();

	glBindTextureUnit(0, 0);
	glBindSampler(0, 0);

	glBindVertexArray(0);
	// shader kikapcsolása 
	glUseProgram(0);
}

void CMyApp::RenderGUI()
{
	//ImGui::ShowDemoWindow();
	ImGui::SliderAngle("Head Left Right", &m_robotHeadLeftRightRot, -90, 90);
	ImGui::SliderAngle("Head Up Down", &m_robotHeadUpDownRot, -130.f, 130.f);
	ImGui::SliderAngle("mAngleArmUpper", &mAngleArmUpper);
	ImGui::SliderAngle("mAngleArmLower", &mAngleArmLower, -100.f, 100.f);
	ImGui::SliderAngle("mAngleArmUpperL", &mAngleArmUpperL);
	ImGui::SliderAngle("mAngleArmLowerL", &mAngleArmLowerL, -100.f, 100.f);
	ImGui::Checkbox("top cam", &m_isTopCamera);
}

// https://wiki.libsdl.org/SDL2/SDL_KeyboardEvent
// https://wiki.libsdl.org/SDL2/SDL_Keysym
// https://wiki.libsdl.org/SDL2/SDL_Keycode
// https://wiki.libsdl.org/SDL2/SDL_Keymod

void CMyApp::KeyboardDown(const SDL_KeyboardEvent& key)
{
	if (key.repeat == 0) // Először lett megnyomva 
	{
		if (key.keysym.sym == SDLK_F5 && key.keysym.mod & KMOD_CTRL)
		{
			CleanShaders();
			InitShaders();
		}
		if (key.keysym.sym == SDLK_F1)
		{
			GLint polygonModeFrontAndBack[2] = {};
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml
			glGetIntegerv(GL_POLYGON_MODE, polygonModeFrontAndBack); // Kérdezzük le a jelenlegi polygon módot! Külön adja a front és back módokat. 
			GLenum polygonMode = (polygonModeFrontAndBack[0] != GL_FILL ? GL_FILL : GL_LINE); // Váltogassuk FILL és LINE között! 
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
			glPolygonMode(GL_FRONT_AND_BACK, polygonMode); // Állítsuk be az újat! 
		}

		if (key.keysym.sym == SDLK_LCTRL || key.keysym.sym == SDLK_RCTRL)
		{
			m_IsCtrlDown = true;
		}
	}
	m_cameraManipulator.KeyboardDown(key);
}

void CMyApp::KeyboardUp(const SDL_KeyboardEvent& key)
{
	m_cameraManipulator.KeyboardUp(key);
	if (key.keysym.sym == SDLK_LCTRL || key.keysym.sym == SDLK_RCTRL)
	{
		m_IsCtrlDown = false;
	}
}

// https://wiki.libsdl.org/SDL2/SDL_MouseMotionEvent

void CMyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	m_cameraManipulator.MouseMove(mouse);
}

// https://wiki.libsdl.org/SDL2/SDL_MouseButtonEvent

void CMyApp::MouseDown(const SDL_MouseButtonEvent& mouse)
{
	if (m_IsCtrlDown)
	{
		m_IsPicking = true;
	}
	m_PickedPixel = { mouse.x, mouse.y };
}

void CMyApp::MouseUp(const SDL_MouseButtonEvent& mouse)
{
}

// https://wiki.libsdl.org/SDL2/SDL_MouseWheelEvent

void CMyApp::MouseWheel(const SDL_MouseWheelEvent& wheel)
{
	m_cameraManipulator.MouseWheel(wheel);
}

// a két paraméterben az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	m_windowSize = glm::uvec2(_w, _h);
	m_camera.SetAspect(static_cast<float>(_w) / _h);
}

// Le nem kezelt, egzotikus esemény kezelése
// https://wiki.libsdl.org/SDL2/SDL_Event

void CMyApp::OtherEvent(const SDL_Event& ev)
{

}
