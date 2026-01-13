#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"
#include "ObjParser.h"
#include "ProgramBuilder.h"

#include <imgui.h>
#include <iostream>
#include <string>

CMyApp::CMyApp()
{
}

CMyApp::~CMyApp()
{
}

void CMyApp::SetupDebugCallback()
{
	// Enable and set the debug callback function if we are in debug context
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
		.ShaderStage(GL_VERTEX_SHADER, "Shaders/Vert_PosNormTexShadow.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "Shaders/Frag_LightingShadow.frag")
		.Link();

	// We don't need to output color, we only need the depth values
	// from vertex shader stage
	m_programPostprocessID = glCreateProgram();
	ProgramBuilder{ m_programPostprocessID }
		.ShaderStage(GL_VERTEX_SHADER, "Shaders/Vert_Shadow.vert")
		.Link();

	InitAxesShader();
}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programID);
	glDeleteProgram(m_programPostprocessID);
	CleanAxesShader();
}

void CMyApp::InitAxesShader()
{
	m_programAxesID = glCreateProgram();
	ProgramBuilder{ m_programAxesID }
		.ShaderStage(GL_VERTEX_SHADER, "Shaders/Vert_axes.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "Shaders/Frag_PosCol.frag")
		.Link();
}

void CMyApp::CleanAxesShader()
{
	glDeleteProgram(m_programAxesID);
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
	m_Suzanne = CreateGLObjectFromMesh(suzanneMeshCPU, vertexAttribList);
	// Plane
	MeshObject<Vertex> planeMeshCPU{
		{
			{ glm::vec3(-20, -1.0, -20),	glm::vec3(0, 1, 0),	glm::vec2(0, 0)	},
			{ glm::vec3(-20, -1.0,  20),	glm::vec3(0, 1, 0),	glm::vec2(0, 1)	},
			{ glm::vec3(20, -1.0, -20),	glm::vec3(0, 1, 0),	glm::vec2(1, 0)	},
			{ glm::vec3(20, -1.0,  20),	glm::vec3(0, 1, 0),	glm::vec2(1, 1)	},
		},
		{
			{0, 1, 2, 2, 1, 3}
		}
	};
	m_plane = CreateGLObjectFromMesh(planeMeshCPU, vertexAttribList);
}

void CMyApp::CleanGeometry()
{
	CleanOGLObject(m_Suzanne);
	CleanOGLObject(m_plane);
}

void CMyApp::InitTextures()
{
	glCreateSamplers(1, &m_SamplerID);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ImageRGBA metalImage = ImageFromFile("Assets/metal.png");

	glCreateTextures(GL_TEXTURE_2D, 1, &m_metalTextureID);
	glTextureStorage2D(m_metalTextureID, NumberOfMIPLevels(metalImage), GL_RGBA8, metalImage.width, metalImage.height);
	glTextureSubImage2D(m_metalTextureID, 0, 0, 0, metalImage.width, metalImage.height, GL_RGBA, GL_UNSIGNED_BYTE, metalImage.data());

	glGenerateTextureMipmap(m_metalTextureID);
}

void CMyApp::CleanTextures()
{
	glDeleteTextures(1, &m_metalTextureID);
	glDeleteSamplers(1, &m_SamplerID);
}

void CMyApp::InitFrameBufferObject()
{
	// FBO létrehozása
	glCreateFramebuffers(1, &m_frameBufferID);
	glCreateFramebuffers(1, &m_frameBufferID2);
}

void CMyApp::CleanFrameBufferObject()
{
	glDeleteFramebuffers(1, &m_frameBufferID);
	glDeleteFramebuffers(1, &m_frameBufferID2);
}

void CMyApp::InitResolutionDependentResources(int width, int height)
{
	// We use texture instead of renderbuffer, because we will sample it in the shader	

	glCreateTextures(GL_TEXTURE_2D, 1, &m_shadowTextureID);
	glTextureStorage2D(m_shadowTextureID, 1, GL_DEPTH_COMPONENT24, width, height);

	glNamedFramebufferTexture(m_frameBufferID, GL_DEPTH_ATTACHMENT, m_shadowTextureID, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_shadowTextureID2);
	glTextureStorage2D(m_shadowTextureID2, 1, GL_DEPTH_COMPONENT24, width, height);

	glNamedFramebufferTexture(m_frameBufferID2, GL_DEPTH_ATTACHMENT, m_shadowTextureID2, 0);

	// Completeness check
	/*GLenum status = glCheckNamedFramebufferStatus(m_frameBufferID, GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (status) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[InitFramebuffer] Incomplete framebuffer GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT!");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[InitFramebuffer] Incomplete framebuffer GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT!");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[InitFramebuffer] Incomplete framebuffer GL_FRAMEBUFFER_UNSUPPORTED!");
			break;
		}
	}*/
}

void CMyApp::CleanResolutionDependentResources()
{
	glDeleteTextures(1, &m_shadowTextureID);
	glDeleteTextures(1, &m_shadowTextureID2);
}

bool CMyApp::Init()
{
	SetupDebugCallback();

	// Set a bluish clear color
	// glClear() will use this for clearing the color buffer.
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	InitShaders();
	InitGeometry();
	InitTextures();
	InitFrameBufferObject();
	InitResolutionDependentResources(m_bufferResolution, m_bufferResolution);

	//
	// Other
	//

	glEnable(GL_CULL_FACE);	 // Enable discarding the back-facing faces.
	glCullFace(GL_BACK);     // GL_BACK: facets facing away from camera, GL_FRONT: facets facing towards the camera
	glEnable(GL_DEPTH_TEST); // Enable depth testing. (for overlapping geometry)

	// Camera
	m_camera.SetView(
		glm::vec3(0, 20, 20),	// From where we look at the scene - eye
		glm::vec3(0, 4, 0),		// Which point of the scene we are looking at - at
		glm::vec3(0, 1, 0)		// Upwards direction - up
	);
	m_cameraManipulator.SetCamera(&m_camera);

	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanGeometry();
	CleanTextures();
	CleanResolutionDependentResources();
	CleanFrameBufferObject();
}

void CMyApp::Update(const SUpdateInfo& updateInfo)
{
	m_cameraManipulator.Update(updateInfo.DeltaTimeInSec);

	m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;

	// World transform of the suzannes
	glm::mat4* suzanneWorldTransform = m_suzanneWorldTransform;
	for (int i = -1; i <= 1; ++i)
		for (int j = -1; j <= 1; ++j, suzanneWorldTransform++)
		{
			*suzanneWorldTransform = glm::translate(glm::vec3((4 * i), (4 * (j + 1)), sinf(m_ElapsedTimeInSec * 2.f * glm::pi<float>() * (i * j))));
		}

	// Shadow matrix
	glm::mat4 light_proj = glm::ortho<float>(-10, 10, -10, 10, -10, 10);
	glm::mat4 light_view = glm::lookAt<float>(glm::vec3(0, 0, 0), glm::vec3(-m_lightPosition), glm::vec3(0, 1, 0));
	m_light_mvp = light_proj * light_view; // This matrix will tell us how to read the distances in the shadow map

	glm::mat4 light_proj2 = glm::ortho<float>(-10, 10, -10, 10, -10, 10);
	glm::mat4 light_view2 = glm::lookAt<float>(glm::vec3(0, 0, 0), glm::vec3(-m_lightPosition2), glm::vec3(0, 1, 0));
	m_light_mvp2 = light_proj2 * light_view2; // This matrix will tell us how to read the distances in the shadow map
}

void CMyApp::DrawAxes()
{
	glUseProgram(m_programAxesID);
	glm::mat4 axisWorld = glm::translate(m_camera.GetAt());
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(axisWorld));

	// We always want to see it, regardless of whether there is an object in front of it
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_LINES, 0, 6);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

