#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"
#include "ProgramBuilder.h"

#include <imgui.h>
#include <iostream>
#include <array>
#include <random>
#include <cmath>
#include <fstream>
#include <sstream>
#include <map>
#include <fstream>
#include <iomanip>



CMyApp::CMyApp()
{
	// Let's see how big worker groups can we make
	std::cout << "GL_MAX_COMPUTE_WORK_GROUP_COUNT:\n";
	int max;
	for (int i : {0, 1, 2})
	{
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, i, &max);
		std::cout << "dim[" << i << "] = " << max << ", ";
	}
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
		.ShaderStage(GL_VERTEX_SHADER, "Shaders/Vert_Poly.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "Shaders/Frag_Poly.frag")
		.Link();

	GLint linked = 0;
	glGetProgramiv(m_programID, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		std::cerr << "Shader program link failed!" << std::endl;
		glDeleteProgram(m_programID);
		m_programID = 0;
	}

}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programID);
	m_programID = 0;
}


void CMyApp::InitBuffers()
{

	glCreateVertexArrays(1, &m_vaoID);
	glCreateBuffers(1, &m_vboID);


	// position
	glEnableVertexArrayAttrib(m_vaoID, 0);
	glVertexArrayAttribFormat(
		m_vaoID, 0, 3, GL_FLOAT, GL_FALSE,
		offsetof(Vertex, position)
	);
	glVertexArrayAttribBinding(m_vaoID, 0, 0);

	// normal
	glEnableVertexArrayAttrib(m_vaoID, 1);
	glVertexArrayAttribFormat(
		m_vaoID, 1, 3, GL_FLOAT, GL_FALSE,
		offsetof(Vertex, normal)
	);
	glVertexArrayAttribBinding(m_vaoID, 1, 0);
}


void CMyApp::CleanBuffers()
{
	for (auto& [N, gpu] : m_polyMeshes)
	{
		glDeleteBuffers(1, &gpu.vbo);
		glDeleteVertexArrays(1, &gpu.vao);
	}
	m_polyMeshes.clear();
}

void CMyApp::InitTextures()
{
	glCreateSamplers(1, &m_SamplerID);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ImageRGBA metal = ImageFromFile("Assets/metal.png");
	glCreateTextures(GL_TEXTURE_2D, 1, &m_metalTextureID);
	glTextureStorage2D(m_metalTextureID, NumberOfMIPLevels(metal), GL_RGBA8, metal.width, metal.height);
	glTextureSubImage2D(m_metalTextureID, 0, 0, 0, metal.width, metal.height, GL_RGBA, GL_UNSIGNED_BYTE, metal.data());
	glGenerateTextureMipmap(m_metalTextureID);

	ImageRGBA wood = ImageFromFile("Assets/wood.jpg");
	glCreateTextures(GL_TEXTURE_2D, 1, &m_woodTextureID);
	glTextureStorage2D(m_woodTextureID, NumberOfMIPLevels(wood), GL_RGBA8, wood.width, wood.height);
	glTextureSubImage2D(m_woodTextureID, 0, 0, 0, wood.width, wood.height, GL_RGBA, GL_UNSIGNED_BYTE, wood.data());
	glGenerateTextureMipmap(m_woodTextureID);
}



void CMyApp::CleanTextures()
{
	glDeleteTextures(1, &m_metalTextureID);
	glDeleteTextures(1, &m_woodTextureID);
	glDeleteSamplers(1, &m_SamplerID);
}




