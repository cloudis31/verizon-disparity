#include "ogl.h"
#include "entity_fullscreenquad.h"
#include "texture.h"

Entity_FullscreenQuad::Entity_FullscreenQuad()
{
	m_Texture = 0;
}

bool Entity_FullscreenQuad::Initialize(OpenGLRenderer* _renderer)
{
	bool result;

	m_Renderer = _renderer;

	// Initialize the world/model matrix to the identity matrix.
	Math::BuildIdentityMatrix(m_worldMatrix);

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = _initializeBuffers();
	if (!result)
	{
		return false;
	}

	return true;
}

void Entity_FullscreenQuad::Shutdown()
{
	// Release the texture used for this model.
	_releaseTexture();

	// Release the vertex and index buffers.
	_shutdownBuffers();

	return;
}

void Entity_FullscreenQuad::Update()
{

}
void Entity_FullscreenQuad::Reset()
{
	// Get the world, view, and projection matrices from the opengl and camera objects.
	float identityMat[16];
	Math::BuildIdentityMatrix(identityMat);

	float projectionMatrix[16];
	m_Renderer->GetProjectionMatrix(projectionMatrix);

	float rotationMatrix[16];
	Math::MatrixRotationZ(rotationMatrix, m_rotationZ);

	float translationMatrix[16];
	Math::MatrixTranslation(translationMatrix, m_positionX, m_positionY, m_positionZ);

	Math::MatrixMultiply(m_worldMatrix, rotationMatrix, translationMatrix);
}

void Entity_FullscreenQuad::Render(float* _viewMatrix)
{
	float identityMat[16];
	Math::BuildIdentityMatrix(identityMat);

	// set the shader stuff
	m_Renderer->SetShader(GetShaderID());

	static_cast<TextureShader*>(m_Renderer->GetShader(GetShaderID()))->SetShaderParameters(m_worldMatrix, identityMat, identityMat, 0);

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_renderBuffers();

	return;
}

bool Entity_FullscreenQuad::_initializeBuffers()
{
	// Create the vertex array.
	TextureShader::VertexType vertices[] = {
		{ -1.0f, -1.0f, 0.0f, 0.0f, 0.0f },
		{ -1.0f,  1.0f, 0.0f, 0.0f, 1.0f },
		{  1.0f,  1.0f, 0.0f, 1.0f, 1.0f },
		{  1.0f, -1.0f, 0.0f, 1.0f, 0.0f }
	};
	m_vertexCount = 4;

	// Create the index array.
	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3
	};
	m_indexCount = 6;

	// Allocate an OpenGL vertex array object.
	m_Renderer->glGenVertexArrays(1, &m_vertexArrayId);

	// Bind the vertex array object to store all the buffers and vertex attributes we create here.
	m_Renderer->glBindVertexArray(m_vertexArrayId);

	// Generate an ID for the vertex buffer.
	m_Renderer->glGenBuffers(1, &m_vertexBufferId);

	// Bind the vertex buffer and load the vertex (position and color) data into the vertex buffer.
	m_Renderer->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	m_Renderer->glBufferData(GL_ARRAY_BUFFER, m_vertexCount * sizeof(TextureShader::VertexType), vertices, GL_STATIC_DRAW);

	// Enable the two vertex array attributes.
	m_Renderer->glEnableVertexAttribArray(0);  // Vertex position.
	m_Renderer->glEnableVertexAttribArray(1);  // Vertex texture coordinate.

	// Specify the location and format of the position portion of the vertex buffer.
	m_Renderer->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	m_Renderer->glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(TextureShader::VertexType), 0);

	// Specify the location and format of the texture coordinate portion of the vertex buffer.
	m_Renderer->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	m_Renderer->glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(TextureShader::VertexType), (unsigned char*)(3 * sizeof(float)));

	// Generate an ID for the index buffer.
	m_Renderer->glGenBuffers(1, &m_indexBufferId);

	// Bind the index buffer and load the index data into it.
	m_Renderer->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
	m_Renderer->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	
	return true;
}
void Entity_FullscreenQuad::_shutdownBuffers()
{
	// Disable the two vertex array attributes.
	m_Renderer->glDisableVertexAttribArray(0);
	m_Renderer->glDisableVertexAttribArray(1);

	// Release the vertex buffer.
	m_Renderer->glBindBuffer(GL_ARRAY_BUFFER, 0);
	m_Renderer->glDeleteBuffers(1, &m_vertexBufferId);

	// Release the index buffer.
	m_Renderer->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	m_Renderer->glDeleteBuffers(1, &m_indexBufferId);

	// Release the vertex array object.
	m_Renderer->glBindVertexArray(0);
	m_Renderer->glDeleteVertexArrays(1, &m_vertexArrayId);

	return;
}

void Entity_FullscreenQuad::_renderBuffers()
{
	m_Texture->Set(m_Renderer, 0);

	// Bind the vertex array object that stored all the information about the vertex and index buffers.
	m_Renderer->glBindVertexArray(m_vertexArrayId);

	// Render the vertex buffer using the index buffer.
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	return;
}

bool Entity_FullscreenQuad::UpdateTexture(char* _data, int _width, int _height, int _textureUnit)
{
	return m_Texture->UpdateTexture(m_Renderer, _data, _width, _height, _textureUnit);
}
bool Entity_FullscreenQuad::LoadTextureFromMat(cv::Mat _image, unsigned int _textureUnit, bool _wrap)
{
	bool result;

	// Create the texture object.
	m_Texture = new Texture();
	if (!m_Texture)
	{
		return false;
	}

	cv::Mat tmpImage;

	switch (_image.channels())
	{
		case 1:
			cv::cvtColor(_image, tmpImage, CV_GRAY2RGBA, 4);
			break;
		case 3:
			cv::cvtColor(_image, tmpImage, CV_RGB2RGBA, 4);
			break;
		default:
			tmpImage = _image;
			break;
	}

	cv::flip(tmpImage, tmpImage, 0);

	// Initialize the texture object.
	result = m_Texture->InitializeFromMemory(m_Renderer, (char*)tmpImage.data, _image.cols, _image.rows, 32, _textureUnit, _wrap);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Entity_FullscreenQuad::LoadTextureFromMemory(char* _data, int _width, int _height, unsigned int _textureUnit, bool _wrap)
{
	bool result;

	// Create the texture object.
	m_Texture = new Texture();
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->InitializeFromMemory(m_Renderer, _data, _width, _height, 32, _textureUnit, _wrap);
	if (!result)
	{
		return false;
	}

	return true;
}

void Entity_FullscreenQuad::_releaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

void Entity_FullscreenQuad::SetPosition(float _x, float _y, float _z)
{
	m_positionX = _x;
	m_positionY = _y;
	m_positionZ = _z;
	return;
}


void Entity_FullscreenQuad::SetRotation(float _x, float _y, float _z)
{
	m_rotationX = _x;
	m_rotationY = _y;
	m_rotationZ = _z;
	return;
}

void Entity_FullscreenQuad::GetWorldMatrix(float* _result)
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