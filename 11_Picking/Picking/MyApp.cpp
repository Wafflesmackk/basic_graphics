#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"
#include "ObjParser.h"

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
	AttachShader(m_programID, GL_FRAGMENT_SHADER, "Shaders/Frag_LightingAndIndicator_TODO.frag");
	LinkProgram(m_programID);
}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programID);
}

void CMyApp::InitGeometry()
{

	const std::initializer_list<VertexAttributeDescriptor> vertexAttribList =
	{
		{ 0, offsetof(Vertex, position), 3, GL_FLOAT },
		{ 1, offsetof(Vertex, normal), 3, GL_FLOAT },
		{ 2, offsetof(Vertex, texcoord), 2, GL_FLOAT },
	};

	// Quad 

	MeshObject<Vertex> quadMeshCPU;

	quadMeshCPU.vertexArray =
	{
		{ glm::vec3(-1, -1, 0),glm::vec3(0.0, 0.0, 1.0), glm::vec2(0.0, 0.0) },
		{ glm::vec3(1, -1, 0),glm::vec3(0.0, 0.0, 1.0), glm::vec2(1.0, 0.0) },
		{ glm::vec3(-1,  1, 0),glm::vec3(0.0, 0.0, 1.0), glm::vec2(0.0, 1.0) },
		{ glm::vec3(1,  1, 0),glm::vec3(0.0, 0.0, 1.0), glm::vec2(1.0, 1.0) }
	};

	quadMeshCPU.indexArray =
	{
		0, 1, 2,
		1, 3, 2
	};

	m_quadGPU = CreateGLObjectFromMesh(quadMeshCPU, vertexAttribList);

	// Gömb 

	MeshObject<Vertex> sphereMeshCPU = ObjParser::parse("Assets/MarbleBall.obj");

	m_sphereGPU = CreateGLObjectFromMesh(sphereMeshCPU, vertexAttribList);
}

void CMyApp::CleanGeometry()
{
	CleanOGLObject(m_quadGPU);
	CleanOGLObject(m_sphereGPU);
}

void CMyApp::InitTextures()
{
	glCreateSamplers(1, &m_SamplerID);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// diffúz textúra 
	ImageRGBA woodImage = ImageFromFile("Assets/Wood_Table_Texture.png");

	glCreateTextures(GL_TEXTURE_2D, 1, &m_woodTextureID);
	glTextureStorage2D(m_woodTextureID, NumberOfMIPLevels(woodImage), GL_RGBA8, woodImage.width, woodImage.height);
	glTextureSubImage2D(m_woodTextureID, 0, 0, 0, woodImage.width, woodImage.height, GL_RGBA, GL_UNSIGNED_BYTE, woodImage.data());

	glGenerateTextureMipmap(m_woodTextureID);

	ImageRGBA sphereImage = ImageFromFile("Assets/MarbleBall.png");

	glCreateTextures(GL_TEXTURE_2D, 1, &m_sphereTextureID);
	glTextureStorage2D(m_sphereTextureID, NumberOfMIPLevels(sphereImage), GL_RGBA8, sphereImage.width, sphereImage.height);
	glTextureSubImage2D(m_sphereTextureID, 0, 0, 0, sphereImage.width, sphereImage.height, GL_RGBA, GL_UNSIGNED_BYTE, sphereImage.data());

	glGenerateTextureMipmap(m_sphereTextureID);
}

void CMyApp::CleanTextures()
{
	glDeleteTextures(1, &m_woodTextureID);
	glDeleteTextures(1, &m_sphereTextureID);

	glDeleteSamplers(1, &m_SamplerID);
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
		glm::vec3(10.0, 50.0, 50.0),   // honnan nézzük a színteret  - eye
		glm::vec3(0.0, 0.0, 0.0),   // a színtér melyik pontját nézzük  - at
		glm::vec3(0.0, 1.0, 0.0));  // felfelé mutató irány a világban  - up

	m_cameraManipulator.SetCamera(&m_camera);

	// Gömbök inicializálása 
	m_Spheres = { {glm::vec3(-8.0f, 3.0f, -8.0f), 3.0f}, {glm::vec3(8.0f, 0.8f,  8.0f), 0.8f} };
	m_spheresWorldTransforms.resize(m_Spheres.size(), glm::identity<glm::mat4>());

	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanGeometry();
	CleanTextures();
}