void CMyApp::RenderGeometry(const glm::mat4& viewProj, GLuint program, bool shadowProgram)
{
	glUseProgram(program);

	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(viewProj));
	if (!shadowProgram) {
		glUniformMatrix4fv(ul("worldInvTransp"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

		glBindTextureUnit(0, m_metalTextureID);
		glBindSampler(0, m_SamplerID);
		glUniform1i(ul("textureImage"), 0);

		// Set shadow texture, shadow VP and light pos:
		glBindTextureUnit(1, m_shadowTextureID);
		glBindSampler(1, m_SamplerID);
		glUniform1i(ul("textureShadow"), 1);

		glBindTextureUnit(2, m_shadowTextureID2);
		glBindSampler(2, m_SamplerID);
		glUniform1i(ul("textureShadow2"), 2);

		glUniformMatrix4fv(ul("shadowVP"), 1, GL_FALSE, glm::value_ptr(m_light_mvp));
		glUniformMatrix4fv(ul("shadowVP2"), 1, GL_FALSE, glm::value_ptr(m_light_mvp2));

		glUniform4fv(ul("lightPosition"), 1, glm::value_ptr(m_lightPosition));
		glUniform4fv(ul("lightPosition2"), 1, glm::value_ptr(m_lightPosition2));

		glUniform3fv(ul("cameraPosition"), 1, glm::value_ptr(m_camera.GetEye()));
	}

	// Drawing the plane underneath

	glBindVertexArray(m_plane.vaoID);
	glDrawElements(GL_TRIANGLES, m_plane.count, GL_UNSIGNED_INT, 0);	// With index buffer


	// Suzanne wall
	glBindVertexArray(m_Suzanne.vaoID);
	for (int i = 0; i < countof(m_suzanneWorldTransform); ++i)
	{
		const glm::mat4& suzanneWorld = m_suzanneWorldTransform[i];
		glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(suzanneWorld));
		if (!shadowProgram) {
			glUniformMatrix4fv(ul("worldInvTransp"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(suzanneWorld))));
		}
		glDrawElements(GL_TRIANGLES, m_Suzanne.count, GL_UNSIGNED_INT, 0);
	}

	// We can unbind them
	//glBindTextureUnit( 0, 0);
	glBindSampler(0, 0);
	//glBindTextureUnit(1, 0);
	glBindSampler(1, 0);
	glBindSampler(2, 0);
	glUseProgram(0);
	glBindVertexArray(0);
}