bool CMyApp::Init()
{
	SetupDebugCallback();

	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	m_camera.SetView(
		glm::vec3(0, 0, 5),   // eye
		glm::vec3(0, 0, 0),   // at
		glm::vec3(0, 1, 0)    // up
	);
	m_camera.SetAspect(static_cast<float>(m_width) / m_height);
	m_cameraManipulator.SetCamera(&m_camera);


	InitShaders();
	InitBuffers();
	InitTextures();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	m_faces.clear();

	/*
	FaceNode root;
	root.id = 0;
	root.N = 6;
	root.parent = -1;
	root.model = glm::mat4(1.0f);
	root.children = { 1, 2 };
	m_faces.push_back(root);

	FaceNode right;
	right.id = 1;
	right.N = 6;
	right.parent = 0;
	right.model = glm::translate(glm::vec3(2.0f, 0.0f, 0.0f));
	m_faces.push_back(right);

	FaceNode left;
	left.id = 2;
	left.N = 6;
	left.parent = 0;
	left.model = glm::translate(glm::vec3(-2.0f, 0.0f, 0.0f));
	m_faces.push_back(left);

	m_faces[1].parentEdge = 0;
	m_faces[1].pivot = glm::radians(60.0f);*/

	//if (!LoadPoly("Assets/kocka.poly"))
	/*if (!LoadPoly("Assets/adv.poly"))
	{
		std::cerr << "Failed to load poly file!" << std::endl;
	}*/





	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanBuffers();
}

void CMyApp::Update(const SUpdateInfo& updateInfo)
{
	m_cameraManipulator.Update(updateInfo.DeltaTimeInSec);

	if (m_autoFold)
	{
		m_foldT += m_foldDir * m_foldSpeed * updateInfo.DeltaTimeInSec;

		if (m_foldT >= 1.0f)
		{
			m_foldT = 1.0f;
			m_foldDir = -1;
		}
		else if (m_foldT <= 0.0f)
		{
			m_foldT = 0.0f;
			m_foldDir = 1;
		}
	}
}



void CMyApp::Render()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!m_programID)
		return;

	glUseProgram(m_programID);
	glBindVertexArray(m_vaoID);

	glBindTextureUnit(0, m_metalTextureID);
	glBindSampler(0, m_SamplerID);
	glUniform1i(glGetUniformLocation(m_programID, "textureOutside"), 0);

	glBindTextureUnit(1, m_woodTextureID);
	glBindSampler(1, m_SamplerID);
	glUniform1i(glGetUniformLocation(m_programID, "textureInside"), 1);

	glUniform3fv(ul("cameraPosition"), 1, glm::value_ptr(m_camera.GetEye()));

	glm::mat4 VP = m_camera.GetViewProj();
	GLint mvpLoc = glGetUniformLocation(m_programID, "MVP");

	for (const FaceNode& f : m_faces)
	{
		if (f.parent == -1)
			DrawFaceRecursive(m_faces, f.id, glm::mat4(1.0f), mvpLoc, VP);
	}

	glBindVertexArray(0);
	glUseProgram(0);
}





void CMyApp::RenderGUI()
{
	ImGui::Begin("Folding");

	ImGui::SliderFloat("Fold amount", &m_foldT, 0.0f, 1.0f);

	ImGui::Checkbox("Auto fold", &m_autoFold);

	ImGui::SliderFloat("Speed", &m_foldSpeed, 0.05f, 2.0f);

	ImGui::End();
}

