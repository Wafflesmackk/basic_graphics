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
	AttachShader(m_programID, GL_FRAGMENT_SHADER, "Shaders/Frag_LightingSkeleton.frag");
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

	// Suzanne

	MeshObject<Vertex> suzanneMeshCPU = ObjParser::parse("Assets/Suzanne.obj");

	m_SuzanneGPU = CreateGLObjectFromMesh(suzanneMeshCPU, vertexAttribList);

	// Gömb 

	MeshObject<Vertex> sphereMeshCPU = ObjParser::parse("Assets/MarbleBall.obj");

	m_sphereGPU = CreateGLObjectFromMesh(sphereMeshCPU, vertexAttribList);
}

void CMyApp::CleanGeometry()
{
	CleanOGLObject(m_quadGPU);
	CleanOGLObject(m_SuzanneGPU);
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

	ImageRGBA SuzanneImage = ImageFromFile("Assets/wood.jpg");

	glCreateTextures(GL_TEXTURE_2D, 1, &m_SuzanneTextureID);
	glTextureStorage2D(m_SuzanneTextureID, NumberOfMIPLevels(SuzanneImage), GL_RGBA8, SuzanneImage.width, SuzanneImage.height);
	glTextureSubImage2D(m_SuzanneTextureID, 0, 0, 0, SuzanneImage.width, SuzanneImage.height, GL_RGBA, GL_UNSIGNED_BYTE, SuzanneImage.data());

	glGenerateTextureMipmap(m_SuzanneTextureID);

	ImageRGBA sphereImage = ImageFromFile("Assets/MarbleBall.png");

	glCreateTextures(GL_TEXTURE_2D, 1, &m_sphereTextureID);
	glTextureStorage2D(m_sphereTextureID, NumberOfMIPLevels(sphereImage), GL_RGBA8, sphereImage.width, sphereImage.height);
	glTextureSubImage2D(m_sphereTextureID, 0, 0, 0, sphereImage.width, sphereImage.height, GL_RGBA, GL_UNSIGNED_BYTE, sphereImage.data());

	glGenerateTextureMipmap(m_sphereTextureID);
}

void CMyApp::CleanTextures()
{
	glDeleteTextures(1, &m_woodTextureID);
	glDeleteTextures(1, &m_SuzanneTextureID);
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
		glm::vec3(0.0, 7.0, 7.0),   // honnan nézzük a színteret  - eye
		glm::vec3(0.0, 0.0, 0.0),   // a színtér melyik pontját nézzük  - at
		glm::vec3(0.0, 1.0, 0.0));  // felfelé mutató irány a világban  - up

	m_cameraManipulator.SetCamera(&m_camera);

	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanGeometry();
	CleanTextures();
}

void CMyApp::Update(const SUpdateInfo& updateInfo)
{
	m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;

	m_cameraManipulator.Update(updateInfo.DeltaTimeInSec);

	// világ transzformációk 

	m_tableWorldTransform = glm::translate(TABLE_POS)
		* glm::rotate(glm::half_pi<float>(), glm::vec3(-1.0f, 0.0, 0.0))
		* glm::scale(glm::vec3(TABLE_SIZE));

	m_sphereWorldTransform = glm::translate(SPHERE_POS);

	m_SuzanneWorldTransform = glm::translate(SUZANNE_POS);

	if (m_IsLampOn) {
		glClearColor(0.125f, 0.25f, 0.5f, 1.0f);
		m_La = glm::vec3(0.125);
		m_Ld = m_Ls = glm::vec3(1.0f);

		m_lightPosition = glm::vec4(m_lampPos, 0.0);
	}
	else {
		glClearColor(0, 0, 0, 0);
		m_La = glm::vec3(0.0125);
		m_Ld = m_Ls = BUG_COLOR;
		m_lightPosition = glm::vec4(m_bugPos, 1.0); //itt már konkrét position
	}
}

