#pragma once

#include <glm/glm.hpp>

class Camera;

struct SDL_KeyboardEvent;
struct SDL_MouseMotionEvent;
struct SDL_MouseWheelEvent;

class CameraManipulator
{
public:
    CameraManipulator();

    ~CameraManipulator();

	void SetCamera( Camera* _pCamera );
    void Update( float _deltaTime );


	void KeyboardDown(const SDL_KeyboardEvent& key);
	void KeyboardUp(const SDL_KeyboardEvent& key);
	void MouseMove(const SDL_MouseMotionEvent& mouse);
	void MouseWheel(const SDL_MouseWheelEvent& wheel );

private:
    Camera* m_pCamera = nullptr;

	// Az u koordináta a gömbkoordináta páros (u,v) része, ami a
	// nézési irányt adja meg az m_eye pozícióból.
	float	m_u = 0.0f;

	// Az v koordináta a gömbkoordináta páros (u,v) része, ami a
	// nézési irányt adja meg az m_eye pozícióból.
	float	m_v = 0.0f;

	// A távolság a nézési cél pozíciója és a kamera pozíciója között. 
	float	m_distance = 0.0f;

	// A középpontja a kör modellnek. 
	glm::vec3 m_center = glm::vec3( 0.0f );
};