void CMyApp::RenderBuilderGUI()
{
	ImGui::Begin("Poly Builder");



	ImGui::Checkbox("Use N", &gui_useN);
	ImGui::Checkbox("Use Pivot", &gui_usePivot);


	if (ImGui::Button("START") && !m_started && gui_N > 2)
	{
		//ExecutePolyCommand("START", { (float)gui_N }, m_guiPolyState);
		ExecuteAndLog("START", { (float)gui_N });
		m_started = true;
	}

	ImGui::Separator();

	ImGui::InputInt("Edge", &gui_edge);
	if (gui_useN) ImGui::InputInt("N", &gui_N);
	if (gui_usePivot) ImGui::InputFloat("Pivot", &gui_pivot);

	if (ImGui::Button("ADD") && m_started)
	{
		std::vector<float> args;
		args.push_back((float)gui_edge);

		if (gui_useN)
			args.push_back((float)gui_N);
		if (gui_usePivot)
			args.push_back(gui_pivot);

		//ExecutePolyCommand("ADD", args, m_guiPolyState);
		ExecuteAndLog("ADD", args);

	}

	if (ImGui::Button("PUSH") && m_started)
	{
		std::vector<float> args;
		args.push_back((float)gui_edge);

		if (gui_useN)
			args.push_back((float)gui_N);
		if (gui_usePivot)
			args.push_back(gui_pivot);

		//ExecutePolyCommand("PUSH", args, m_guiPolyState);
		ExecuteAndLog("PUSH", args);

	}

	if (ImGui::Button("POP") && m_started)
	{
		//ExecutePolyCommand("POP", {}, m_guiPolyState);
		ExecuteAndLog("POP", {});
	}

	ImGui::SeparatorText("Pivot");

	ImGui::InputInt("N1", &gui_pivot_n1);
	ImGui::InputInt("N2", &gui_pivot_n2);
	ImGui::InputFloat("Pivot value", &gui_pivot_val);

	if (ImGui::Button("PIVOT"))
	{
		/*ExecutePolyCommand(
			"PIVOT",
			{
				(float)gui_pivot_n1,
				(float)gui_pivot_n2,
				gui_pivot_val
			},
			m_guiPolyState
		);*/
		ExecuteAndLog("PIVOT", 
			{
				(float)gui_pivot_n1,
				(float)gui_pivot_n2,
				gui_pivot_val
			});
	}

	ImGui::SeparatorText("Pivot Poly");

	ImGui::InputInt("N1##poly", &gui_pivot_n1);
	ImGui::InputInt("N2##poly", &gui_pivot_n2);
	ImGui::InputInt("N3##poly", &gui_pivot_n3);

	if (ImGui::Button("PIVOT_POLY"))
	{
		/*ExecutePolyCommand(
			"PIVOT_POLY",
			{
				(float)gui_pivot_n1,
				(float)gui_pivot_n2,
				(float)gui_pivot_n3
			},
			m_guiPolyState
		);*/
		ExecuteAndLog(
			"PIVOT_POLY",
			{
				(float)gui_pivot_n1,
				(float)gui_pivot_n2,
				(float)gui_pivot_n3
			});
	}

	ImGui::SeparatorText("Pivot Vertex");

	ImGui::InputInt("N1##v", &gui_pivot_n1);
	ImGui::InputInt("N2##v", &gui_pivot_n2);
	ImGui::InputInt("N3##v", &gui_pivot_n3);

	if (ImGui::Button("PIVOT_VERTEX"))
	{
		/*ExecutePolyCommand(
			"PIVOT_VERTEX",
			{
				(float)gui_pivot_n1,
				(float)gui_pivot_n2,
				(float)gui_pivot_n3
			},
			m_guiPolyState
		);*/

		ExecuteAndLog(
			"PIVOT_VERTEX", 
			{
				(float)gui_pivot_n1,
				(float)gui_pivot_n2,
				(float)gui_pivot_n3
			});
	}





	if (ImGui::Button("RESET"))
	{
		//ExecutePolyCommand("RESET", {}, m_guiPolyState);
		ExecuteAndLog("RESET", {});
		m_started = false;
	}

	static char savePath[256] = "Assets/export.poly";
	ImGui::InputText("Save path", savePath, sizeof(savePath));

	if (ImGui::Button("SAVE .poly"))
	{
		if (!SavePolyFromLog(savePath))
			std::cerr << "Save failed!\n";
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

	if (ev.type == SDL_EVENT_DROP_FILE)
	{
		const char* path = ev.drop.data;
		if (!path)
			return;

		std::string filename(path);

		if (filename.size() >= 5 &&
			filename.substr(filename.size() - 5) == ".poly")
		{
			std::cout << "Dropped poly file: " << filename << std::endl;

			if (!LoadPoly(filename))
			{
				std::cerr << "Failed to load dropped .poly file!" << std::endl;
			}
		}
		else
		{
			std::cout << "Dropped file ignored (not .poly): "
				<< filename << std::endl;
		}
	}
}

std::vector<Vertex> CMyApp::MakePolygon(int N)
{
	std::vector<Vertex> v;

	float R = RadiusForSide(N, 1.0f);

	v.push_back({
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 1),
		glm::vec2(0.5f, 0.5f)
		});

	float step = glm::two_pi<float>() / N;

	for (int i = 0; i <= N; ++i)
	{
		float a = i * step;
		glm::vec3 p(R * cos(a), R * sin(a), 0);

		glm::vec2 uv = glm::vec2(p.x, p.y) / (2.0f * R) + 0.5f;

		v.push_back({
			p,
			glm::vec3(0, 0, 1),
			uv
			});
	}
	return v;
}


