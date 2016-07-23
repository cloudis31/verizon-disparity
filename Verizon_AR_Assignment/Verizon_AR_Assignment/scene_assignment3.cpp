#include "scene_assignment3.h"
#include "ogl.h"
#include "camera.h"
#include "shaders.h"
#include <opencv2\highgui\highgui.hpp>
#include "disparitymapper.h"

Scene_Assignment3::Scene_Assignment3()
{
	m_Renderer = 0;
	m_Camera = 0;
	m_VisibleEntity = 0;
}

bool Scene_Assignment3::Initialize(OpenGLRenderer* _renderer, HWND _hwnd)
{
	bool result;
	// Store a pointer to the OpenGL class object.
	m_Renderer = _renderer;

	// Create the camera object.
	m_Camera = new Camera;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// load in the sample images
	cv::Mat left_img = cv::imread("home_left.jpg");
	cv::Mat right_img = cv::imread("home_right.jpg");

	int numDisparity = 16 * 8;   // Range of disparity

	int SADWindowSize = 7; // Size of the block window. Must be odd

	// create and set up a disparity mapper
	DisparityMapper mapper = DisparityMapper(left_img, right_img, numDisparity, SADWindowSize, true, DISPARITY_MAPPER_QUALITY::DISPARITY_MAPPER_QUALITY_QUALITY);
	mapper.SetCalibrationImageFilename("CalibrationImages/images.xml");
	mapper.SetP1(8 * 3 * SADWindowSize * SADWindowSize);
	mapper.SetP2(32 * 3 * SADWindowSize * SADWindowSize);
	mapper.SetMode(cv::StereoSGBM::MODE_SGBM_3WAY);
	mapper.SetUseConfidence(true);

	// compute the disparity map and point cloud
	mapper.Compute();

	double focalLength = mapper.GetFocalLength();
	double baseline = mapper.GetBaseline();

	// get point cloud and cropped left color image
	cv::Mat pointcloud = mapper.GetPointCloud();
	cv::Mat color_left = mapper.GetCroppedLeftOriginal();

	float whscale = 8.0f; // width/height scale
	float xscale = whscale / pointcloud.cols; // downscale image
	float yscale = whscale / pointcloud.rows; // downscale image
	float cscale = 1.0f / 255.0f; // convert from 0-255 to 0-1
	float dscale = whscale / ((focalLength*baseline) / numDisparity);
	float doffset = -6.0f;
	// create point array from 3d image
	ColorShader::VertexType* points = new ColorShader::VertexType[pointcloud.rows * pointcloud.cols];
	int totalverts = 0;
	for (int y = 0; y < pointcloud.rows; ++y)
	{
		for (int x = 0; x < pointcloud.cols; ++x)
		{
			// get point position
			cv::Vec3f pos = pointcloud.at<cv::Vec3f>(y, x);

			// skip this point if depth is unknown (+-infinity)
			if (pos[2] == -INFINITY || pos[2] == INFINITY)
			{
				continue;
			}

			// get point color
			cv::Vec3b color = color_left.at<cv::Vec3b>(color_left.rows - y - 1, x);

			// convert colors from 0-255 to 0-1
			float r = color[2] * cscale;
			float g = color[1] * cscale;
			float b = color[0] * cscale;

			float posx = (pos[0] * xscale); // scale and center width
			float posy = (pos[1] * yscale); // scale and center height
			float posz = (pos[2] * dscale) + doffset; // scale depth

													  // store point
			points[totalverts++] = { posx, posy, posz, r, g, b };
		}
	}

	// Create the full screen quad entity for left image
	_createFullScreenQuad(left_img, _hwnd);

	// Create the full screen quad entity for right image
	_createFullScreenQuad(right_img, _hwnd);

	// Create the full screen quad entity for disparity image
	cv::Mat disparityRGBA;
	cv::cvtColor(mapper.GetCroppedDisparity(), disparityRGBA, CV_GRAY2RGBA, 4);
	_createFullScreenQuad(disparityRGBA, _hwnd);

	// create the point cloud entity to show the disparity map in 3d
	_createPointCloud(points, totalverts, _hwnd);

	// free everything not needed anymore
	delete points;

	return true;
}


bool Scene_Assignment3::_createFullScreenQuad(cv::Mat _image, HWND _hwnd)
{
	Entity_FullscreenQuad* fsq = new Entity_FullscreenQuad();
	if (!fsq)
	{
		return false;
	}

	// Initialize the model object.
	bool result = fsq->Initialize(m_Renderer);
	if (!result)
	{
		MessageBox(_hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	result = fsq->LoadTextureFromMat(_image, 0, true);
	if (!result)
	{
		MessageBox(_hwnd, L"Could not load the model's texture.", L"Error", MB_OK);
		return false;
	}

	m_Entities.push_back(fsq);
	m_Renderer->InitializeShader(_hwnd, fsq->GetShaderID());

	return true;
}
bool Scene_Assignment3::_createPointCloud(ColorShader::VertexType* _vertices, int _numVerts, HWND _hwnd)
{
	Entity_PointCloud* pc = new Entity_PointCloud();
	if (!pc)
	{
		return false;
	}

	pc->Initialize(m_Renderer, _vertices, _numVerts);
	m_Entities.push_back(pc);
	m_Renderer->InitializeShader(_hwnd, pc->GetShaderID());

	return true;
}

void Scene_Assignment3::Reset()
{
	m_Entities[m_VisibleEntity]->Reset();
}


void Scene_Assignment3::Shutdown()
{
	// Release the model object.
	for (auto entity : m_Entities)
	{
		if (entity)
		{
			entity->Shutdown();
			delete entity;
			entity = 0;
		}
	}
	m_Entities.clear();

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the pointer to the OpenGL class object.
	m_Renderer = 0;

	return;
}


bool Scene_Assignment3::Update()
{
	if (m_VisibleEntity == 3)
	{
		Entity_PointCloud* entity = (Entity_PointCloud*)m_Entities[m_VisibleEntity];
		float rot[3];
		entity->GetRotation(rot);
		entity->SetRotation(-0.1, rot[1] + 0.0008f, rot[2]);
		//entity->SetRotation(rot[0], rot[1] - 0.0008f, rot[2]);
	}
	m_Entities[m_VisibleEntity]->Update();
	return true;
}

bool Scene_Assignment3::Render()
{
	// Clear the buffers to begin the scene.
	m_Renderer->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	float viewMatrix[16];
	m_Camera->GetViewMatrix(viewMatrix);

	// Render the visible entity using the color shader.
	m_Entities[m_VisibleEntity]->Render(viewMatrix);

	// Present the rendered scene to the screen.
	m_Renderer->EndScene();

	return true;
}

void Scene_Assignment3::Action()
{
	m_VisibleEntity++;
	if (m_VisibleEntity >= m_Entities.size())
		m_VisibleEntity = 0;

	m_Entities[m_VisibleEntity]->Reset();
}