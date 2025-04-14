#include "CameraManipulator.h"

#include "Camera.h"

#include <SDL2/SDL.h>

CameraManipulator::CameraManipulator()
{
}

CameraManipulator::~CameraManipulator()
{
}

void CameraManipulator::SetCamera( Camera* _pCamera )
{
    m_pCamera = _pCamera;

    if ( !m_pCamera ) return;
}

void CameraManipulator::Update( float _deltaTime )
{
    if ( !m_pCamera ) return;
}


void CameraManipulator::KeyboardDown(const SDL_KeyboardEvent& key)
{
	switch ( key.keysym.sym )
	{
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		break;
	case SDLK_w:
		break;
	case SDLK_s:
		break;
	case SDLK_a:
		break;
	case SDLK_d:
		break;
	case SDLK_e:
		break;
	case SDLK_q:
		break;
	}
}

void CameraManipulator::KeyboardUp(const SDL_KeyboardEvent& key)
{
	
	switch ( key.keysym.sym )
	{
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		break;
	case SDLK_w:
	case SDLK_s:
		break;
	case SDLK_a:
	case SDLK_d:
		break;
	case SDLK_q:
	case SDLK_e:
		break;
	}
}


void CameraManipulator::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	if ( mouse.state & SDL_BUTTON_LMASK )
	{
	}
	if ( mouse.state & SDL_BUTTON_RMASK )
	{
	}
}

void CameraManipulator::MouseWheel(const SDL_MouseWheelEvent& wheel)
{
}