void CMyApp::SetCommonUniforms()
{
	// view és projekciós mátrix 
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	glUniform3fv(ul("cameraPosition"), 1, glm::value_ptr(m_camera.GetEye()));
	glUniform4fv(ul("lightPosition"), 1, glm::value_ptr(m_lightPosition));

	glUniform3fv(ul("La"), 1, glm::value_ptr(m_La));
	glUniform3fv(ul("Ld"), 1, glm::value_ptr(m_Ld));
	glUniform3fv(ul("Ls"), 1, glm::value_ptr(m_Ls));
}

void CMyApp::DrawObject(OGLObject& obj, const glm::mat4& world) {
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(ul("worldInvTransp"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(world))));
	glBindVertexArray(obj.vaoID);
	glDrawElements(GL_TRIANGLES, obj.count, GL_UNSIGNED_INT, nullptr);
}

void CMyApp::RenderTable()
{
	// - textúraegységek beállítása 
	glUniform1i(ul("textureImage"), 0);

	// - Textúrák beállítása, minden egységre külön 
	glBindTextureUnit(0, m_woodTextureID);
	glBindSampler(0, m_SamplerID);

	glUniform3fv(ul("Ka"), 1, glm::value_ptr(m_Ka)); //anyag tulajdonság
	glUniform3fv(ul("Kd"), 1, glm::value_ptr(m_Kd));
	glUniform3fv(ul("Ks"), 1, glm::value_ptr(m_Ks));

	DrawObject(m_quadGPU, m_tableWorldTransform);
}

void CMyApp::RenderBall()
{
	// - textúraegységek beállítása 
	glUniform1i(ul("textureImage"), 0);

	// - Textúrák beállítása, minden egységre külön 
	glBindTextureUnit(0, m_sphereTextureID);
	glBindSampler(0, m_SamplerID);

	glUniform3fv(ul("Ka"), 1, glm::value_ptr(m_Ka));
	glUniform3fv(ul("Kd"), 1, glm::value_ptr(m_Kd));
	glUniform3fv(ul("Ks"), 1, glm::value_ptr(m_Ks));

	DrawObject(m_sphereGPU, m_sphereWorldTransform);
}

void CMyApp::RenderSuzanne()
{
	// - textúraegységek beállítása 
	glUniform1i(ul("textureImage"), 0);

	// - Textúrák beállítása, minden egységre külön 
	glBindTextureUnit(0, m_SuzanneTextureID);
	glBindSampler(0, m_SamplerID);

	glUniform3fv(ul("Ka"), 1, glm::value_ptr(m_Ka));
	glUniform3fv(ul("Kd"), 1, glm::value_ptr(m_Kd));
	glUniform3fv(ul("Ks"), 1, glm::value_ptr(m_Ks));

	DrawObject(m_SuzanneGPU, m_SuzanneWorldTransform);
}

void CMyApp::Render()
{
	// töröljük a framepuffert (GL_COLOR_BUFFER_BIT)...
	// ... és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);

	SetCommonUniforms();

	RenderTable();
	RenderBall();
	RenderSuzanne();

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
	ImGui::Checkbox("Is lamp on", &m_IsLampOn);
	ImGui::SliderFloat3("bug", glm::value_ptr(m_bugPos), -5.0, 5.0);
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
	}
	m_cameraManipulator.KeyboardDown(key);
}

void CMyApp::KeyboardUp(const SDL_KeyboardEvent& key)
{
	m_cameraManipulator.KeyboardUp(key);
}

// https://wiki.libsdl.org/SDL2/SDL_MouseMotionEvent

void CMyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	m_cameraManipulator.MouseMove(mouse);
}

// https://wiki.libsdl.org/SDL2/SDL_MouseButtonEvent

void CMyApp::MouseDown(const SDL_MouseButtonEvent& mouse)
{
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
}

// Le nem kezelt, egzotikus esemény kezelése
// https://wiki.libsdl.org/SDL2/SDL_Event

void CMyApp::OtherEvent(const SDL_Event& ev)
{

}
