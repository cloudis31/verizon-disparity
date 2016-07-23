#pragma once
#include <math.h>

namespace Math
{
	struct Vec3
	{
		float x, y, z;
	};

	inline void BuildIdentityMatrix(float* _result)
	{
		_result[0] = 1.0f;
		_result[1] = 0.0f;
		_result[2] = 0.0f;
		_result[3] = 0.0f;

		_result[4] = 0.0f;
		_result[5] = 1.0f;
		_result[6] = 0.0f;
		_result[7] = 0.0f;

		_result[8] = 0.0f;
		_result[9] = 0.0f;
		_result[10] = 1.0f;
		_result[11] = 0.0f;

		_result[12] = 0.0f;
		_result[13] = 0.0f;
		_result[14] = 0.0f;
		_result[15] = 1.0f;

		return;
	}

	inline void BuildPerspectiveFovLHMatrix(float* _result, float _fieldOfView, float _screenAspect, float _screenNear, float _screenDepth)
	{
		_result[0] = 1.0f / (_screenAspect * tan(_fieldOfView * 0.5f));
		_result[1] = 0.0f;
		_result[2] = 0.0f;
		_result[3] = 0.0f;

		_result[4] = 0.0f;
		_result[5] = 1.0f / tan(_fieldOfView * 0.5f);
		_result[6] = 0.0f;
		_result[7] = 0.0f;

		_result[8] = 0.0f;
		_result[9] = 0.0f;
		_result[10] = _screenDepth / (_screenDepth - _screenNear);
		_result[11] = 1.0f;

		_result[12] = 0.0f;
		_result[13] = 0.0f;
		_result[14] = (-_screenNear * _screenDepth) / (_screenDepth - _screenNear);
		_result[15] = 0.0f;

		return;
	}

	inline void MatrixRotationY(float* _result, float _angle)
	{
		_result[0] = cosf(_angle);
		_result[1] = 0.0f;
		_result[2] = -sinf(_angle);
		_result[3] = 0.0f;

		_result[4] = 0.0f;
		_result[5] = 1.0f;
		_result[6] = 0.0f;
		_result[7] = 0.0f;

		_result[8] = sinf(_angle);
		_result[9] = 0.0f;
		_result[10] = cosf(_angle);
		_result[11] = 0.0f;

		_result[12] = 0.0f;
		_result[13] = 0.0f;
		_result[14] = 0.0f;
		_result[15] = 1.0f;

		return;
	}

	inline void MatrixRotationX(float* _result, float _angle)
	{
		_result[0] = 1.0f;
		_result[1] = 0.0f;
		_result[2] = 0.0f;
		_result[3] = 0.0f;

		_result[4] = 0.0f;
		_result[5] = cosf(_angle);
		_result[6] = sinf(_angle);
		_result[7] = 0.0f;

		_result[8] = 0.0f;
		_result[9] = -sinf(_angle);
		_result[10] = cosf(_angle);
		_result[11] = 0.0f;

		_result[12] = 0.0f;
		_result[13] = 0.0f;
		_result[14] = 0.0f;
		_result[15] = 1.0f;

		return;
	}

	inline void MatrixRotationZ(float* _result, float _angle)
	{
		_result[0] = cosf(_angle);
		_result[1] = sinf(_angle);
		_result[2] = 0.0f;
		_result[3] = 0.0f;

		_result[4] = -sinf(_angle);
		_result[5] = cosf(_angle);
		_result[6] = 0.0f;
		_result[7] = 0.0f;

		_result[8] = 0.0f;
		_result[9] = 0.0f;
		_result[10] = 1.0f;
		_result[11] = 0.0f;

		_result[12] = 0.0f;
		_result[13] = 0.0f;
		_result[14] = 0.0f;
		_result[15] = 1.0f;

		return;
	}

	inline void MatrixTranslation(float* _result, float _x, float _y, float _z)
	{
		_result[0] = 1.0f;
		_result[1] = 0.0f;
		_result[2] = 0.0f;
		_result[3] = 0.0f;

		_result[4] = 0.0f;
		_result[5] = 1.0f;
		_result[6] = 0.0f;
		_result[7] = 0.0f;

		_result[8] = 0.0f;
		_result[9] = 0.0f;
		_result[10] = 1.0f;
		_result[11] = 0.0f;

		_result[12] = _x;
		_result[13] = _y;
		_result[14] = _z;
		_result[15] = 1.0f;

		return;
	}

