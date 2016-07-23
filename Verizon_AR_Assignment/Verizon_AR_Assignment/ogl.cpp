#include "ogl.h"
#include "interfaces.h"
#include "shaders.h"

bool OpenGLRenderer::Initialize(HWND _hwnd, int _screenWidth, int _screenHeight, float _screenDepth, float _screenNear, bool _vsync)
{	
	int pixelFormat[1];
	unsigned int formatCount;
	int result;
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	int attributeList[5];
	float fieldOfView, screenAspect;
	char *vendorString, *rendererString;

	// Get the device context for this window.
	m_deviceContext = GetDC(_hwnd);
	if (!m_deviceContext)
	{
		return false;
	}

	// Support for OpenGL rendering.
	int attributeListInt[] = {
		WGL_SUPPORT_OPENGL_ARB, TRUE, // Support for OpenGL rendering.
		WGL_DRAW_TO_WINDOW_ARB, TRUE, // Support for rendering to a window.
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, // Support for hardware acceleration.
		WGL_COLOR_BITS_ARB, 32, // Support for 32bit color.
		WGL_DEPTH_BITS_ARB, 24, // Support for 24 bit depth buffer.
		WGL_STENCIL_BITS_ARB, 8, // Support for a 8 bit stencil buffer.
		WGL_DOUBLE_BUFFER_ARB, TRUE, // Support for double buffer.
		WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB, // Support for swapping front and back buffer.
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, // Support for the RGBA pixel type.
		0 }; // Null terminate the attribute list.
	
	// Query for a pixel format that fits the attributes we want.
	result = wglChoosePixelFormatARB(m_deviceContext, attributeListInt, NULL, 1, pixelFormat, &formatCount);
	if (result != 1)
	{
		return false;
	}

	// If the video card/display can handle our desired pixel format then we set it as the current one.
	result = SetPixelFormat(m_deviceContext, pixelFormat[0], &pixelFormatDescriptor);
	if (result != 1)
	{
		return false;
	}

	// Set the 4.0 version of OpenGL in the attribute list.
	attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
	attributeList[1] = 4;
	attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
	attributeList[3] = 0;

	// Null terminate the attribute list.
	attributeList[4] = 0;

	// Create a OpenGL 4.0 rendering context.
	m_renderingContext = wglCreateContextAttribsARB(m_deviceContext, 0, attributeList);
	if (m_renderingContext == NULL)
	{
		return false;
	}

	// Set the rendering context to active.
	result = wglMakeCurrent(m_deviceContext, m_renderingContext);
	if (result != 1)
	{
		return false;
	}

	// Set the depth buffer to be entirely cleared to 1.0 values.
	glClearDepth(1.0f);

	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GEQUAL);
	glClearDepth(0.0f);

	// Set the polygon winding to front facing for the left handed system.
	glFrontFace(GL_CW);

	// Enable back face culling.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Set the field of view and screen aspect ratio.
	fieldOfView = PI / 4.0f;
	screenAspect = (float)_screenWidth / (float)_screenHeight;

	// Build the perspective projection matrix.
	Math::BuildPerspectiveFovLHMatrix(m_projectionMatrix, fieldOfView, screenAspect, _screenNear, _screenDepth);

	// Get the name of the video card.
	vendorString = (char*)glGetString(GL_VENDOR);
	rendererString = (char*)glGetString(GL_RENDERER);

	// Store the video card name in a class member variable so it can be retrieved later.
	strcpy_s(m_videoCardDescription, vendorString);
	strcat_s(m_videoCardDescription, " - ");
	strcat_s(m_videoCardDescription, rendererString);

	// Turn on or off the vertical sync depending on the input bool value.
	if (_vsync)
	{
		result = wglSwapIntervalEXT(1);
	}
	else
	{
		result = wglSwapIntervalEXT(0);
	}

	// Check if vsync was set correctly.
	if (result != 1)
	{
		return false;
	}

	return true;
}

