#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"
#include "ObjParser.h"
#include "ParametricSurfaceMesh.hpp"
#include "ProgramBuilder.h"

#include <imgui.h>

#include <string>
#include <array>
#include <algorithm>

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
	ProgramBuilder{ m_programID }
		.ShaderStage(GL_VERTEX_SHADER, "Shaders/Vert_PosNormTex.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "Shaders/Frag_Lighting.frag")
		.Link();

	InitSkyboxShaders();
}

void CMyApp::InitSkyboxShaders()
{
	m_programSkyboxID = glCreateProgram();
	ProgramBuilder{ m_programSkyboxID }
		.ShaderStage(GL_VERTEX_SHADER, "Shaders/Vert_skybox.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "Shaders/Frag_skybox_skeleton.frag")
		.Link();
}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programID);

	CleanSkyboxShaders();
}

void CMyApp::CleanSkyboxShaders()
{
	glDeleteProgram(m_programSkyboxID);
}

void CMyApp::InitGeometry()
{

	const std::initializer_list<VertexAttributeDescriptor> vertexAttribList =
	{
		{ 0, offsetof(Vertex, position), 3, GL_FLOAT },
		{ 1, offsetof(Vertex, normal), 3, GL_FLOAT },
		{ 2, offsetof(Vertex, texcoord), 2, GL_FLOAT },
	};

	// Suzanne

	MeshObject<Vertex> suzanneMeshCPU = ObjParser::parse("Assets/Suzanne.obj");

	m_SuzanneGPU = CreateGLObjectFromMesh(suzanneMeshCPU, vertexAttribList);

	// Skybox
	InitSkyboxGeometry();
}

void CMyApp::CleanGeometry()
{
	CleanOGLObject(m_SuzanneGPU);
	CleanSkyboxGeometry();
}

void CMyApp::InitSkyboxGeometry()
{
	// skybox geo
	MeshObject<glm::vec3> skyboxCPU =
	{
		std::vector<glm::vec3>
		{
		// hátsó lap
		glm::vec3(-1, -1, -1),
		glm::vec3(1, -1, -1),
		glm::vec3(1,  1, -1),
		glm::vec3(-1,  1, -1),
			// elülső lap
			glm::vec3(-1, -1, 1),
			glm::vec3(1, -1, 1),
			glm::vec3(1,  1, 1),
			glm::vec3(-1,  1, 1),
		},

		std::vector<GLuint>
		{
		// hátsó lap
		0, 1, 2,
		2, 3, 0,
			// elülső lap
			4, 6, 5,
			6, 4, 7,
			// bal
			0, 3, 4,
			4, 3, 7,
			// jobb
			1, 5, 2,
			5, 6, 2,
			// alsó
			1, 0, 4,
			1, 4, 5,
			// felső
			3, 2, 6,
			3, 6, 7,
		}
	};

	m_SkyboxGPU = CreateGLObjectFromMesh(skyboxCPU, { { 0, offsetof(glm::vec3,x), 3, GL_FLOAT } });
}

void CMyApp::CleanSkyboxGeometry()
{
	CleanOGLObject(m_SkyboxGPU);
}

void CMyApp::InitTextures()
{
	// sampler

	glCreateSamplers(1, &m_SamplerID);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// diffuse texture

	ImageRGBA SuzanneImage = ImageFromFile("Assets/wood.jpg");

	glCreateTextures(GL_TEXTURE_2D, 1, &m_SuzanneTextureID);
	glTextureStorage2D(m_SuzanneTextureID, NumberOfMIPLevels(SuzanneImage), GL_RGBA8, SuzanneImage.width, SuzanneImage.height);
	glTextureSubImage2D(m_SuzanneTextureID, 0, 0, 0, SuzanneImage.width, SuzanneImage.height, GL_RGBA, GL_UNSIGNED_BYTE, SuzanneImage.data());

	glGenerateTextureMipmap(m_SuzanneTextureID);

	InitSkyboxTextures();
}

