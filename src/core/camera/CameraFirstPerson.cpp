#include "CameraFirstPerson.h"

#include <input/InputContext.h>

void CameraFirstPerson::Update( const float deltaTime ) {
	m_MoveSpeed = g_Input.KeyDown( SDL_SCANCODE_KP_0 ) ? m_AlternativeSpeed : m_MainSpeed;

	// Look right
	if ( ( g_Input.GetMousePosDeltaX() != 0 ) && g_Input.MouseButtonDown( MOUSE_BUTTON_RIGHT ) ) {
		this->LookRight( g_Input.GetMousePosDeltaX() * CAMERA_FIRST_PERSON_TURN_SPEED );
	}

	// Look up
	if ( ( g_Input.GetMousePosDeltaY() != 0 ) && g_Input.MouseButtonDown( MOUSE_BUTTON_RIGHT ) ) {
		this->LookUp( -g_Input.GetMousePosDeltaY() * CAMERA_FIRST_PERSON_TURN_SPEED );
	}

	// Move forward
	if ( g_Input.KeyDown( SDL_SCANCODE_UP ) ) {
		this->MoveForward( m_MoveSpeed * deltaTime );
	}

	// Move back
	if ( g_Input.KeyDown( SDL_SCANCODE_DOWN ) ) {
		this->MoveForward( -m_MoveSpeed * deltaTime );
	}

	// Move right
	if ( g_Input.KeyDown( SDL_SCANCODE_RIGHT ) ) {
		this->MoveRight( m_MoveSpeed * deltaTime );
	}

	// Move left
	if ( g_Input.KeyDown( SDL_SCANCODE_LEFT ) ) {
		this->MoveRight( -m_MoveSpeed * deltaTime );
	}

	// Float up
	if ( g_Input.KeyDown( SDL_SCANCODE_RSHIFT ) ) {
		this->MoveUp( m_MoveSpeed * deltaTime );
	}

	// Float down
	if ( g_Input.KeyDown( SDL_SCANCODE_RCTRL ) ) {
		this->MoveUp( -m_MoveSpeed * deltaTime );
	}
}

void CameraFirstPerson::MoveForward( const float distance ) {
	this->MoveRelative( glm::vec3( 0.0f, 0.0f, -distance ) );
}

void CameraFirstPerson::MoveRight( const float distance ) {
	this->MoveRelative( glm::vec3( distance, 0.0f, 0.0f ) );
}

void CameraFirstPerson::MoveUp( const float distance ) {
	this->MoveWorld( glm::vec3( 0.0f, distance, 0.0f ) );
}

void CameraFirstPerson::LookUp( const float radians ) {
	this->PitchRelative( radians );
}

void CameraFirstPerson::LookRight( const float radians ) {
	this->YawWorld( -radians );
}

void CameraFirstPerson::SetMoveSpeed( const float newMoveSpeed ) {
	m_MainSpeed = newMoveSpeed;
}

void CameraFirstPerson::SetAlternativeMoveSpeed( const float newMoveSpeed ) {
	m_AlternativeSpeed = newMoveSpeed;
}