	inline void MatrixMultiply(float* _result, float* _matrix1, float* _matrix2)
	{
		_result[0] = (_matrix1[0] * _matrix2[0]) + (_matrix1[1] * _matrix2[4]) + (_matrix1[2] * _matrix2[8]) + (_matrix1[3] * _matrix2[12]);
		_result[1] = (_matrix1[0] * _matrix2[1]) + (_matrix1[1] * _matrix2[5]) + (_matrix1[2] * _matrix2[9]) + (_matrix1[3] * _matrix2[13]);
		_result[2] = (_matrix1[0] * _matrix2[2]) + (_matrix1[1] * _matrix2[6]) + (_matrix1[2] * _matrix2[10]) + (_matrix1[3] * _matrix2[14]);
		_result[3] = (_matrix1[0] * _matrix2[3]) + (_matrix1[1] * _matrix2[7]) + (_matrix1[2] * _matrix2[11]) + (_matrix1[3] * _matrix2[15]);

		_result[4] = (_matrix1[4] * _matrix2[0]) + (_matrix1[5] * _matrix2[4]) + (_matrix1[6] * _matrix2[8]) + (_matrix1[7] * _matrix2[12]);
		_result[5] = (_matrix1[4] * _matrix2[1]) + (_matrix1[5] * _matrix2[5]) + (_matrix1[6] * _matrix2[9]) + (_matrix1[7] * _matrix2[13]);
		_result[6] = (_matrix1[4] * _matrix2[2]) + (_matrix1[5] * _matrix2[6]) + (_matrix1[6] * _matrix2[10]) + (_matrix1[7] * _matrix2[14]);
		_result[7] = (_matrix1[4] * _matrix2[3]) + (_matrix1[5] * _matrix2[7]) + (_matrix1[6] * _matrix2[11]) + (_matrix1[7] * _matrix2[15]);

		_result[8] = (_matrix1[8] * _matrix2[0]) + (_matrix1[9] * _matrix2[4]) + (_matrix1[10] * _matrix2[8]) + (_matrix1[11] * _matrix2[12]);
		_result[9] = (_matrix1[8] * _matrix2[1]) + (_matrix1[9] * _matrix2[5]) + (_matrix1[10] * _matrix2[9]) + (_matrix1[11] * _matrix2[13]);
		_result[10] = (_matrix1[8] * _matrix2[2]) + (_matrix1[9] * _matrix2[6]) + (_matrix1[10] * _matrix2[10]) + (_matrix1[11] * _matrix2[14]);
		_result[11] = (_matrix1[8] * _matrix2[3]) + (_matrix1[9] * _matrix2[7]) + (_matrix1[10] * _matrix2[11]) + (_matrix1[11] * _matrix2[15]);

		_result[12] = (_matrix1[12] * _matrix2[0]) + (_matrix1[13] * _matrix2[4]) + (_matrix1[14] * _matrix2[8]) + (_matrix1[15] * _matrix2[12]);
		_result[13] = (_matrix1[12] * _matrix2[1]) + (_matrix1[13] * _matrix2[5]) + (_matrix1[14] * _matrix2[9]) + (_matrix1[15] * _matrix2[13]);
		_result[14] = (_matrix1[12] * _matrix2[2]) + (_matrix1[13] * _matrix2[6]) + (_matrix1[14] * _matrix2[10]) + (_matrix1[15] * _matrix2[14]);
		_result[15] = (_matrix1[12] * _matrix2[3]) + (_matrix1[13] * _matrix2[7]) + (_matrix1[14] * _matrix2[11]) + (_matrix1[15] * _matrix2[15]);

		return;
	}

