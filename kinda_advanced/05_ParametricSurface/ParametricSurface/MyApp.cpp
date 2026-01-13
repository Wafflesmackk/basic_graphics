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

}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programID);
}

// Nyers parameterek
struct ParamSphere
{
	float r = 1.0f;
	glm::vec3 GetPos(float u, float v) const noexcept
	{
		/*glm::vec3 b[3][3] = {{glm::vec3(0,0,0),glm::vec3(1,0,0),glm::vec3(2,0,0)},
								{glm::vec3(0,0,1),glm::vec3(1,2,1),glm::vec3(2,0,1)},
								{glm::vec3(0,0,2),glm::vec3(1,0,2),glm::vec3(2,0,2)} };*/
		float phi = u * 2.0f * glm::pi<float>();
		float theta = v * glm::pi<float>();
		return glm::vec3(
			r * sin(theta) * cos(phi),
			r * cos(theta),
			r * sin(theta) * sin(phi)
		);
	}
	glm::vec3 GetNorm(float u, float v) const noexcept
	{
		float epsilon = 0.00001f;
		/*glm::vec3 a = glm::vec3((GetPos(u, v) + epsilon) - (GetPos(u, v) - epsilon) / 2.0f * epsilon);
		return glm::normalize(a);*/

		glm::vec3 du = GetPos(u + epsilon, v) - GetPos(u - epsilon, v);
		glm::vec3 dv = GetPos(u, v + epsilon) - GetPos(u, v - epsilon);
		return glm::normalize(cross(du, dv));

	}
	glm::vec2 GetTex(float u, float v) const noexcept
	{
		return glm::vec2(u, v);
	}
};

struct ParamTorus
{
	float r = 1.0f;
	float R = 2.f;
	glm::vec3 GetPos(float u, float v) const noexcept
	{
		/*glm::vec3 b[3][3] ={{glm::vec3(0,0,0),glm::vec3(1,0,0),glm::vec3(2,0,0)},
								{glm::vec3(0,0,1),glm::vec3(1,2,1),glm::vec3(2,0,1)},
								{glm::vec3(0,0,2),glm::vec3(1,0,2),glm::vec3(2,0,2)} };*/
		float phi = u * 2.0f * glm::pi<float>();
		float theta = v * 2.0f * glm::pi<float>();
		return glm::vec3(
			(R + r * cos(phi)) * sin(theta),
			(R + r * cos(phi)) * cos(theta),
			r * sin(phi)
		);
	}
	glm::vec3 GetNorm(float u, float v) const noexcept
	{
		float epsilon = 0.00001f;
		/*glm::vec3 a = glm::vec3((GetPos(u, v) + epsilon) - (GetPos(u, v) - epsilon) / 2.0f * epsilon);
		return glm::normalize(a);*/

		glm::vec3 du = GetPos(u + epsilon, v) - GetPos(u - epsilon, v);
		glm::vec3 dv = GetPos(u, v + epsilon) - GetPos(u, v - epsilon);
		return glm::normalize(cross(du, dv));

	}
	glm::vec2 GetTex(float u, float v) const noexcept
	{
		return glm::vec2(u, v);
	}
};

struct ParamBezier
{
	static const int n = 2;
	static const int m = 2;
	/*glm::vec3 b[3][3] */
	std::array<std::array<glm::vec3, n + 1>, m + 1> b
		= { std::array{glm::vec3(0,0,0),glm::vec3(1,0,0),glm::vec3(2,0,0)},
		std::array{glm::vec3(0,0,1),glm::vec3(1,2,1),glm::vec3(2,0,1)},
		std::array{glm::vec3(0,0,2),glm::vec3(1,0,2),glm::vec3(2,0,2)} };