void OpenGLRenderer::Shutdown(HWND _hwnd)
{
	// Release the shader objects.
	for (auto shader : m_Shaders)
	{
		if (shader.second)
		{
			shader.second->Shutdown();
			delete shader.second;
			shader.second = 0;
		}
	}
	m_Shaders.clear();

	// Release the rendering context.
	if (m_renderingContext)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_renderingContext);
		m_renderingContext = 0;
	}

	// Release the device context.
	if (m_deviceContext)
	{
		ReleaseDC(_hwnd, m_deviceContext);
		m_deviceContext = 0;
	}

	return;
}

bool OpenGLRenderer::_addShader(int _id, Shader* _shader)
{
	// insert if it doesn't exist
	if (m_Shaders.find(_id) == m_Shaders.end())
	{
		m_Shaders.insert(std::make_pair(_id, _shader));
		return true;
	}
	return false;
}

void OpenGLRenderer::SetShader(int _id)
{
	// assume the shader has already been initialized
	m_Shaders[_id]->SetShader();
}

bool OpenGLRenderer::InitializeShader(HWND _hwnd, int _shaderid)
{
	if (!ShaderExists(_shaderid))
	{
		bool result;

		switch (_shaderid)
		{
			// texture shader
			case TextureShader::SHADER_ID:
			{
				TextureShader* shader = new TextureShader();
				if (!shader)
				{
					return false;
				}

				result = _addShader(TextureShader::SHADER_ID, shader);
				if (!result)
				{
					shader->Shutdown();
					delete shader;
					return false;
				}

				// Initialize the color shader object.
				result = shader->Initialize(this, _hwnd);
				if (!result)
				{
					MessageBox(_hwnd, L"Could not initialize the shader object.", L"Error", MB_OK);
					return false;
				}
				return true;
			}

			// color shader
			case ColorShader::SHADER_ID:
			{
				ColorShader* shader = new ColorShader();
				if (!shader)
				{
					return false;
				}

				result = _addShader(ColorShader::SHADER_ID, shader);
				if (!result)
				{
					shader->Shutdown();
					delete shader;
					return false;
				}

				// Initialize the color shader object.
				result = shader->Initialize(this, _hwnd);
				if (!result)
				{
					MessageBox(_hwnd, L"Could not initialize the shader object.", L"Error", MB_OK);
					return false;
				}
				return true;
			}
			default:
				return false;
		}
	}
	return true;
}

void OpenGLRenderer::BeginScene(float _r, float _g, float _b, float _a)
{
	// Set the color to clear the screen to.
	glClearColor(_r, _g, _b, _a);

	// Clear the screen and depth buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return;
}


void OpenGLRenderer::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	SwapBuffers(m_deviceContext);

	return;
}

bool OpenGLRenderer::InitializeExtensionFunctions(HWND _hwnd)
{
	HDC deviceContext;
	PIXELFORMATDESCRIPTOR pixelFormat;
	int error;
	HGLRC renderContext;
	bool result;

	// Get the device context for this window.
	deviceContext = GetDC(_hwnd);
	if (!deviceContext)
	{
		return false;
	}

	// Set a temporary default pixel format.
	error = SetPixelFormat(deviceContext, 1, &pixelFormat);
	if (error != 1)
	{
		return false;
	}

	// Create a temporary rendering context.
	renderContext = wglCreateContext(deviceContext);
	if (!renderContext)
	{
		return false;
	}

	// Set the temporary rendering context as the current rendering context for this window.
	error = wglMakeCurrent(deviceContext, renderContext);
	if (error != 1)
	{
		return false;
	}

	// Initialize the OpenGL extensions needed for this application.  Note that a temporary rendering context was needed to do so.
	result = LoadExtensionList();
	if (!result)
	{
		return false;
	}

	// Release the temporary rendering context now that the extensions have been loaded.
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(renderContext);
	renderContext = NULL;

	// Release the device context for this window.
	ReleaseDC(_hwnd, deviceContext);
	deviceContext = 0;

	return true;
}