void CMyApp::DrawFaceRecursive(
	const std::vector<FaceNode>& faces,
	int id,
	const glm::mat4& parentWorld,
	GLint mvpLoc,
	const glm::mat4& VP
)
{
	const FaceNode& f = faces[id];

	glm::mat4 world;

	if (f.parent == -1)
	{
		world = parentWorld;
	}
	else
	{
		const FaceNode& parent = faces[f.parent];

		world =
			parentWorld
			* PlaceFaceNextToParent(parent, f)
			* FoldTransform(f);
	}

	glm::mat4 MVP = VP * world;
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
	const PolyMeshGPU& mesh = GetOrCreatePolyMesh(f.N);

	glBindVertexArray(mesh.vao);

	glUniform1i(
		glGetUniformLocation(m_programID, "uPolygonN"),
		f.N
	);

	glDrawArrays(GL_TRIANGLE_FAN, 0, mesh.vertexCount);



	for (int child : f.children)
	{
		DrawFaceRecursive(faces, child, world, mvpLoc, VP);
	}
}



glm::vec3 CMyApp::EdgePoint(int edge, int N)
{
	float R = RadiusForSide(N, 1.0f);
	float a = glm::two_pi<float>() * edge / N;
	return glm::vec3(R * cos(a), R * sin(a), 0);
}

glm::vec3 CMyApp::EdgeDir(int edge, int N)
{
	glm::vec3 p0 = EdgePoint(edge, N);
	glm::vec3 p1 = EdgePoint(edge + 1, N);
	return glm::normalize(p1 - p0);
}

glm::mat4 CMyApp::FoldTransform(const FaceNode& f)
{
	//return glm::mat4(1.0f);

	const int hingeEdge = 0;

	glm::vec3 pivotPos = EdgePoint(hingeEdge, f.N);
	glm::vec3 axis = EdgeDir(hingeEdge, f.N);

	return
		glm::translate(pivotPos) *
		glm::rotate(m_foldT * f.pivot, axis) *
		glm::translate(-pivotPos);

}

glm::mat4 CMyApp::PlaceFaceNextToParent(const FaceNode& parent, const FaceNode& child)
{
	int pe = child.parentEdge;

	glm::vec3 p0 = EdgePoint(pe, parent.N);
	glm::vec3 p1 = EdgePoint(pe + 1, parent.N);
	glm::vec3 pMid = 0.5f * (p0 + p1);
	glm::vec3 pDir = glm::normalize(p1 - p0);

	glm::vec3 c0 = EdgePoint(0, child.N);
	glm::vec3 c1 = EdgePoint(1, child.N);
	glm::vec3 cMid = 0.5f * (c0 + c1);
	glm::vec3 cDir = glm::normalize(c1 - c0);

	float angle = atan2(
		glm::cross(cDir, -pDir).z,
		glm::dot(cDir, -pDir)
	);

	return
		glm::translate(pMid)
		* glm::rotate(angle, glm::vec3(0, 0, 1))
		* glm::translate(-cMid);
}