void CMyApp::CleanTextures()
{
	glDeleteTextures(1, &m_SuzanneTextureID);

	CleanSkyboxTextures();

	glDeleteSamplers(1, &m_SamplerID);
}

void CMyApp::InitSkyboxTextures()
{
	//skybox texture
	static const char* skyboxFiles[6] = {
	   "Assets/xpos.png",
	   "Assets/xneg.png",
	   "Assets/ypos.png",
	   "Assets/yneg.png",
	   "Assets/zpos.png",
	   "Assets/zneg.png",
	};

	ImageRGBA images[6];
	for (int i = 0; i < 6; ++i)
	{
		images[i] = ImageFromFile(skyboxFiles[i], false);
	}

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_SkyboxTextureID);
	glTextureStorage2D(m_SkyboxTextureID, 1, GL_RGBA8, images[0].width, images[0].height);

	for (int face = 0; face < 6; ++face)
	{
		glTextureSubImage3D(m_SkyboxTextureID, 0, 0, 0, face, images[face].width, images[face].height, 1, GL_RGBA, GL_UNSIGNED_BYTE, images[face].data());
	}

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void CMyApp::CleanSkyboxTextures()
{
	glDeleteTextures(1, &m_SkyboxTextureID);
}

bool CMyApp::Init()
{
	SetupDebugCallback();

	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glPointSize(16.0f); // nagyobb pontok
	glLineWidth(4.0f); // vastagabb vonalak

	InitShaders();
	InitGeometry();
	InitTextures();

	//
	// egyéb inicializálás
	//

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hátrafelé néző lapok eldobását
	glCullFace(GL_BACK);    // GL_BACK: a kamerától "elfelé" néző lapok, GL_FRONT: a kamera felé néző lapok

	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

	// kamera
	m_camera.SetView(
		glm::vec3(0.0, 7.0, 7.0),	// honnan nézzük a színteret	   - eye
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

	m_suzanneWorldTransform = glm::translate(SUZANNE_POS);
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT)...
	// ... és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//
	// Suzanne
	//

	// - Program
	glUseProgram(m_programID);

	// - Uniform paraméterek
	// view és projekciós mátrix
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(m_suzanneWorldTransform));
	glUniformMatrix4fv(ul("worldInvTransp"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(m_suzanneWorldTransform))));

	// - Fényforrások beállítása
	glUniform3fv(ul("cameraPosition"), 1, glm::value_ptr(m_camera.GetEye()));
	glUniform4fv(ul("lightPosition"), 1, glm::value_ptr(m_lightPosition));

	glUniform3fv(ul("La"), 1, glm::value_ptr(m_La));
	glUniform3fv(ul("Ld"), 1, glm::value_ptr(m_Ld));
	glUniform3fv(ul("Ls"), 1, glm::value_ptr(m_Ls));

	glUniform1f(ul("lightConstantAttenuation"), m_lightConstantAttenuation);
	glUniform1f(ul("lightLinearAttenuation"), m_lightLinearAttenuation);
	glUniform1f(ul("lightQuadraticAttenuation"), m_lightQuadraticAttenuation);

	// - Anyagjellemzők beállítása
	glUniform3fv(ul("Ka"), 1, glm::value_ptr(m_Ka));
	glUniform3fv(ul("Kd"), 1, glm::value_ptr(m_Kd));
	glUniform3fv(ul("Ks"), 1, glm::value_ptr(m_Ks));

	glUniform1f(ul("Shininess"), m_Shininess);

	// - textúraegységek beállítása
	glUniform1i(ul("textureImage"), 0);

	// - Textúrák beállítása, minden egységre külön
	glBindTextureUnit(0, m_SuzanneTextureID);
	glBindSampler(0, m_SamplerID);

	// - VAO
	glBindVertexArray(m_SuzanneGPU.vaoID);

	// Rajzolási parancs kiadása
	glDrawElements(GL_TRIANGLES,
		m_SuzanneGPU.count,
		GL_UNSIGNED_INT,
		nullptr);

	//
	// skybox
	//

	// mentsük el az előző Z-test eredményt, azaz azt a relációt, ami alapján update-eljük a pixelt.
	GLint prevDepthFnc;
	glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFnc);

	// most kisebb-egyenlőt használjunk, mert mindent kitolunk a távoli vágósíkokra
	glDepthFunc(GL_LEQUAL);

	// - Program
	glUseProgram(m_programSkyboxID);

	// - uniform parameterek
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(glm::translate(m_camera.GetEye())));

	// - textúraegységek beállítása
	glUniform1i(ul("skyboxTexture"), 0);
	glBindTextureUnit(0, m_SkyboxTextureID);


	// - VAO
	glBindVertexArray(m_SkyboxGPU.vaoID);

	// - Rajzolas
	glDrawElements(GL_TRIANGLES, m_SkyboxGPU.count, GL_UNSIGNED_INT, nullptr);

	glDepthFunc(prevDepthFnc);

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
	//ImGui::ShowDemoWindow();
	if (ImGui::Begin("Lighting settings"))
	{
		ImGui::InputFloat("Shininess", &m_Shininess, 0.1f, 1.0f, "%.1f");
		static float Kaf = 1.0f;
		static float Kdf = 1.0f;
		static float Ksf = 1.0f;
		if (ImGui::SliderFloat("Ka", &Kaf, 0.0f, 1.0f))
		{
			m_Ka = glm::vec3(Kaf);
		}
		if (ImGui::SliderFloat("Kd", &Kdf, 0.0f, 1.0f))
		{
			m_Kd = glm::vec3(Kdf);
		}
		if (ImGui::SliderFloat("Ks", &Ksf, 0.0f, 1.0f))
		{
			m_Ks = glm::vec3(Ksf);
		}

		{
			static glm::vec2 lightPositionXZ = glm::vec2(0.0f);
			lightPositionXZ = glm::vec2(m_lightPosition.x, m_lightPosition.z);
			if (ImGui::SliderFloat2("Light Position XZ", glm::value_ptr(lightPositionXZ), -1.0f, 1.0f))
			{
				float lightPositionL2 = lightPositionXZ.x * lightPositionXZ.x + lightPositionXZ.y * lightPositionXZ.y;
				if (lightPositionL2 > 1.0f) // Ha kívülre esne a körön, akkor normalizáljuk
				{
					lightPositionXZ /= sqrtf(lightPositionL2);
					lightPositionL2 = 1.0f;
				}

				m_lightPosition.x = lightPositionXZ.x;
				m_lightPosition.z = lightPositionXZ.y;
				m_lightPosition.y = sqrtf(1.0f - lightPositionL2);
			}
			ImGui::LabelText("Light Position Y", "%f", m_lightPosition.y);
		}
	}
	ImGui::End();
}

// https://wiki.libsdl.org/SDL3/SDL_KeyboardEvent
// https://wiki.libsdl.org/SDL3/SDL_Keysym
// https://wiki.libsdl.org/SDL3/SDL_Keycode
// https://wiki.libsdl.org/SDL3/SDL_Keymod

void CMyApp::KeyboardDown(const SDL_KeyboardEvent& key)
{
	if (!key.repeat) // Először lett megnyomva
	{
		if (key.key == SDLK_F5 && key.mod & SDL_KMOD_CTRL)
		{
			CleanShaders();
			InitShaders();
		}
		if (key.key == SDLK_F1)
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

// https://wiki.libsdl.org/SDL3/SDL_MouseMotionEvent

void CMyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	m_cameraManipulator.MouseMove(mouse);
}

// https://wiki.libsdl.org/SDL3/SDL_MouseButtonEvent

void CMyApp::MouseDown(const SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(const SDL_MouseButtonEvent& mouse)
{
}

// https://wiki.libsdl.org/SDL3/SDL_MouseWheelEvent

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
// https://wiki.libsdl.org/SDL3/SDL_Event

void CMyApp::OtherEvent(const SDL_Event& ev)
{

}