void CMyApp::GetRayDirAndOrigin(glm::vec3& rayDir, glm::vec3& rayOrigin)
{
	// TODO:  Sugár indítás implementációja 
	/*rayDir = glm::vec3(0.0f, -1.0f, 0.0f);
	rayOrigin = glm::vec3(0.0f, 1.0f, 0.0f);*/


	glm::vec3 m_PickedNDC(
		2.0f * (m_PickedPixel.x + 0.5f) / m_windowSize.x - 1.0f,
		1.0f - 2.0f * (m_PickedPixel.y + 0.5f) / m_windowSize.y,
		0.0f);
	glm::vec4 pickedWorld = glm::inverse(m_camera.GetViewProj()) * glm::vec4(m_PickedNDC, 1.0f);
	pickedWorld /= pickedWorld.w;

	rayOrigin = m_camera.GetEye();
	rayDir = glm::vec3(pickedWorld) - rayOrigin;
}

static bool HitSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& sphereCenter, float sphereRadius, float& t)
{
	glm::vec3 p_m_c = rayOrigin - sphereCenter;
	float a = glm::dot(rayDir, rayDir);
	float b = 2.0f * glm::dot(rayDir, p_m_c);
	float c = glm::dot(p_m_c, p_m_c) - sphereRadius * sphereRadius;

	float discriminant = b * b - 4.0f * a * c;

	if (discriminant < 0.0f)
	{
		return false;
	}

	float sqrtDiscriminant = sqrtf(discriminant);

	// Mivel 2*a, és sqrt(D) mindig pozitívak, ezért tudjuk, hogy t0 < t1
	float t0 = (-b - sqrtDiscriminant) / (2.0f * a);
	float t1 = (-b + sqrtDiscriminant) / (2.0f * a);

	if (t1 < 0.0f) // mivel t0 < t1, ha t1 negatív, akkor t0 is az 
	{
		return false;
	}

	if (t0 < 0.0f)
	{
		t = t1;
	}
	else
	{
		t = t0;
	}

	return true;
}

static bool HitPlane(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
	const glm::vec3& planeQ, const glm::vec3& planeI, const glm::vec3& planeJ, float& t, glm::vec2& uv)
{
	// TODO:  Sugár és sík metszésének implementálása 
	glm::mat3 A(-rayDir, planeI, planeJ);
	glm::vec3 B = rayOrigin - planeQ;

	if (fabsf(glm::determinant(A)) < 1e-6) {
		return false;
	};
	glm::vec3 X = glm::inverse(A) * B;

	t = X.x;
	uv.x = X.y;
	uv.y = X.z;

	return t >= 0.0f;
}

int CMyApp::GetPickedObject(const glm::vec3& rayOrigin, const glm::vec3& rayDir, float& t)
{
	int pickedObject = -1;
	t = 1e16f;

	for (int objIdx = 0; objIdx < m_Spheres.size(); objIdx++) {
		float t_sphere = 0.0f;

		if (HitSphere(rayOrigin, rayDir, m_Spheres[objIdx].Position, m_Spheres[objIdx].Radius, t_sphere)) {
			if (t_sphere < t) {
				t = t_sphere;
				pickedObject = objIdx;
			}
		}
	}

	return pickedObject;
}

