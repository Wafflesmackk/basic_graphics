#include "MyApp.h"
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
	AttachShader(m_programID, GL_VERTEX_SHADER, "Shaders/Vert_PosTex.vert");
	AttachShader(m_programID, GL_FRAGMENT_SHADER, "Shaders/Frag_PosTex.frag");
	LinkProgram(m_programID);
}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programID);
}

void CMyApp::InitGeometry()
{
	// Pyramid 

	MeshObject<VertexPosTex> pyramidMeshCPU;

	static constexpr float SQRT_2 = glm::root_two<float>();

	pyramidMeshCPU.vertexArray =
	{
		{ glm::vec3(-1.0f,    0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(1.0f,    0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(-1.0f,    0.0f,  1.0f), glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(1.0f,    0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
		{ glm::vec3(0.0f,  SQRT_2,  0.0f), glm::vec2(0.5, 1.0f) },
		{ glm::vec3(0.0f,  SQRT_2,  0.0f), glm::vec2(0.5, 1.0f) },
		{ glm::vec3(0.0f,  SQRT_2,  0.0f), glm::vec2(0.5, 1.0f) },
		{ glm::vec3(0.0f,  SQRT_2,  0.0f), glm::vec2(0.5, 1.0f) },
		{ glm::vec3(1.0f,    0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(-1.0f,    0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(-1.0f,    0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(-1.0f,    0.0f,  1.0f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(-1.0f,    0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(1.0f,    0.0f,  1.0f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(1.0f,    0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(1.0f,    0.0f, -1.0f), glm::vec2(1.0f, 0.0f) }
	};

	pyramidMeshCPU.indexArray =
	{
		// 1. háromszög
		0,1,2,
		// 2. háromszög
		2,1,3,
		// 3. háromszög
		4,8,9,
		// 4. háromszög
		5,10,11,
		// 5. háromszög
		6,12,13,
		// 6.háromszög
		7,14,15
	};

	const std::initializer_list<VertexAttributeDescriptor> vertexAttribList =
	{
		{ 0, offsetof(VertexPosTex, position), 3, GL_FLOAT },
		{ 1, offsetof(VertexPosTex, texcoord), 2, GL_FLOAT },
	};

	m_pyramidGPU = CreateGLObjectFromMesh(pyramidMeshCPU, vertexAttribList);

	//quad
	MeshObject<VertexPosTex> quadMeshCPU;

	quadMeshCPU.vertexArray = {
		{ glm::vec3(-1.0f,    -1.0f, -0.0f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(1.0f,    -1.0f, 0.0f), glm::vec2(5.0f, 0.0f) },
		{ glm::vec3(-1.0f,    1.0f,  0.0f), glm::vec2(0.0f, 5.0f) },
		{ glm::vec3(1.0f,    1.0f,  0.0f), glm::vec2(5.0f, 5.0f) },
	};
	quadMeshCPU.indexArray = {
		0,1,2,
		1,3,2
	};

	m_quadGPU = CreateGLObjectFromMesh(quadMeshCPU, vertexAttribList);
}

void CMyApp::CleanGeometry()
{
	CleanOGLObject(m_pyramidGPU);
	CleanOGLObject(m_quadGPU);
}

void CMyApp::InitTextures()
{
	glCreateSamplers(1, &m_SamplerID);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// pyramid

	// diffuse texture
	ImageRGBA Image = ImageFromFile("Assets/yellowbrick.png");

	glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
	glTextureStorage2D(m_TextureID, NumberOfMIPLevels(Image), GL_RGBA8, Image.width, Image.height);
	glTextureSubImage2D(m_TextureID, 0, 0, 0, Image.width, Image.height, GL_RGBA, GL_UNSIGNED_BYTE, Image.data());

	glGenerateTextureMipmap(m_TextureID);

	//quad
	ImageRGBA fenceImage = ImageFromFile("Assets/fence.png");
	glCreateTextures(GL_TEXTURE_2D, 1, &m_FenceTextureID);
	glTextureStorage2D(m_FenceTextureID, NumberOfMIPLevels(fenceImage), GL_RGBA8, fenceImage.width, fenceImage.height);
	glTextureSubImage2D(m_FenceTextureID, 0, 0, 0, fenceImage.width, fenceImage.height, GL_RGBA, GL_UNSIGNED_BYTE, fenceImage.data());

	glGenerateTextureMipmap(m_FenceTextureID);

	ImageRGBA fenceMaskImage = ImageFromFile("Assets/fence.png");
	glCreateTextures(GL_TEXTURE_2D, 1, &m_FenceMaskTextureID);
	glTextureStorage2D(m_FenceMaskTextureID, NumberOfMIPLevels(fenceMaskImage), GL_RGBA8, fenceMaskImage.width, fenceMaskImage.height);
	glTextureSubImage2D(m_FenceMaskTextureID, 0, 0, 0, fenceMaskImage.width, fenceMaskImage.height, GL_RGBA, GL_UNSIGNED_BYTE, fenceMaskImage.data());

	glGenerateTextureMipmap(m_FenceMaskTextureID);

	ImageRGBA DummyWhiteImage;
	DummyWhiteImage.Allocate(1, 1);
	DummyWhiteImage.SetTexel(0, 0, glm::u8vec4(0xFF));

	glCreateTextures(GL_TEXTURE_2D, 1, &m_DummyWhiteTextureID);
	glTextureStorage2D(m_DummyWhiteTextureID, NumberOfMIPLevels(DummyWhiteImage), GL_RGBA8, DummyWhiteImage.width, DummyWhiteImage.height);
	glTextureSubImage2D(m_DummyWhiteTextureID, 0, 0, 0, DummyWhiteImage.width, DummyWhiteImage.height, GL_RGBA, GL_UNSIGNED_BYTE, DummyWhiteImage.data());

	glGenerateTextureMipmap(m_DummyWhiteTextureID);
}

void CMyApp::CleanTextures()
{
	glDeleteTextures(1, &m_TextureID);
	glDeleteTextures(1, &m_FenceTextureID);
	glDeleteTextures(1, &m_FenceMaskTextureID);
	glDeleteTextures(1, &m_DummyWhiteTextureID);
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
		glm::vec3(0.0, 0.0, 5.0),// honnan nézzük a színteret	   - eye
		glm::vec3(0.0, 0.0, 0.0),   // a színtér melyik pontját nézzük - at
		glm::vec3(0.0, 1.0, 0.0));  // felfelé mutató irány a világban - up

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

	m_pyramidWorldTransform = glm::identity<glm::mat4>();

	m_quadWorldTransform = glm::translate(glm::vec3(0.0f, 1.0f, 2.0f));

	indicatorElevation = m_ElapsedTimeInSec * 0.25;
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT)...
	// ... és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// pyramid

	// - shader bekapcsolasa
	glUseProgram(m_programID);

	// - Uniform paraméterek
	// view és projekciós mátrix
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	glUniform1f(ul("indicatorElevation"), indicatorElevation);

	// Transzformációs mátrixok
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(m_pyramidWorldTransform));

	glUniform1i(ul("textureImage"), 0);
	glUniform1i(ul("textureMask"), 1);

	// - VAO beállítása
	glBindVertexArray(m_pyramidGPU.vaoID);

	// - Textúrák beállítása, minden egységre külön
	glBindTextureUnit(0, m_TextureID);
	glBindSampler(0, m_SamplerID);
	glBindTextureUnit(1, m_DummyWhiteTextureID);
	glBindSampler(1, m_SamplerID);

	// Rajzolási parancs kiadása
	glDrawElements(GL_TRIANGLES,
		m_pyramidGPU.count,
		GL_UNSIGNED_INT,
		nullptr);

	//quad
	glUniform1i(ul("textureImage"), 0);
	glUniform1i(ul("textureMask"), 1);

	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(m_quadWorldTransform));

	glBindVertexArray(m_quadGPU.vaoID);

	glBindTextureUnit(0, m_FenceTextureID);
	glBindSampler(0, m_SamplerID);

	glBindTextureUnit(1, m_FenceMaskTextureID);
	glBindSampler(1, m_SamplerID);

	glDrawElements(GL_TRIANGLES,
		m_quadGPU.count,
		GL_UNSIGNED_INT,
		nullptr);


	// shader kikapcsolasa
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
