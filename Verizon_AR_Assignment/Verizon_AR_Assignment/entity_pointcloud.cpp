#include "ogl.h"
#include "entity_pointcloud.h"

Entity_PointCloud::Entity_PointCloud()
{
}

bool Entity_PointCloud::Initialize(OpenGLRenderer* _renderer, ColorShader::VertexType* _points, int _numPoints)
{
	bool result;

	m_Renderer = _renderer;

	// Initialize the world/model matrix to the identity matrix.
	Math::BuildIdentityMatrix(m_worldMatrix);

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = _initializeBuffers(_points, _numPoints);
	if (!result)
	{
		return false;
	}

	return true;
}

void Entity_PointCloud::Shutdown()
{
	// Release the vertex and index buffers.
	_shutdownBuffers();

	return;
}

void Entity_PointCloud::Reset()
{
	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;
}

void Entity_PointCloud::Update()
{
	// get rotation x mat
	float rotationXMatrix[16];
	Math::MatrixRotationX(rotationXMatrix, m_rotationX);

	// get rotation y mat
	float rotationYMatrix[16];
	Math::MatrixRotationY(rotationYMatrix, m_rotationY);

	// get rotation z mat
	float rotationZMatrix[16];
	Math::MatrixRotationZ(rotationZMatrix, m_rotationZ);

	// get trans mat
	float translationMatrix[16];
	Math::MatrixTranslation(translationMatrix, m_positionX, m_positionY, m_positionZ);

	// get world mat
	Math::MatrixMultiply(m_worldMatrix, rotationXMatrix, rotationYMatrix);
	Math::MatrixMultiply(m_worldMatrix, m_worldMatrix, rotationZMatrix);
	Math::MatrixMultiply(m_worldMatrix, m_worldMatrix, translationMatrix);
}

void Entity_PointCloud::Render(float* viewMatrix)
{
	// get projection mat
	float projectionMatrix[16];
	m_Renderer->GetProjectionMatrix(projectionMatrix);

	// set the shader stuff
	m_Renderer->SetShader(GetShaderID());
	static_cast<ColorShader*>(m_Renderer->GetShader(GetShaderID()))->SetShaderParameters(m_worldMatrix, viewMatrix, projectionMatrix);

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_renderBuffers();

	return;
}

bool Entity_PointCloud::_initializeBuffers(ColorShader::VertexType* points, int numPoints)
{
	m_vertexCount = numPoints;
	
	// Allocate an OpenGL vertex array object.
	m_Renderer->glGenVertexArrays(1, &m_vertexArrayId);

	// Bind the vertex array object to store all the buffers and vertex attributes we create here.
	m_Renderer->glBindVertexArray(m_vertexArrayId);

	// Generate an ID for the vertex buffer.
	m_Renderer->glGenBuffers(1, &m_vertexBufferId);

	// Bind the vertex buffer and load the vertex (position and color) data into the vertex buffer.
	m_Renderer->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	m_Renderer->glBufferData(GL_ARRAY_BUFFER, m_vertexCount * sizeof(ColorShader::VertexType), points, GL_STATIC_DRAW);

	// Enable the two vertex array attributes.
	m_Renderer->glEnableVertexAttribArray(0);  // Vertex position.
	m_Renderer->glEnableVertexAttribArray(1);  // Vertex color.

	// Specify the location and format of the position portion of the vertex buffer.
	m_Renderer->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	m_Renderer->glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(ColorShader::VertexType), 0);

	// Specify the location and format of the color portion of the vertex buffer.
	m_Renderer->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	m_Renderer->glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(ColorShader::VertexType), (unsigned char*)(3 * sizeof(float)));

	return true;
}
void Entity_PointCloud::_shutdownBuffers()
{
	// Disable the two vertex array attributes.
	m_Renderer->glDisableVertexAttribArray(0);
	m_Renderer->glDisableVertexAttribArray(1);

	// Release the vertex buffer.
	m_Renderer->glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_Renderer->glDeleteBuffers(1, &m_vertexBufferId);

	// Release the vertex array object.
	m_Renderer->glBindVertexArray(0);
	m_Renderer->glDeleteVertexArrays(1, &m_vertexArrayId);

	return;
}

void Entity_PointCloud::_renderBuffers()
{
	// Bind the vertex array object that stored all the information about the vertex buffer.
	m_Renderer->glBindVertexArray(m_vertexArrayId);

	// Render the vertex buffer using the index buffer.
	glDrawArrays(GL_POINTS, 0, m_vertexCount);

	return;
}

void Entity_PointCloud::SetPosition(float _x, float _y, float _z)
{
	m_positionX = _x;
	m_positionY = _y;
	m_positionZ = _z;
	return;
}


void Entity_PointCloud::SetRotation(float _x, float _y, float _z)
{
	m_rotationX = _x;
	m_rotationY = _y;
	m_rotationZ = _z;
	return;
}

void Entity_PointCloud::GetPosition(float* _result)
{
	_result[0] = m_positionX;
	_result[1] = m_positionY;
	_result[2] = m_positionZ;
	return;
}

void Entity_PointCloud::GetRotation(float* _result)
{
	_result[0] = m_rotationX;
	_result[1] = m_rotationY;
	_result[2] = m_rotationZ;
	return;
}

void Entity_PointCloud::GetWorldMatrix(float* _result)
{
	_result[0] = m_worldMatrix[0];
	_result[1] = m_worldMatrix[1];
	_result[2] = m_worldMatrix[2];
	_result[3] = m_worldMatrix[3];

	_result[4] = m_worldMatrix[4];
	_result[5] = m_worldMatrix[5];
	_result[6] = m_worldMatrix[6];
	_result[7] = m_worldMatrix[7];

	_result[8] = m_worldMatrix[8];
	_result[9] = m_worldMatrix[9];
	_result[10] = m_worldMatrix[10];
	_result[11] = m_worldMatrix[11];

	_result[12] = m_worldMatrix[12];
	_result[13] = m_worldMatrix[13];
	_result[14] = m_worldMatrix[14];
	_result[15] = m_worldMatrix[15];

	return;
}