bool OpenGLRenderer::LoadExtensionList()
{
	// Load the OpenGL extensions that this application will be using.
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	if (!wglChoosePixelFormatARB)
	{
		return false;
	}

	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (!wglCreateContextAttribsARB)
	{
		return false;
	}

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if (!wglSwapIntervalEXT)
	{
		return false;
	}

	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	if (!glAttachShader)
	{
		return false;
	}

	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	if (!glBindBuffer)
	{
		return false;
	}

	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
	if (!glBindVertexArray)
	{
		return false;
	}

	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	if (!glBufferData)
	{
		return false;
	}

	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	if (!glCompileShader)
	{
		return false;
	}

	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	if (!glCreateProgram)
	{
		return false;
	}

	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	if (!glCreateShader)
	{
		return false;
	}

	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	if (!glDeleteBuffers)
	{
		return false;
	}

	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	if (!glDeleteProgram)
	{
		return false;
	}

	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	if (!glDeleteShader)
	{
		return false;
	}

	glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
	if (!glDeleteVertexArrays)
	{
		return false;
	}

	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	if (!glDetachShader)
	{
		return false;
	}

	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	if (!glEnableVertexAttribArray)
	{
		return false;
	}

	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	if (!glGenBuffers)
	{
		return false;
	}

	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	if (!glGenVertexArrays)
	{
		return false;
	}

	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	if (!glGetAttribLocation)
	{
		return false;
	}

	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
	if (!glGetProgramInfoLog)
	{
		return false;
	}

	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	if (!glGetProgramiv)
	{
		return false;
	}

	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	if (!glGetShaderInfoLog)
	{
		return false;
	}

	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	if (!glGetShaderiv)
	{
		return false;
	}

	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	if (!glLinkProgram)
	{
		return false;
	}

	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	if (!glShaderSource)
	{
		return false;
	}

	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	if (!glUseProgram)
	{
		return false;
	}

	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	if (!glVertexAttribPointer)
	{
		return false;
	}

	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	if (!glBindAttribLocation)
	{
		return false;
	}

	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	if (!glGetUniformLocation)
	{
		return false;
	}

	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	if (!glUniformMatrix4fv)
	{
		return false;
	}

	glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	if (!glActiveTexture)
	{
		return false;
	}

	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	if (!glUniform1i)
	{
		return false;
	}

	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
	if (!glGenerateMipmap)
	{
		return false;
	}

	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	if (!glDisableVertexAttribArray)
	{
		return false;
	}

	glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	if (!glUniform3fv)
	{
		return false;
	}

	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	if (!glUniform4fv)
	{
		return false;
	}

	glMapBuffer = (PFNGLMAPBUFFERPROC)wglGetProcAddress("glMapBuffer");
	if (!glMapBuffer)
	{
		return false;
	}

	glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBuffer");
	if (!glUnmapBuffer)
	{
		return false;
	}


	return true;
}

void OpenGLRenderer::GetProjectionMatrix(float* _result)
{
	_result[0] = m_projectionMatrix[0];
	_result[1] = m_projectionMatrix[1];
	_result[2] = m_projectionMatrix[2];
	_result[3] = m_projectionMatrix[3];

	_result[4] = m_projectionMatrix[4];
	_result[5] = m_projectionMatrix[5];
	_result[6] = m_projectionMatrix[6];
	_result[7] = m_projectionMatrix[7];

	_result[8] = m_projectionMatrix[8];
	_result[9] = m_projectionMatrix[9];
	_result[10] = m_projectionMatrix[10];
	_result[11] = m_projectionMatrix[11];

	_result[12] = m_projectionMatrix[12];
	_result[13] = m_projectionMatrix[13];
	_result[14] = m_projectionMatrix[14];
	_result[15] = m_projectionMatrix[15];

	return;
}

void OpenGLRenderer::GetVideoCardInfo(char* _cardName)
{
	strcpy_s(_cardName, 128, m_videoCardDescription);
	return;
}

