#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"

#include <imgui.h>
#include <random>

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
	AttachShader(m_programID, GL_VERTEX_SHADER, "Shaders/Vert_PosCol.vert");
	AttachShader(m_programID, GL_FRAGMENT_SHADER, "Shaders/Frag_PosCol.frag");
	LinkProgram(m_programID);
}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programID);
}

void CMyApp::InitGeometry()
{
	MeshObject<VertexPosColor> meshCPU;

	const float R = 0.5f / (2.0f * sin(glm::pi<float>() / 8.0f));

	std::vector<glm::vec3> baseVertices;

	const int N = 8; // szabályos nyolcszög
	const float a = 0.5f; // élhossz
	const float height = 0.5f; // hasáb magasság fele
	const float r = a / (2.0f * sin(glm::pi<float>() / N)); // külső sugár

	// Alap + tető pontjai (2x8)
	for (int i = 0; i < N; ++i)
	{
		float angle = 2.0f * glm::pi<float>() * i / N;
		float x = r * cos(angle);
		float z = r * sin(angle);

		// alsó pont
		meshCPU.vertexArray.push_back({ glm::vec3(x, -height / 2.0f, z), glm::vec3(0.7f, 0.2f, 0.2f) });

		// felső pont
		meshCPU.vertexArray.push_back({ glm::vec3(x, +height / 2.0f, z), glm::vec3(0.2f, 0.7f, 0.2f) });
	}

	// Középpont alul + felül
	int baseCenterIndex = meshCPU.vertexArray.size();
	meshCPU.vertexArray.push_back({ glm::vec3(0, -height / 2.0f, 0), glm::vec3(0.2f, 0.2f, 1.0f) }); // alul
	meshCPU.vertexArray.push_back({ glm::vec3(0, +height / 2.0f, 0), glm::vec3(0.2f, 0.2f, 1.0f) }); // felül

	// Oldallapok 
	for (int i = 0; i < N; ++i)
	{
		int next = (i + 1) % N;
		int i0 = i * 2;     // alsó
		int i1 = i * 2 + 1; // felső
		int j0 = next * 2;
		int j1 = next * 2 + 1;

		meshCPU.indexArray.push_back(i0);
		meshCPU.indexArray.push_back(j0);
		meshCPU.indexArray.push_back(i1);

		meshCPU.indexArray.push_back(i1);
		meshCPU.indexArray.push_back(j0);
		meshCPU.indexArray.push_back(j1);
	}

	// Alsó nyolcszög
	for (int i = 0; i < N; ++i)
	{
		int next = (i + 1) % N;
		meshCPU.indexArray.push_back(baseCenterIndex);
		meshCPU.indexArray.push_back(next * 2);
		meshCPU.indexArray.push_back(i * 2);
	}

	// Felső nyolcszög 
	for (int i = 0; i < N; ++i)
	{
		int next = (i + 1) % N;
		meshCPU.indexArray.push_back(baseCenterIndex + 1);
		meshCPU.indexArray.push_back(i * 2 + 1);
		meshCPU.indexArray.push_back(next * 2 + 1);
	}

	// hozzunk létre egy új VBO erőforrás nevet
	glCreateBuffers(1, &vboID);

	// töltsük fel adatokkal a VBO-t
	glNamedBufferData(vboID,	// a VBO-ba töltsünk adatokat 
		meshCPU.vertexArray.size() * sizeof(VertexPosColor),		// ennyi bájt nagyságban 
		meshCPU.vertexArray.data(),	// erről a rendszermemóriabeli címről olvasva 
		GL_STATIC_DRAW);	// úgy, hogy a VBO-nkba nem tervezünk ezután írni és minden kirajzoláskor felhasználjuk a benne lévő adatokat 

	// index puffer létrehozása 
	glCreateBuffers(1, &iboID);
	glNamedBufferData(iboID, meshCPU.indexArray.size() * sizeof(GLuint), meshCPU.indexArray.data(), GL_STATIC_DRAW);

	count = static_cast<GLsizei>(meshCPU.indexArray.size());

	// 1 db VAO foglalása 
	glCreateVertexArrays(1, &vaoID);

	// VBO beállítása a VAO-hoz, 0. indexen 
	glVertexArrayVertexBuffer(vaoID, 0, vboID, 0, sizeof(VertexPosColor));

	// attribútumok beállítása

	// 0-as indexű attribútum: pozíció
	glEnableVertexArrayAttrib(vaoID, 0); // engedélyezzük az attribútumot 
	glVertexArrayAttribBinding(vaoID, 0, 0); // az attribútumot a 0. indexű VBO-hoz kötjük 
	glVertexArrayAttribFormat(vaoID, // a VAO-hoz tartozó attribútumokat állítjuk be 
		0,     // a 0. indexű attribútum 
		3,     // 3 komponens (x, y, z) 
		GL_FLOAT, // az adatok típusa 
		GL_FALSE, // az adatok normalizálva vannak-e 
		offsetof(VertexPosColor, position) // az attribútum hol kezdődik a sizeof(Vertex)-nyi területen belül 
	);

	// 1-es indexű attribútum: szín
	glEnableVertexArrayAttrib(vaoID, 1);
	glVertexArrayAttribBinding(vaoID, 1, 0);
	glVertexArrayAttribFormat(vaoID, 1, 3, GL_FLOAT, GL_FALSE, offsetof(VertexPosColor, color));

	// index buffer beállítása a VAO-hoz 
	glVertexArrayElementBuffer(vaoID, iboID);


	for (int i = 0; i < 6; i++) {
		std::default_random_engine rng(std::random_device{}());
		std::uniform_real_distribution<float> dist(-4.0f, 4.0f);
		float x = dist(rng);
		float z = dist(rng);
		float y = -x / 2.0f + x * z / 4.0f + pow(z / 2.0f, 2);
		glm::mat4 randomTransform = glm::translate<float>(glm::vec3(x, y, z));
		m_RandomStarterformArray[i] = randomTransform;
		m_WorldTransformArray[i] = randomTransform;
	}

}