bool CMyApp::LoadPoly(const std::string& filename)
{
	PolyParseState state;
	state.defaultPivot.clear();

	std::ifstream file(filename);
	if (!file.is_open())
		return false;

	m_faces.clear();

	int activeFace = -1;
	int lastN = -1;
	int nextId = 0;
	bool started = false;

	auto trim = [](std::string& s)
		{
			while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ' || s.back() == '\t'))
				s.pop_back();
			size_t i = 0;
			while (i < s.size() && (s[i] == ' ' || s[i] == '\t'))
				++i;
			if (i > 0) s.erase(0, i);
		};

	std::string line;
	while (std::getline(file, line))
	{
		if (auto pos = line.find('#'); pos != std::string::npos)
			line = line.substr(0, pos);

		trim(line);
		if (line.empty())
			continue;

		std::stringstream ss(line);
		std::string cmd;
		ss >> cmd;

		if (cmd == "PIVOT")
		{
			int N1, N2;
			float pv;
			if (!(ss >> N1 >> N2 >> pv))
			{
				std::cerr << "[.poly] PIVOT syntax error: " << line << "\n";
				return false;
			}
			state.defaultPivot[{N1, N2}] = pv;
			state.defaultPivot[{N2, N1}] = pv;
		}
		else if (cmd == "PIVOT_POLY")
		{
			int N1, N2, N3;
			if (!(ss >> N1 >> N2 >> N3))
			{
				std::cerr << "[.poly] PIVOT_POLY syntax error: " << line << "\n";
				return false;
			}
			const float pv = PivotFromVertex(N1, N2, N3);
			state.defaultPivot[{N1, N2}] = pv;
			state.defaultPivot[{N2, N1}] = pv;
		}
		else if (cmd == "PIVOT_VERTEX")
		{
			int N1, N2, N3;
			if (!(ss >> N1 >> N2 >> N3))
			{
				std::cerr << "[.poly] PIVOT_VERTEX syntax error: " << line << "\n";
				return false;
			}

			float pv12 = PivotFromVertex(N1, N2, N3);
			float pv23 = PivotFromVertex(N2, N3, N1);
			float pv31 = PivotFromVertex(N3, N1, N2);
			state.defaultPivot[{N1, N2}] = pv12; state.defaultPivot[{N2, N1}] = pv12;
			state.defaultPivot[{N2, N3}] = pv23; state.defaultPivot[{N3, N2}] = pv23;
			state.defaultPivot[{N3, N1}] = pv31; state.defaultPivot[{N1, N3}] = pv31;
		}
		else if (cmd == "START")
		{
			int N;
			if (!(ss >> N))
			{
				std::cerr << "[.poly] START syntax error: " << line << "\n";
				return false;
			}
			if (started)
			{
				std::cerr << "[.poly] START appears more than once.\n";
				return false;
			}
			started = true;

			FaceNode f;
			f.id = nextId++;
			f.N = N;
			f.parent = -1;
			f.parentEdge = -1;
			f.pivot = 0.0f;
			f.children.clear();

			m_faces.push_back(f);
			activeFace = f.id;
			lastN = N;
		}
		else if (cmd == "ADD" || cmd == "PUSH")
		{
			if (!started || activeFace < 0)
			{
				std::cerr << "[.poly] " << cmd << " before START.\n";
				return false;
			}

			int edge;
			if (!(ss >> edge))
			{
				std::cerr << "[.poly] " << cmd << " syntax error (missing edge): " << line << "\n";
				return false;
			}

			const int parentN = m_faces[activeFace].N;
			int N = lastN;
			//float pivot = GetDefaultPivot(parentN, N, m_guiPolyState);
			float pivot = GetDefaultPivot(parentN, N, state);

			if (ss.good())
			{
				int maybeN;
				std::streampos pos = ss.tellg();
				if (ss >> maybeN)
				{
					N = maybeN;
					//pivot = GetDefaultPivot(parentN, N, m_guiPolyState);
					pivot = GetDefaultPivot(parentN, N, state);
					float maybePivot;
					pos = ss.tellg();
					if (ss >> maybePivot)
					{
						pivot = maybePivot;
					}
				}
				else
				{
					ss.clear();
					ss.seekg(pos);
				}
			}

			if (edge < 0 || edge >= parentN)
			{
				std::cerr << "[.poly] Edge out of range on parent (N=" << parentN << "): " << edge << "\n";
				return false;
			}

			FaceNode f;
			f.id = nextId++;
			f.N = N;
			f.parent = activeFace;
			f.parentEdge = edge;
			f.pivot = pivot;
			f.children.clear();

			m_faces.push_back(f);

			m_faces[activeFace].children.push_back(f.id);

			lastN = N;

			if (cmd == "PUSH")
				activeFace = f.id;
		}
		else if (cmd == "POP")
		{
			if (!started || activeFace < 0)
				continue;

			activeFace = m_faces[activeFace].parent;
		}
		else
		{
			std::cerr << "[.poly] Unknown command: " << cmd << "\n";
			return false;
		}
	}

	return started && !m_faces.empty();
}


