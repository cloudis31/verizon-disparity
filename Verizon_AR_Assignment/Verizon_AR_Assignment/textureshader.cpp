#include "textureshader.h"
#include <fstream>

bool TextureShader::Initialize(OpenGLRenderer* _renderer, HWND _hwnd)
{
	bool result;
	m_Renderer = _renderer;

	// Initialize the vertex and pixel shaders.
	result = _initializeShader("vs_texture.glsl", "ps_texture.glsl", _hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void TextureShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	_shutdownShader();

	return;
}


void TextureShader::SetShader()
{
	// Install the shader program as part of the current rendering state.
	m_Renderer->glUseProgram(m_shaderProgram);

	return;
}


bool TextureShader::_initializeShader(char* _vsFilename, char* _psFilename, HWND _hwnd)
{
	const char* vertexShaderBuffer;
	const char* fragmentShaderBuffer;
	int status;


	// Load the vertex shader source file into a text buffer.
	vertexShaderBuffer = _loadShaderSourceFile(_vsFilename);
	if (!vertexShaderBuffer)
	{
		return false;
	}

	// Load the fragment shader source file into a text buffer.
	fragmentShaderBuffer = _loadShaderSourceFile(_psFilename);
	if (!fragmentShaderBuffer)
	{
		return false;
	}

	// Create a vertex and fragment shader object.
	m_vertexShader = m_Renderer->glCreateShader(GL_VERTEX_SHADER);
	m_pixelShader = m_Renderer->glCreateShader(GL_FRAGMENT_SHADER);

	// Copy the shader source code strings into the vertex and fragment shader objects.
	m_Renderer->glShaderSource(m_vertexShader, 1, &vertexShaderBuffer, NULL);
	m_Renderer->glShaderSource(m_pixelShader, 1, &fragmentShaderBuffer, NULL);

	// Release the vertex and fragment shader buffers.
	delete[] vertexShaderBuffer;
	vertexShaderBuffer = 0;

	delete[] fragmentShaderBuffer;
	fragmentShaderBuffer = 0;

	// Compile the shaders.
	m_Renderer->glCompileShader(m_vertexShader);
	m_Renderer->glCompileShader(m_pixelShader);

	// Check to see if the vertex shader compiled successfully.
	m_Renderer->glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &status);
	if (status != 1)
	{
		// If it did not compile then write the syntax error message out to a text file for review.
		_outputShaderErrorMessage(_hwnd, m_vertexShader, _vsFilename);
		return false;
	}

	// Check to see if the fragment shader compiled successfully.
	m_Renderer->glGetShaderiv(m_pixelShader, GL_COMPILE_STATUS, &status);
	if (status != 1)
	{
		// If it did not compile then write the syntax error message out to a text file for review.
		_outputShaderErrorMessage(_hwnd, m_pixelShader, _psFilename);
		return false;
	}

	// Create a shader program object.
	m_shaderProgram = m_Renderer->glCreateProgram();

	// Attach the vertex and fragment shader to the program object.
	m_Renderer->glAttachShader(m_shaderProgram, m_vertexShader);
	m_Renderer->glAttachShader(m_shaderProgram, m_pixelShader);

	// Bind the shader input variables.
	m_Renderer->glBindAttribLocation(m_shaderProgram, 0, "inputPosition");
	m_Renderer->glBindAttribLocation(m_shaderProgram, 1, "inputTexCoord");

	// Link the shader program.
	m_Renderer->glLinkProgram(m_shaderProgram);

	// Check the status of the link.
	m_Renderer->glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &status);
	if (status != 1)
	{
		// If it did not link then write the syntax error message out to a text file for review.
		_outputLinkerErrorMessage(_hwnd, m_shaderProgram);
		return false;
	}

	return true;
}


char* TextureShader::_loadShaderSourceFile(char* _filename)
{
	ifstream fin;
	int fileSize;
	char input;
	char* buffer;


	// Open the shader source file.
	fin.open(_filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		return 0;
	}

	// Initialize the size of the file.
	fileSize = 0;

	// Read the first element of the file.
	fin.get(input);

	// Count the number of elements in the text file.
	while (!fin.eof())
	{
		fileSize++;
		fin.get(input);
	}

	// Close the file for now.
	fin.close();

	// Initialize the buffer to read the shader source file into.
	buffer = new char[fileSize + 1];
	if (!buffer)
	{
		return 0;
	}

	// Open the shader source file again.
	fin.open(_filename);

	// Read the shader text file into the buffer as a block.
	fin.read(buffer, fileSize);

	// Close the file.
	fin.close();

	// Null terminate the buffer.
	buffer[fileSize] = '\0';

	return buffer;
}


