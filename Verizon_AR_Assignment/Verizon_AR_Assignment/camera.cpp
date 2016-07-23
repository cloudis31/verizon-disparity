#include "camera.h"

Camera::Camera()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

void Camera::SetPosition(float _x, float _y, float _z)
{
	m_positionX = _x;
	m_positionY = _y;
	m_positionZ = _z;
	return;
}


void Camera::SetRotation(float _x, float _y, float _z)
{
	m_rotationX = _x;
	m_rotationY = _y;
	m_rotationZ = _z;
	return;
}

void Camera::Render()
{
	Math::Vec3 up, position, lookAt;
	float yaw, pitch, roll;
	float rotationMatrix[9];
	
	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	Math::MatrixRotationYawPitchRoll(rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	Math::TransformCoord(lookAt, rotationMatrix);
	Math::TransformCoord(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt.x = position.x + lookAt.x;
	lookAt.y = position.y + lookAt.y;
	lookAt.z = position.z + lookAt.z;

	// Finally create the view matrix from the three updated vectors.
	Math::BuildViewMatrix(m_viewMatrix, position, lookAt, up);

	return;
}

void Camera::GetViewMatrix(float* _result)
{
	_result[0] = m_viewMatrix[0];
	_result[1] = m_viewMatrix[1];
	_result[2] = m_viewMatrix[2];
	_result[3] = m_viewMatrix[3];

	_result[4] = m_viewMatrix[4];
	_result[5] = m_viewMatrix[5];
	_result[6] = m_viewMatrix[6];
	_result[7] = m_viewMatrix[7];

	_result[8] = m_viewMatrix[8];
	_result[9] = m_viewMatrix[9];
	_result[10] = m_viewMatrix[10];
	_result[11] = m_viewMatrix[11];

	_result[12] = m_viewMatrix[12];
	_result[13] = m_viewMatrix[13];
	_result[14] = m_viewMatrix[14];
	_result[15] = m_viewMatrix[15];

	return;
}