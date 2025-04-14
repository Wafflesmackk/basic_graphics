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
	AttachShader( m_programID, GL_VERTEX_SHADER, "Shaders/Vert_PosCol.vert" );
	AttachShader( m_programID, GL_FRAGMENT_SHADER, "Shaders/Frag_PosCol.frag" );
	LinkProgram( m_programID );
}

void CMyApp::CleanShaders()
{
	glDeleteProgram( m_programID );
}

void CMyApp::InitGeometry()
{
	MeshObject<VertexPosColor> meshCPU;

	meshCPU.vertexArray = 
	{
		{ glm::vec3( -1, -1, 0), glm::vec3(1, 0, 0) },
		{ glm::vec3(  1, -1, 0), glm::vec3(0, 1, 0) },
		{ glm::vec3( -1,  1, 0), glm::vec3(0, 0, 1) },
		{ glm::vec3(  1,  1, 0), glm::vec3(1, 1, 1) }
	};

	meshCPU.indexArray =
	{
		0, 1, 2,
		1, 3, 2
	};

	// hozzunk létre egy új VBO erőforrás nevet
	glCreateBuffers( 1, &vboID );

	// töltsük fel adatokkal a VBO-t
	glNamedBufferData( vboID,	// a VBO-ba töltsünk adatokat 
					   meshCPU.vertexArray.size() * sizeof( VertexPosColor ),		// ennyi bájt nagyságban 
					   meshCPU.vertexArray.data(),	// erről a rendszermemóriabeli címről olvasva 
					   GL_STATIC_DRAW );	// úgy, hogy a VBO-nkba nem tervezünk ezután írni és minden kirajzoláskor felhasználjuk a benne lévő adatokat 

	// index puffer létrehozása 
	glCreateBuffers( 1, &iboID );
	glNamedBufferData( iboID, meshCPU.indexArray.size() * sizeof( GLuint ), meshCPU.indexArray.data(), GL_STATIC_DRAW );

	count = static_cast<GLsizei>( meshCPU.indexArray.size() );

	// 1 db VAO foglalása 
	glCreateVertexArrays( 1, &vaoID );

	// VBO beállítása a VAO-hoz, 0. indexen 
	glVertexArrayVertexBuffer( vaoID, 0, vboID, 0, sizeof( VertexPosColor ) );

	// attribútumok beállítása
	
	// 0-as indexű attribútum: pozíció
	glEnableVertexArrayAttrib( vaoID, 0 ); // engedélyezzük az attribútumot 
	glVertexArrayAttribBinding( vaoID, 0, 0 ); // az attribútumot a 0. indexű VBO-hoz kötjük 
	glVertexArrayAttribFormat( vaoID, // a VAO-hoz tartozó attribútumokat állítjuk be 
							   0,     // a 0. indexű attribútum 
							   3,     // 3 komponens (x, y, z) 
							   GL_FLOAT, // az adatok típusa 
							   GL_FALSE, // az adatok normalizálva vannak-e 
							   offsetof( VertexPosColor, position ) // az attribútum hol kezdődik a sizeof(Vertex)-nyi területen belül 
	);

	// 1-es indexű attribútum: szín
	glEnableVertexArrayAttrib( vaoID, 1 );
	glVertexArrayAttribBinding( vaoID, 1, 0 );
	glVertexArrayAttribFormat( vaoID, 1, 3, GL_FLOAT, GL_FALSE, offsetof( VertexPosColor, color ) );

	// index buffer beállítása a VAO-hoz 
	glVertexArrayElementBuffer( vaoID, iboID );
}

void CMyApp::CleanGeometry()
{
	glDeleteBuffers(1,      &vboID);
	glDeleteBuffers(1,      &iboID);
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

	m_cameraManipulator.SetCamera( &m_camera );

	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanGeometry();
}

void CMyApp::Update( const SUpdateInfo& updateInfo )
{
	m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;
	m_cameraManipulator.Update( updateInfo.DeltaTimeInSec );

    m_objectWorldTransform = glm::identity<glm::mat4>();
}

void CMyApp::Render()
{
	// töröljük a framepuffert (GL_COLOR_BUFFER_BIT)...
	// ... és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// shader bekapcsolása 
	glUseProgram( m_programID );

	// - Uniform paraméterek
	// view és projekciós mátrix
	glUniformMatrix4fv( ul("viewProj"), 1, GL_FALSE, glm::value_ptr( m_camera.GetViewProj() ) );

	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glUniform.xhtml
	glUniformMatrix4fv( ul("world"),// erre a helyre töltsünk át adatot 
						1,			// egy darab mátrixot 
						GL_FALSE,	// NEM transzponálva 
						glm::value_ptr( m_objectWorldTransform ) ); // innen olvasva a 16 x sizeof(float)-nyi adatot 

	// - VAO beállítása 
	glBindVertexArray( vaoID );

	// Rajzolási parancs kiadása 
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElements.xhtml
	glDrawElements( GL_TRIANGLES,    // primitív típusa; u.a mint glDrawArrays esetén 
					count,			 // mennyi indexet rajzoljunk 
					GL_UNSIGNED_INT, // indexek típusa 
					nullptr );       // hagyjuk nullptr-en! 

	// shader kikapcsolása 
	glUseProgram( 0 );

	// VAO kikapcsolása 
	glBindVertexArray( 0 );
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
	if ( key.repeat == 0 ) // Először lett megnyomva 
	{
		if ( key.keysym.sym == SDLK_F5 && key.keysym.mod & KMOD_CTRL )
		{
			CleanShaders();
			InitShaders();
		}
		if ( key.keysym.sym == SDLK_F1 )
		{
			GLint polygonModeFrontAndBack[ 2 ] = {};
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml
			glGetIntegerv( GL_POLYGON_MODE, polygonModeFrontAndBack ); // Kérdezzük le a jelenlegi polygon módot! Külön adja a front és back módokat. 
			GLenum polygonMode = ( polygonModeFrontAndBack[ 0 ] != GL_FILL ? GL_FILL : GL_LINE ); // Váltogassuk FILL és LINE között! 
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
			glPolygonMode( GL_FRONT_AND_BACK, polygonMode ); // Állítsuk be az újat! 
		}
	}
	m_cameraManipulator.KeyboardDown( key );
}

void CMyApp::KeyboardUp(const SDL_KeyboardEvent& key)
{
	m_cameraManipulator.KeyboardUp( key );
}

// https://wiki.libsdl.org/SDL2/SDL_MouseMotionEvent

void CMyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	m_cameraManipulator.MouseMove( mouse );
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
	m_cameraManipulator.MouseWheel( wheel );
}


// a két paraméterben az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	m_camera.SetAspect( static_cast<float>(_w) / _h );
}

// Le nem kezelt, egzotikus esemény kezelése
// https://wiki.libsdl.org/SDL2/SDL_Event

void CMyApp::OtherEvent( const SDL_Event& ev )
{

}
