#pragma once
#include "math.h"

class Camera
{
public:
	Camera();
	Camera(const Camera& _other) = default;
	~Camera() = default;

	void SetPosition(float _x, float _y, float _z);
	void SetRotation(float _x, float _y, float _z);

	inline void GetPosition(float* _result) {
		_result[0] = m_positionX;
		_result[1] = m_positionY;
		_result[2] = m_positionZ;
	}

	inline void GetRotation(float* _result) {
		_result[0] = m_rotationX;
		_result[1] = m_rotationY;
		_result[2] = m_rotationZ;
	}

	void Render();
	void GetViewMatrix(float* _result);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float m_viewMatrix[16];
};