void CMyApp::CleanGeometry()
{
	glDeleteBuffers(1, &vboID);
	glDeleteBuffers(1, &iboID);
	glDeleteVertexArrays(1, &vaoID);
}

bool CMyApp::Init()
{
	SetupDebugCallback();

	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	InitShaders();
	InitGeometry();

	// egyéb inicializálás

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hátrafelé néző lapok eldobását 
	glCullFace(GL_BACK);    // GL_BACK: a kamerától "elfelé" néző lapok, GL_FRONT: a kamera felé néző lapok 

	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás) 

	// kamera 
	m_camera.SetView(
		glm::vec3(0.0, 0.0, 5.0),   // honnan nézzük a színteret  - eye
		glm::vec3(0.0, 0.0, 0.0),   // a színtér melyik pontját nézzük  - at
		glm::vec3(0.0, 1.0, 0.0));  // felfelé mutató irány a világban  - up

	m_cameraManipulator.SetCamera(&m_camera);

	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanGeometry();
}

void CMyApp::Update(const SUpdateInfo& updateInfo)
{
	m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;
	m_cameraManipulator.Update(updateInfo.DeltaTimeInSec);

	// Transzformációs mátrixok 
	/*
	 GLM transzformációs mátrixokra példák:
	 glm::rotate<float>( szög, glm::vec3(tengely_x, tengely_y, tengely_z) ) <- tengely_{xyz} körüli elforgatás
	 glm::translate<float>( glm::vec3(eltol_x, eltol_y, eltol_z) ) <- eltolás
	 glm::scale<float>( glm::vec3(s_x, s_y, s_z) ) <- skálázás

	*/

	static const float SPINNING_PERIOD = 4.0f;
	const float spinning_angle = m_ElapsedTimeInSec / SPINNING_PERIOD * 360.0f;
	glm::mat4 spinningTransform = glm::mat4(1.0);


	if (m_isSpinning) {
		spinningTransform = glm::rotate(glm::radians(spinning_angle), glm::vec3(0.0, 1.0, 0.0));
	}


	static const float TRAVEL_PERIOD = 7.0f;
	float timeInCycle = fmod(m_ElapsedTimeInSec, TRAVEL_PERIOD);
	float phase = timeInCycle / TRAVEL_PERIOD;
	float xPath = -10.0f + 20.0f * 0.5f * (1.0f - cos(2.0f * glm::pi<float>() * phase));
	float yPath = 4.0f * cos(glm::pi<float>() / 5.0f * xPath);

	glm::vec3 pathPos = glm::vec3(xPath, yPath, 0.0f);
	glm::mat4 movement = glm::translate(glm::mat4(1.0f), pathPos);



	for (int i = 0; i < 6; i++) {
		m_WorldTransformArray[i] = movement * m_RandomStarterformArray[i] * spinningTransform;
	}

}

void CMyApp::Render()
{
	// töröljük a framepuffert (GL_COLOR_BUFFER_BIT)...
	// ... és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolása 
	glUseProgram(m_programID);

	// - VAO beállítása 
	glBindVertexArray(vaoID);

	// - Uniform paraméterek
	// view és projekciós mátrix
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	glUniform1f(ul("EmissiveFactor"), fmodf(m_ElapsedTimeInSec, 1.0f));

	glm::vec3 EmissiveColor = glm::vec3(1.0, 0.0, 1.0);
	glUniform3fv(ul("EmissiveColor"), 1, glm::value_ptr(EmissiveColor));

	for (int i = 0; i < 6; i++) {

		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glUniform.xhtml
		glUniformMatrix4fv(ul("world"),// erre a helyre töltsünk át adatot 
			1,			// egy darab mátrixot 
			GL_FALSE,	// NEM transzponálva 
			glm::value_ptr(m_WorldTransformArray[i])); // innen olvasva a 16 x sizeof(float)-nyi adatot 

		// Rajzolási parancs kiadása 
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElements.xhtml
		glDrawElements(GL_TRIANGLES,    // primitív típusa; u.a mint glDrawArrays esetén 
			count,			 // mennyi indexet rajzoljunk 
			GL_UNSIGNED_INT, // indexek típusa 
			nullptr);       // hagyjuk nullptr-en! 

	}



	// shader kikapcsolása 
	glUseProgram(0);

	// VAO kikapcsolása 
	glBindVertexArray(0);
}

void CMyApp::RenderGUI()
{
	// ImGui::ShowDemoWindow();
	if (ImGui::Button(m_isSpinning ? "Stop Spinning" : "Start Spinning")) {
		m_isSpinning = !m_isSpinning;
	}
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