void CMyApp::Update(const SUpdateInfo& updateInfo)
{
	m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;

	m_cameraManipulator.Update(updateInfo.DeltaTimeInSec);


	// Kiválasztás 
	if (m_IsCtrlDown || m_IsPicking)
	{
		// Raycasting
		glm::vec3 rayDir;
		glm::vec3 rayOrigin;

		GetRayDirAndOrigin(rayDir, rayOrigin);

		float t = 0.0f; // "távolság" a találati pontig 

		// Az esetlegesen kattintott gömb kivalasztasa 
		int pickedObject = GetPickedObject(rayOrigin, rayDir, t);

		// Sík metszés vizsgálata 

		glm::vec3 planeQ = TABLE_POS + glm::vec3(-TABLE_SIZE, 0.0f, TABLE_SIZE);
		glm::vec3 planeI = glm::vec3(2.0f * TABLE_SIZE, 0.0f, 0.0f);
		glm::vec3 planeJ = glm::vec3(0.0f, 0.0f, -2.0f * TABLE_SIZE);
		bool tableHit = HitPlane(rayOrigin, rayDir, planeQ, planeI, planeJ, t, m_tableIndicatorUV);

		// Ha kiválasztunk éppen,  ...
		if (m_IsPicking)
		{
			if (pickedObject > -1) //...,  és eltaláltunk egy gömböt, akkor azt válasszuk ki 
			{
				m_SelectedObject = pickedObject;
			}
			// Abban az esetben, ha nem találtunk egy gömböt sem, de van már gömb kiválasztva,
			// és a síkot találtuk el, helyezzük át a gömböt a találati pont fölé!
			else if (m_SelectedObject > -1 && tableHit)
			{
				glm::vec3 HitPos = planeQ + m_tableIndicatorUV.x * planeI + m_tableIndicatorUV.y * planeJ; // vagy  HitPos = rayOrigin + t * rayDir;
				m_Spheres[m_SelectedObject].Position = HitPos // Találati pont a síkon,... 
					+ glm::vec3(0.0f, m_Spheres[m_SelectedObject].Radius, 0.0f); // ...  így helyezzük a gömböt sugárnyival fentebb 
				m_SelectedObject = -1; // Ne legyen semmi kiválasztva! 
			}
		}

		// Akkor jelenjen meg az asztalon idikátor, ha nem kattintunk éppen, 
		// de van kiválasztott gömb, és az asztalt is eltaláltuk.
		m_tableIndicator = tableHit && !m_IsPicking && m_SelectedObject > -1;
		// Az indikátor sugarat is beállítjuk, ha az asztalt találtuk el.
		// Ez UV kooridnátában értendő.
		m_tableIndicatorR = m_tableIndicator ? m_Spheres[m_SelectedObject].Radius / (2.0f * TABLE_SIZE) : -1.0f;

		m_IsPicking = false;
	}
	else
	{
		// Ha nincs lenyomva a Ctrl, ne jelenjen meg az indikátor. 
		m_tableIndicator = false;
	}

	// világ transzformációk 

	m_tableWorldTransform = glm::translate(TABLE_POS)
		* glm::rotate(glm::half_pi<float>(), glm::vec3(-1.0f, 0.0, 0.0))
		* glm::scale(glm::vec3(TABLE_SIZE));

	for (int objIdx = 0; objIdx < m_Spheres.size(); ++objIdx)
	{
		m_spheresWorldTransforms[objIdx] = glm::translate(m_Spheres[objIdx].Position) * glm::scale(glm::vec3(m_Spheres[objIdx].Radius));
	}
}

void CMyApp::SetCommonUniforms()
{
	// - view és projekciós mátrix 
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	// - Fényforrások beállítása 
	glUniform3fv(ul("cameraPosition"), 1, glm::value_ptr(m_camera.GetEye()));
	glUniform4fv(ul("lightPosition"), 1, glm::value_ptr(m_lightPosition));

	glUniform3fv(ul("La"), 1, glm::value_ptr(m_La));
	glUniform3fv(ul("Ld"), 1, glm::value_ptr(m_Ld));
	glUniform3fv(ul("Ls"), 1, glm::value_ptr(m_Ls));

	glUniform1f(ul("lightConstantAttenuation"), m_lightConstantAttenuation);
	glUniform1f(ul("lightLinearAttenuation"), m_lightLinearAttenuation);
	glUniform1f(ul("lightQuadraticAttenuation"), m_lightQuadraticAttenuation);
}