void TextureShader::_outputShaderErrorMessage(HWND _hwnd, unsigned int _shaderId, char* _shaderFilename)
{
	int logSize, i;
	char* infoLog;
	ofstream fout;
	wchar_t newString[128];
	unsigned int error, convertedChars;


	// Get the size of the string containing the information log for the failed shader compilation message.
	m_Renderer->glGetShaderiv(_shaderId, GL_INFO_LOG_LENGTH, &logSize);

	// Increment the size by one to handle also the null terminator.
	logSize++;

	// Create a char buffer to hold the info log.
	infoLog = new char[logSize];
	if (!infoLog)
	{
		return;
	}

	// Now retrieve the info log.
	m_Renderer->glGetShaderInfoLog(_shaderId, logSize, NULL, infoLog);

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<logSize; i++)
	{
		fout << infoLog[i];
	}

	// Close the file.
	fout.close();

	// Convert the shader filename to a wide character string.
	error = mbstowcs_s(&convertedChars, newString, 128, _shaderFilename, 128);
	if (error != 0)
	{
		return;
	}

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(_hwnd, L"Error compiling shader.  Check shader-error.txt for message.", newString, MB_OK);

	return;
}


void TextureShader::_outputLinkerErrorMessage(HWND _hwnd, unsigned int _programId)
{
	int logSize, i;
	char* infoLog;
	ofstream fout;


	// Get the size of the string containing the information log for the failed shader compilation message.
	m_Renderer->glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &logSize);

	// Increment the size by one to handle also the null terminator.
	logSize++;

	// Create a char buffer to hold the info log.
	infoLog = new char[logSize];
	if (!infoLog)
	{
		return;
	}

	// Now retrieve the info log.
	m_Renderer->glGetProgramInfoLog(_programId, logSize, NULL, infoLog);

	// Open a file to write the error message to.
	fout.open("linker-error.txt");

	// Write out the error message.
	for (i = 0; i<logSize; i++)
	{
		fout << infoLog[i];
	}

	// Close the file.
	fout.close();

	// Pop a message up on the screen to notify the user to check the text file for linker errors.
	MessageBox(_hwnd, L"Error compiling linker.  Check linker-error.txt for message.", L"Linker Error", MB_OK);

	return;
}


void TextureShader::_shutdownShader()
{
	// Detach the vertex and fragment shaders from the program.
	m_Renderer->glDetachShader(m_shaderProgram, m_vertexShader);
	m_Renderer->glDetachShader(m_shaderProgram, m_pixelShader);

	// Delete the vertex and fragment shaders.
	m_Renderer->glDeleteShader(m_vertexShader);
	m_Renderer->glDeleteShader(m_pixelShader);

	// Delete the shader program.
	m_Renderer->glDeleteProgram(m_shaderProgram);

	return;
}

bool TextureShader::SetShaderParameters(float* _worldMatrix, float* _viewMatrix, float* _projectionMatrix, int _textureUnit)
{
	unsigned int location;


	// Set the world matrix in the vertex shader.
	location = m_Renderer->glGetUniformLocation(m_shaderProgram, "worldMatrix");
	if (location == -1)
	{
		return false;
	}
	m_Renderer->glUniformMatrix4fv(location, 1, false, _worldMatrix);

	// Set the view matrix in the vertex shader.
	location = m_Renderer->glGetUniformLocation(m_shaderProgram, "viewMatrix");
	if (location == -1)
	{
		return false;
	}
	m_Renderer->glUniformMatrix4fv(location, 1, false, _viewMatrix);

	// Set the projection matrix in the vertex shader.
	location = m_Renderer->glGetUniformLocation(m_shaderProgram, "projectionMatrix");
	if (location == -1)
	{
		return false;
	}
	m_Renderer->glUniformMatrix4fv(location, 1, false, _projectionMatrix);

	// Set the texture in the pixel shader to use the data from the first texture unit.
	location = m_Renderer->glGetUniformLocation(m_shaderProgram, "shaderTexture");
	if (location == -1)
	{
		return false;
	}
	m_Renderer->glUniform1i(location, _textureUnit);

	return true;
}