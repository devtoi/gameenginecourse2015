#include "Camera.h"

#include <glm/gtx/transform2.hpp>	// lookAt, and perspectiveFov

void gfx::Camera::CalculateViewProjection() {
	m_Orientation	= glm::normalize( m_Orientation );
	m_View			= glm::lookAt( m_Position, m_Position + this->GetForward(), this->GetUp() );
	m_Projection	= glm::perspective( m_Lens.VerticalFOV, m_Lens.WindowWidth / static_cast<float>(m_Lens.WindowHeight), m_Lens.Near, m_Lens.Far );
}

void gfx::Camera::MoveWorld(const glm::vec3& distanceToMove) {
	m_Position += distanceToMove;
}

void gfx::Camera::MoveRelative(const glm::vec3& distanceToMove) {
	m_Position += distanceToMove.x * (m_Orientation * m_Right	);
	m_Position += distanceToMove.y * (m_Orientation * m_Up		);
	m_Position -= distanceToMove.z * (m_Orientation * m_Forward	);
}

void gfx::Camera::YawWorld(const float radians) {
	RotateAroundNormalizedAxis( m_Up, radians );
}

void gfx::Camera::YawRelative(const float radians) {
	RotateAroundNormalizedAxis( m_Orientation * m_Up, radians );
}

void gfx::Camera::PitchWorld(const float radians) {
	RotateAroundNormalizedAxis( m_Right, radians );
}

void gfx::Camera::PitchRelative(const float radians) {
	RotateAroundNormalizedAxis( m_Orientation * m_Right, radians );
}

void gfx::Camera::RollWorld(const float radians) {
	RotateAroundNormalizedAxis( m_Forward, radians );
}

void gfx::Camera::RollRelative(const float radians) {
	RotateAroundNormalizedAxis( m_Orientation * m_Forward, radians );
}

void gfx::Camera::RotateAroundNormalizedAxis(const glm::vec3& normalizedRotationAxis, const float radians) {
	float rotationAmount = radians * 0.5f;
	glm::quat rotation( glm::cos(rotationAmount), normalizedRotationAxis * glm::sin(rotationAmount) );
	m_Orientation = glm::normalize( rotation * m_Orientation );
}

void gfx::Camera::RotateWithQuaternion(const glm::quat& rotation) {
	m_Orientation = glm::normalize( rotation * m_Orientation );
}

const glm::vec3 gfx::Camera::GetForward() const {
	return m_Orientation * m_Forward;
}

const glm::vec3 gfx::Camera::GetUp() const {
	return m_Orientation * m_Up;
}

const glm::vec3 gfx::Camera::GetRight() const {
	return m_Orientation * m_Right;
}

const glm::vec3& gfx::Camera::GetPosition() const {
	return m_Position;
}

const glm::quat& gfx::Camera::GetOrientation() const {
	return m_Orientation;
}

const glm::mat4& gfx::Camera::GetView() const {
	return m_View;
}

const glm::mat4& gfx::Camera::GetProjection() const {
	return m_Projection;
}

const glm::mat4 gfx::Camera::GetViewProjection() const {
	return m_Projection * m_View;
}

const gfx::CameraLens& gfx::Camera::GetLens() const {
	return m_Lens;
}

glm::vec3& gfx::Camera::GetEditablePosition() {
	return m_Position;
}

gfx::CameraLens& gfx::Camera::GetEditableLens() {
	return m_Lens;
}

void gfx::Camera::SetPosition(const glm::vec3& newPosition) {
	m_Position = newPosition;
}

void gfx::Camera::SetOrientation(const glm::quat& newOrientation) {
	m_Orientation = newOrientation;
}

void gfx::Camera::SetLens(const CameraLens& newCameraLens) {
	m_Lens = newCameraLens;
}

void gfx::Camera::SetMoveSpeed(const float newMoveSpeed) {
	// Just here to be overriden by its derived classes. But I don't want to force them to have an own implementation.
}