void CMyApp::DrawObject(OGLObject& obj, const glm::mat4& world) {
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(ul("worldInvTransp"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(world))));
	glBindVertexArray(obj.vaoID);
	glDrawElements(GL_TRIANGLES, obj.count, GL_UNSIGNED_INT, nullptr);
}

void CMyApp::RenderTable()
{
	// - Anyagjellemzők beállítása 
	glUniform3fv(ul("Ka"), 1, glm::value_ptr(m_Ka));
	glUniform3fv(ul("Kd"), 1, glm::value_ptr(m_Kd));
	glUniform3fv(ul("Ks"), 1, glm::value_ptr(m_Ks));

	glUniform1f(ul("Shininess"), m_Shininess);

	// - Indikátor beállítása 
	glUniform1f(ul("indicatorFactor"), m_tableIndicator ? 1.0f : 0.0f);
	glUniform3fv(ul("indicatorColor"), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
	glUniform1i(ul("isIndicatorLocal"), 1);
	glUniform2fv(ul("indicatorUV"), 1, glm::value_ptr(m_tableIndicatorUV));
	glUniform1f(ul("indicatorR"), m_tableIndicatorR);

	// - textúraegységek beállítása 
	glUniform1i(ul("textureImage"), 0);

	// - Textúrák beállítása, minden egységre külön 
	glBindTextureUnit(0, m_woodTextureID);
	glBindSampler(0, m_SamplerID);

	DrawObject(m_quadGPU, m_tableWorldTransform);
}

void CMyApp::RenderSpheres()
{
	// - Textúrák beállítása, minden egységre külön 
	glBindTextureUnit(0, m_sphereTextureID);
	glBindSampler(0, m_SamplerID);

	for (int objIdx = 0; objIdx < m_Spheres.size(); ++objIdx)
	{
		const glm::mat4& sphereWorldTransform = m_spheresWorldTransforms[objIdx];

		// - Anyagjellemzők beállítása 
		glUniform3fv(ul("Ka"), 1, glm::value_ptr(m_Ka));
		glUniform3fv(ul("Kd"), 1, glm::value_ptr(m_Kd));
		glUniform3fv(ul("Ks"), 1, glm::value_ptr(m_Ks));

		glUniform1f(ul("Shininess"), m_Shininess);

		float indicatorFactor = (m_SelectedObject == objIdx) ? sinf(glm::two_pi<float>() * m_ElapsedTimeInSec) * 0.5f + 0.5f : 0.0f;

		glUniform1f(ul("indicatorFactor"), indicatorFactor);
		glUniform3fv(ul("indicatorColor"), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 1.0f)));
		glUniform1i(ul("isIndicatorLocal"), 0);

		DrawObject(m_sphereGPU, sphereWorldTransform);
	}
}

void CMyApp::Render()
{
	// töröljük a framepuffert (GL_COLOR_BUFFER_BIT)...
	// ... és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);

	SetCommonUniforms();

	RenderTable();
	RenderSpheres();

	// shader kikapcsolása 
	glUseProgram(0);

	// - Textúrák kikapcsolása, minden egységre külön 
	glBindTextureUnit(0, 0);
	glBindSampler(0, 0);


	// VAO kikapcsolása 
	glBindVertexArray(0);
}

void CMyApp::RenderGUI()
{
	// ImGui::ShowDemoWindow();
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
	m_PickedPixel = { mouse.x, mouse.y };
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
	m_camera.SetAspect(static_cast<float>(_w) / _h);

	// tároljuk el az új ablakméretet a pickinghez 
	m_windowSize = glm::uvec2(_w, _h);
}

// Le nem kezelt, egzotikus esemény kezelése
// https://wiki.libsdl.org/SDL2/SDL_Event

void CMyApp::OtherEvent(const SDL_Event& ev)
{

}