	inline void MatrixRotationYawPitchRoll(float* _result, float _yaw, float _pitch, float _roll)
	{
		float cYaw, cPitch, cRoll, sYaw, sPitch, sRoll;

		// Get the cosine and sin of the yaw, pitch, and roll.
		cYaw = cosf(_yaw);
		cPitch = cosf(_pitch);
		cRoll = cosf(_roll);

		sYaw = sinf(_yaw);
		sPitch = sinf(_pitch);
		sRoll = sinf(_roll);

		// Calculate the yaw, pitch, roll rotation matrix.
		_result[0] = (cRoll * cYaw) + (sRoll * sPitch * sYaw);
		_result[1] = (sRoll * cPitch);
		_result[2] = (cRoll * -sYaw) + (sRoll * sPitch * cYaw);

		_result[3] = (-sRoll * cYaw) + (cRoll * sPitch * sYaw);
		_result[4] = (cRoll * cPitch);
		_result[5] = (sRoll * sYaw) + (cRoll * sPitch * cYaw);

		_result[6] = (cPitch * sYaw);
		_result[7] = -sPitch;
		_result[8] = (cPitch * cYaw);

		return;
	}

	inline void TransformCoord(Vec3& _result, float* _matrix)
	{
		float x, y, z;

		// Transform the vector by the 3x3 matrix.
		x = (_result.x * _matrix[0]) + (_result.y * _matrix[3]) + (_result.z * _matrix[6]);
		y = (_result.x * _matrix[1]) + (_result.y * _matrix[4]) + (_result.z * _matrix[7]);
		z = (_result.x * _matrix[2]) + (_result.y * _matrix[5]) + (_result.z * _matrix[8]);

		// Store the result in the reference.
		_result.x = x;
		_result.y = y;
		_result.z = z;

		return;
	}

	inline void BuildViewMatrix(float* _result, Vec3 _position, Vec3 _lookAt, Vec3 _up)
	{
		Vec3 zAxis, xAxis, yAxis;
		float length, result1, result2, result3;

		// zAxis = normal(lookAt - position)
		zAxis.x = _lookAt.x - _position.x;
		zAxis.y = _lookAt.y - _position.y;
		zAxis.z = _lookAt.z - _position.z;
		length = sqrt((zAxis.x * zAxis.x) + (zAxis.y * zAxis.y) + (zAxis.z * zAxis.z));
		zAxis.x = zAxis.x / length;
		zAxis.y = zAxis.y / length;
		zAxis.z = zAxis.z / length;

		// xAxis = normal(cross(up, zAxis))
		xAxis.x = (_up.y * zAxis.z) - (_up.z * zAxis.y);
		xAxis.y = (_up.z * zAxis.x) - (_up.x * zAxis.z);
		xAxis.z = (_up.x * zAxis.y) - (_up.y * zAxis.x);
		length = sqrt((xAxis.x * xAxis.x) + (xAxis.y * xAxis.y) + (xAxis.z * xAxis.z));
		xAxis.x = xAxis.x / length;
		xAxis.y = xAxis.y / length;
		xAxis.z = xAxis.z / length;

		// yAxis = cross(zAxis, xAxis)
		yAxis.x = (zAxis.y * xAxis.z) - (zAxis.z * xAxis.y);
		yAxis.y = (zAxis.z * xAxis.x) - (zAxis.x * xAxis.z);
		yAxis.z = (zAxis.x * xAxis.y) - (zAxis.y * xAxis.x);

		// -dot(xAxis, position)
		result1 = ((xAxis.x * _position.x) + (xAxis.y * _position.y) + (xAxis.z * _position.z)) * -1.0f;

		// -dot(yaxis, eye)
		result2 = ((yAxis.x * _position.x) + (yAxis.y * _position.y) + (yAxis.z * _position.z)) * -1.0f;

		// -dot(zaxis, eye)
		result3 = ((zAxis.x * _position.x) + (zAxis.y * _position.y) + (zAxis.z * _position.z)) * -1.0f;

		// Set the computed values in the view matrix.
		_result[0] = xAxis.x;
		_result[1] = yAxis.x;
		_result[2] = zAxis.x;
		_result[3] = 0.0f;

		_result[4] = xAxis.y;
		_result[5] = yAxis.y;
		_result[6] = zAxis.y;
		_result[7] = 0.0f;

		_result[8] = xAxis.z;
		_result[9] = yAxis.z;
		_result[10] = zAxis.z;
		_result[11] = 0.0f;

		_result[12] = result1;
		_result[13] = result2;
		_result[14] = result3;
		_result[15] = 1.0f;

		return;
	}
}