	glm::vec3 GetPos(float u, float v) const noexcept
	{
		float Bu[3] = { (1 - u) * (1 - u), 2 * u * (1 - u), u * u };
		float Bv[3] = { (1 - v) * (1 - v), 2 * v * (1 - v),v * v };

		glm::vec3 pos(0.0f);
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				pos += Bu[i] * Bv[j] * b[i][j];
			}
		}
		return pos;
	}
	glm::vec3 GetNorm(float u, float v) const noexcept
	{
		float epsilon = 0.00001f;
		/*glm::vec3 a = glm::vec3((GetPos(u, v) + epsilon) - (GetPos(u, v) - epsilon) / 2.0f * epsilon);
		return glm::normalize(a);*/

		glm::vec3 du = GetPos(u + epsilon, v) - GetPos(u - epsilon, v);
		glm::vec3 dv = GetPos(u, v + epsilon) - GetPos(u, v - epsilon);
		return glm::normalize(cross(du, dv));

	}
	glm::vec2 GetTex(float u, float v) const noexcept
	{
		return glm::vec2(u, v);
	}
};

void CMyApp::InitGeometry()
{

	const std::initializer_list<VertexAttributeDescriptor> vertexAttribList =
	{
		{ 0, offsetof(Vertex, position), 3, GL_FLOAT },
		{ 1, offsetof(Vertex, normal), 3, GL_FLOAT },
		{ 2, offsetof(Vertex, texcoord), 2, GL_FLOAT },
	};

	MeshObject<Vertex> SurfaceMeshCPU = GetParamSurfMesh(ParamSphere());
	m_SurfaceGPU = CreateGLObjectFromMesh(SurfaceMeshCPU, vertexAttribList);

	MeshObject<Vertex> SurfaceMeshTorusCPU = GetParamSurfMesh(ParamTorus());
	m_SurfaceTGPU = CreateGLObjectFromMesh(SurfaceMeshTorusCPU, vertexAttribList);
	MeshObject<Vertex> SurfaceMeshBezierCPU = GetParamSurfMesh(ParamBezier());
	m_SurfaceBGPU = CreateGLObjectFromMesh(SurfaceMeshBezierCPU, vertexAttribList);

}

void CMyApp::CleanGeometry()
{
	CleanOGLObject(m_SurfaceGPU);
	CleanOGLObject(m_SurfaceTGPU);
	CleanOGLObject(m_SurfaceBGPU);
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

	ImageRGBA Image = ImageFromFile("Assets/color_checkerboard.png");

	glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
	glTextureStorage2D(m_TextureID, NumberOfMIPLevels(Image), GL_RGBA8, Image.width, Image.height);
	glTextureSubImage2D(m_TextureID, 0, 0, 0, Image.width, Image.height, GL_RGBA, GL_UNSIGNED_BYTE, Image.data());

	glGenerateTextureMipmap(m_TextureID);
}

void CMyApp::CleanTextures()
{
	glDeleteTextures(1, &m_TextureID);

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

	// kivetelesen a fényforrás a kamera pozíciója legyen, hogy mindig lássuk a feluletet,
	// es ne keljen allitgatni a fenyforrast
	m_lightPosition = glm::vec4(m_camera.GetEye(), 1.0);

	m_objectWorldTransform = glm::translate(OBJECT_POS);
}


void CMyApp::SetCommonUniforms()
{
	// view és projekciós mátrix
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

void CMyApp::RenderSurface()
{
	// - Anyagjellemzők beállítása
	glUniform3fv(ul("Ka"), 1, glm::value_ptr(m_Ka));
	glUniform3fv(ul("Kd"), 1, glm::value_ptr(m_Kd));
	glUniform3fv(ul("Ks"), 1, glm::value_ptr(m_Ks));

	glUniform1f(ul("Shininess"), m_Shininess);

	// - textúraegységek beállítása
	glUniform1i(ul("textureImage"), 0);

	// - Textúrák beállítása, minden egységre külön
	glBindTextureUnit(0, m_TextureID);
	glBindSampler(0, m_SamplerID);
	DrawObject(m_SurfaceGPU, m_objectWorldTransform);
	DrawObject(m_SurfaceTGPU, m_objectWorldTransform);
	DrawObject(m_SurfaceBGPU, m_objectWorldTransform);
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT)...
	// ... és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader program ON
	glUseProgram(m_programID);

	SetCommonUniforms();

	bool culling = glIsEnabled(GL_CULL_FACE);
	if (culling) glDisable(GL_CULL_FACE);
	RenderSurface();
	if (culling) glEnable(GL_CULL_FACE);

	// shader program OFF
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
	if (key.repeat == 0) // Először lett megnyomva
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