void CMyApp::ComputeTransformsRecursive(
	int id,
	const glm::mat4& parentWorld,
	std::vector<glm::mat4>& outWorld
)
{
	const FaceNode& f = m_faces[id];

	glm::mat4 world;

	if (f.parent == -1)
	{
		world = parentWorld;
	}
	else
	{
		const FaceNode& parent = m_faces[f.parent];

		world =
			parentWorld
			* PlaceFaceNextToParent(parent, f)
			* FoldTransform(f);
	}

	outWorld[id] = world;

	for (int c : f.children)
		ComputeTransformsRecursive(c, world, outWorld);
}

float CMyApp::InteriorAngle(int N)
{

	return (N - 2) * glm::pi<float>() / (float)N;
}



const PolyMeshGPU& CMyApp::GetOrCreatePolyMesh(int N)
{
	auto it = m_polyMeshes.find(N);
	if (it != m_polyMeshes.end())
		return it->second;

	PolyMeshGPU gpu;
	std::vector<Vertex> verts = MakePolygon(N);

	glCreateVertexArrays(1, &gpu.vao);
	glCreateBuffers(1, &gpu.vbo);

	glNamedBufferData(gpu.vbo, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);
	glVertexArrayVertexBuffer(gpu.vao, 0, gpu.vbo, 0, sizeof(Vertex));

	// position
	glEnableVertexArrayAttrib(gpu.vao, 0);
	glVertexArrayAttribFormat(gpu.vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexArrayAttribBinding(gpu.vao, 0, 0);

	// normal
	glEnableVertexArrayAttrib(gpu.vao, 1);
	glVertexArrayAttribFormat(gpu.vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	glVertexArrayAttribBinding(gpu.vao, 1, 0);

	//texture
	glEnableVertexArrayAttrib(gpu.vao, 2);
	glVertexArrayAttribFormat(
		gpu.vao,
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		offsetof(Vertex, texcoord)
	);
	glVertexArrayAttribBinding(gpu.vao, 2, 0);

	gpu.vertexCount = static_cast<GLsizei>(verts.size());

	auto [insIt, ok] = m_polyMeshes.emplace(N, gpu);
	return insIt->second;
}


float CMyApp::PivotFromVertex(int N1, int N2, int N3)
{

	const float A = InteriorAngle(N1);
	const float B = InteriorAngle(N2);
	const float C = InteriorAngle(N3);

	const float denom = std::sin(A) * std::sin(B);
	if (std::abs(denom) < 1e-6f)
		return glm::half_pi<float>();

	float x = (std::cos(C) - std::cos(A) * std::cos(B)) / denom;

	x = std::max(-1.0f, std::min(1.0f, x));

	const float dihedral = std::acos(x);
	const float pivot = glm::pi<float>() - dihedral;
	return pivot;
}

float CMyApp::GetDefaultPivot(
	int N1,
	int N2,
	const PolyParseState& state
) const
{
	auto it = state.defaultPivot.find({ N1, N2 });
	if (it != state.defaultPivot.end())
		return it->second;

	it = state.defaultPivot.find({ N2, N1 });
	if (it != state.defaultPivot.end())
		return it->second;

	return glm::half_pi<float>();
}



void CMyApp::ExecutePolyCommand(
	const std::string& cmd,
	const std::vector<float>& args,
	PolyParseState& state
) {

	if (cmd == "START")
	{
		int N = (int)args[0];

		if (N > 2) {
			m_faces.clear();

			FaceNode f;
			f.id = 0;
			f.N = N;
			f.parent = -1;

			m_faces.push_back(f);

			state.activeFace = 0;
			state.lastN = N;
		}
	}

	else if (cmd == "ADD" || cmd == "PUSH")
	{
		int edge = (int)args[0];

		int N = (args.size() >= 2) ? (int)args[1] : state.lastN;

		float pivot;
		if (args.size() >= 3)
		{
			pivot = args[2];
		}
		else
		{
			pivot = GetDefaultPivot(
				m_faces[state.activeFace].N,
				N,
				state
			);
		}


		FaceNode f;
		f.id = (int)m_faces.size();
		f.N = N;
		f.parent = state.activeFace;
		f.parentEdge = edge;
		f.pivot = pivot;

		m_faces.push_back(f);
		m_faces[state.activeFace].children.push_back(f.id);

		state.lastN = N;

		if (cmd == "PUSH")
			state.activeFace = f.id;
	}

	else if (cmd == "POP")
	{
		if (state.activeFace >= 0)
			state.activeFace = m_faces[state.activeFace].parent;
	}
	else if (cmd == "PIVOT")
	{
		int n1 = (int)args[0];
		int n2 = (int)args[1];
		float val = args[2];

		state.defaultPivot[{n1, n2}] = val;
		state.defaultPivot[{n2, n1}] = val;
	}

	else if (cmd == "RESET")
	{
		m_faces.clear();
	}

	else if (cmd == "PIVOT_POLY")
	{
		int n1 = (int)args[0];
		int n2 = (int)args[1];
		int n3 = (int)args[2];

		float pivot = PivotFromVertex(n1, n2, n3);

		state.defaultPivot[{n1, n2}] = pivot;
		state.defaultPivot[{n2, n1}] = pivot;
	}

	else if (cmd == "PIVOT_VERTEX")
	{
		int a = (int)args[0];
		int b = (int)args[1];
		int c = (int)args[2];

		ExecutePolyCommand("PIVOT_POLY", { (float)a, (float)b, (float)c }, state);
		ExecutePolyCommand("PIVOT_POLY", { (float)b, (float)c, (float)a }, state);
		ExecutePolyCommand("PIVOT_POLY", { (float)c, (float)a, (float)b }, state);
	}

}

float CMyApp::RadiusForSide(int N, float side) const
{
	return side / (2.0f * std::sin(glm::pi<float>() / float(N)));
}

void CMyApp::ExecuteAndLog(const std::string& cmd, const std::vector<float>& args)
{
	if (cmd == "RESET")
	{
		m_commandLog.clear();
		m_started = false;
		ExecutePolyCommand(cmd, args, m_guiPolyState);
		return;
	}

	ExecutePolyCommand(cmd, args, m_guiPolyState);

	m_commandLog.push_back({ cmd, args });
}



static void WriteCmd(std::ofstream& out, const PolyCommand& c)
{
	out << c.cmd;
	out << std::setprecision(10) << std::fixed;

	for (float a : c.args)
	{
		if (c.cmd == "START" || c.cmd == "ADD" || c.cmd == "PUSH" ||
			c.cmd == "PIVOT" || c.cmd == "PIVOT_POLY" || c.cmd == "PIVOT_VERTEX")
		{
			float r = std::round(a);
			if (std::abs(a - r) < 1e-6f)
				out << " " << (int)r;
			else
				out << " " << a;
		}
		else
		{
			out << " " << a;
		}
	}
	out << "\n";
}

bool CMyApp::SavePolyFromLog(const std::string& filename) const
{
	if (m_commandLog.empty())
		return false;

	std::ofstream out(filename);
	if (!out.is_open())
		return false;

	out << "# Exported from GUI\n";
	for (const auto& c : m_commandLog)
		WriteCmd(out, c);

	return true;
}