void CMyApp::Render()
{
	// 1.
	// Draw scene to shadow map
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);			// FBO that has the shadow map attached to it
	glViewport(0, 0, m_bufferResolution, m_bufferResolution);	// Set which pixels to render to within this fbo.
	glClear(GL_DEPTH_BUFFER_BIT);								// Clear depth values

	RenderGeometry(m_light_mvp, m_programPostprocessID, true);
	//2 feny
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID2);			// FBO that has the shadow map attached to it
	glViewport(0, 0, m_bufferResolution, m_bufferResolution);	// Set which pixels to render to within this fbo.
	glClear(GL_DEPTH_BUFFER_BIT);								// Clear depth values

	RenderGeometry(m_light_mvp2, m_programPostprocessID, true);

	// 2.
	// Draw mesh to screen

	glBindFramebuffer(GL_FRAMEBUFFER, 0);				// default framebuffer (the backbuffer)
	glViewport(0, 0, m_width, m_height);				// We need to set the render area back
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clearing the default fbo
	RenderGeometry(m_camera.GetViewProj(), m_programID, false);

	DrawAxes();
}

void CMyApp::RenderGUI()
{
	// ImGui::ShowDemoWindow();

	ImGui::Begin("Shadow window");
	{
		static int bufferResolutionLevel = 10;
		std::string bufferResolutionText = std::to_string(1 << bufferResolutionLevel);
		if (ImGui::SliderInt("Shadow resolution", &bufferResolutionLevel, 5, 12, bufferResolutionText.c_str()))
		{
			m_bufferResolution = 1 << bufferResolutionLevel;
			CleanResolutionDependentResources();
			InitResolutionDependentResources(m_bufferResolution, m_bufferResolution);
		}
		ImGui::Text("Render resolution %dx%d", m_width, m_height);
		ImGui::SliderFloat3("light_dir", &m_lightPosition.x, -1.f, 1.f);
		m_lightPosition = glm::normalize(m_lightPosition); // This needs to remain a normalized direction
		ImGui::Image((ImTextureID)(intptr_t)(m_shadowTextureID), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::SliderFloat3("light_dir2", &m_lightPosition2.x, -1.f, 1.f);
		m_lightPosition2 = glm::normalize(m_lightPosition2); // This needs to remain a normalized direction
		ImGui::Image((ImTextureID)(intptr_t)(m_shadowTextureID2), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();
}

// https://wiki.libsdl.org/SDL3/SDL_KeyboardEvent
// https://wiki.libsdl.org/SDL3/SDL_Keysym
// https://wiki.libsdl.org/SDL3/SDL_Keycode
// https://wiki.libsdl.org/SDL3/SDL_Keymod

void CMyApp::KeyboardDown(const SDL_KeyboardEvent& key)
{
	if (!key.repeat) // Triggers only once when held
	{
		if (key.key == SDLK_F5 && key.mod & SDL_KMOD_CTRL) // CTRL + F5
		{
			CleanShaders();
			InitShaders();
		}
		if (key.key == SDLK_F1) // F1
		{
			GLint polygonModeFrontAndBack[2] = {};
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml
			glGetIntegerv(GL_POLYGON_MODE, polygonModeFrontAndBack); // Query the current polygon mode. It gives the front and back modes separately.
			GLenum polygonMode = (polygonModeFrontAndBack[0] != GL_FILL ? GL_FILL : GL_LINE); // Switch between FILL and LINE
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
			glPolygonMode(GL_FRONT_AND_BACK, polygonMode); // Set the new polygon mode
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

// New window size
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	m_camera.SetAspect(static_cast<float>(_w) / _h);
	m_width = _w;
	m_height = _h;
}

// Other SDL events
// https://wiki.libsdl.org/SDL3/SDL_Event

void CMyApp::OtherEvent(const SDL_Event& ev)
{
}