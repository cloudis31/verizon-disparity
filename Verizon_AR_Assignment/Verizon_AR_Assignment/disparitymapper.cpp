#include "disparitymapper.h"
#include <fstream>
#include <iostream>

DisparityMapper::DisparityMapper(cv::Mat _left, cv::Mat _right, int _numDisparities, int _wsize, bool _rectify, DISPARITY_MAPPER_QUALITY _quality)
	: m_LeftOriginal(_left), m_RightOriginal(_right), m_NumDisparities(_numDisparities), m_SADWindowSize(_wsize), m_RectifyImages(_rectify), m_Quality(_quality)
{
	m_UniquenessRatio = 0;
	m_MinDisparity = 0;
	m_P1 = 0;
	m_P2 = 0;
	m_Disp12MaxDiff = 1000000;
	m_SpeckleWindowSize = 0;
	m_Mode = cv::StereoSGBM::MODE_HH;
	m_LambdaValue = 8000.0;
	m_SigmaColor = 1.5;
	m_UseConfidence = false;
	m_Downscale = false;
	m_QMatSet = false;
	m_CalibrationImagesFilename = NULL;

	m_CalibrationBoardSize = cv::Size(4, 11);
	m_CalibrationSquareSize = 12;
	m_FocalLength = 0.0;
	m_Baseline = 0.0;
}

void DisparityMapper::Compute()
{
	if (!m_RectifyImages && !m_QMatSet)
	{
		throw "Must provide a Q Matrix for already rectified images";
	}

	if (m_RectifyImages)
	{
		_calibrateCamera();
		_rectifyImages();
	}

	// Use Semi-Global Block Matching Stereo Correspondence algorithm, slower than BM but better quality
	if (m_Quality == DISPARITY_MAPPER_QUALITY::DISPARITY_MAPPER_QUALITY_QUALITY)
	{
		_computeQuality();
	}

	// use Block Matching Stereo Correspondence algorithm, faster than SGBM
	else if (m_Quality == DISPARITY_MAPPER_QUALITY::DISPARITY_MAPPER_QUALITY_FAST)
	{
		_computeFast();
	}

	// do not filter disparity map, take only left disparity
	else
	{
		_computeVeryFast();
	}

	_createPointCloud();
}
void DisparityMapper::_computeQuality()
{
	cv::Mat left_grey, right_grey;
	cv::Mat left_disp, right_disp, filtered_disp; // 16S

	// get greyscale images
	cv::cvtColor(m_LeftOriginal, left_grey, CV_BGR2GRAY);
	cv::cvtColor(m_RightOriginal, right_grey, CV_BGR2GRAY);

	// scale down the image
	if (m_Downscale)
	{
		cv::resize(left_grey, left_grey, cv::Size(), 0.5, 0.5);
		cv::resize(right_grey, right_grey, cv::Size(), 0.5, 0.5);
	}

	// compute left disparity map using stereo correspondence algorithm (Semi-Global Block Matching or SGBM algorithm)
	cv::Ptr<cv::StereoSGBM> left_sbm = cv::StereoSGBM::create(m_MinDisparity, m_NumDisparities, m_SADWindowSize);
	left_sbm->setUniquenessRatio(m_UniquenessRatio);
	left_sbm->setDisp12MaxDiff(m_Disp12MaxDiff);
	left_sbm->setSpeckleWindowSize(m_SpeckleWindowSize);
	left_sbm->setP1(m_P1);
	left_sbm->setP2(m_P2);
	left_sbm->setMode(m_Mode);
	m_LeftRegionOfInterest = _computeRegionOfInterest(m_LeftOriginal.size(), left_sbm);
	left_sbm->compute(left_grey, right_grey, left_disp);

	// compute right disparity map
	cv::Ptr<cv::StereoMatcher> right_sbm = cv::ximgproc::createRightMatcher(left_sbm);
	m_RightRegionOfInterest = _computeRegionOfInterest(m_RightOriginal.size(), right_sbm);
	right_sbm->compute(right_grey, left_grey, right_disp);

	// create disparity map filter based one Weighted Least Squares or WLS filter (in form of Fast Global Smoother)
	cv::Ptr<cv::ximgproc::DisparityWLSFilter> filter = cv::ximgproc::createDisparityWLSFilterGeneric(m_UseConfidence);
	filter->setDepthDiscontinuityRadius((int)ceil(0.5*m_SADWindowSize));
	filter->setLambda(m_LambdaValue);
	filter->setSigmaColor(m_SigmaColor);

	// compute filtered disparity map
	filter->filter(left_disp, left_grey, filtered_disp, right_disp);

	// convert filtered disparity map from 16 bit short to 8 bit unsigned char and normalize values
	double minVal, maxVal;
	cv::minMaxLoc(filtered_disp, &minVal, &maxVal);
	filtered_disp.convertTo(m_Disparity, CV_8UC1, 255 / (maxVal - minVal));
}
void DisparityMapper::_computeFast()
{
	cv::Mat left_grey, right_grey;
	cv::Mat left_disp, right_disp, filtered_disp; // 16S

	// get greyscale images
	cv::cvtColor(m_LeftOriginal, left_grey, CV_BGR2GRAY);
	cv::cvtColor(m_RightOriginal, right_grey, CV_BGR2GRAY);

	// scale down the image
	if (m_Downscale)
	{
		cv::resize(left_grey, left_grey, cv::Size(), 0.5, 0.5);
		cv::resize(right_grey, right_grey, cv::Size(), 0.5, 0.5);
	}

	// compute left disparity map using stereo correspondence algorithm (Block Matching or BM algorithm)
	cv::Ptr<cv::StereoBM> left_sbm = cv::StereoBM::create(m_NumDisparities, m_SADWindowSize);
	left_sbm->setMinDisparity(m_MinDisparity);
	left_sbm->setUniquenessRatio(m_UniquenessRatio);
	left_sbm->setDisp12MaxDiff(m_Disp12MaxDiff);
	left_sbm->setSpeckleWindowSize(m_SpeckleWindowSize);
	m_LeftRegionOfInterest = _computeRegionOfInterest(m_LeftOriginal.size(), left_sbm);
	left_sbm->compute(left_grey, right_grey, left_disp);

	// compute right disparity map
	cv::Ptr<cv::StereoMatcher> right_sbm = cv::ximgproc::createRightMatcher(left_sbm);
	m_RightRegionOfInterest = _computeRegionOfInterest(m_RightOriginal.size(), right_sbm);
	right_sbm->compute(right_grey, left_grey, right_disp);

	// create disparity map filter based one Weighted Least Squares or WLS filter (in form of Fast Global Smoother)
	cv::Ptr<cv::ximgproc::DisparityWLSFilter> filter = cv::ximgproc::createDisparityWLSFilterGeneric(m_UseConfidence);
	filter->setDepthDiscontinuityRadius((int)ceil(0.5*m_SADWindowSize));
	filter->setLambda(m_LambdaValue);
	filter->setSigmaColor(m_SigmaColor);

	// compute filtered disparity map
	filter->filter(left_disp, left_grey, filtered_disp, right_disp);

	// convert filtered disparity map from 16 bit short to 8 bit unsigned char and normalize values
	double minVal, maxVal;
	cv::minMaxLoc(filtered_disp, &minVal, &maxVal);
	filtered_disp.convertTo(m_Disparity, CV_8UC1, 255 / (maxVal - minVal));
}
void DisparityMapper::_computeVeryFast()
{
	cv::Mat left_grey, right_grey;
	cv::Mat left_disp; // 16 bit short

	// get greyscale images
	cv::cvtColor(m_LeftOriginal, left_grey, CV_BGR2GRAY);
	cv::cvtColor(m_RightOriginal, right_grey, CV_BGR2GRAY);

	// compute left disparity map using stereo correspondence algorithm (Block Matching or BM algorithm)
	cv::Ptr<cv::StereoBM> left_sbm = cv::StereoBM::create(m_NumDisparities, m_SADWindowSize);
	left_sbm->setUniquenessRatio(m_UniquenessRatio);
	left_sbm->setDisp12MaxDiff(m_Disp12MaxDiff);
	left_sbm->setSpeckleWindowSize(m_SpeckleWindowSize);
	m_LeftRegionOfInterest = _computeRegionOfInterest(m_LeftOriginal.size(), left_sbm);
	left_sbm->compute(left_grey, right_grey, left_disp);

	// convert disparity map from 16 bit short to 8 bit unsigned char and normalize values
	double minVal, maxVal;
	cv::minMaxLoc(left_disp, &minVal, &maxVal);
	left_disp.convertTo(m_Disparity, CV_8UC1, 255 / (maxVal - minVal));
}

void DisparityMapper::_createPointCloud()
{
	cv::Mat flippedDisp;
	cv::flip(m_Disparity(m_LeftRegionOfInterest), flippedDisp, 0);
	reprojectImageTo3D(flippedDisp, m_PointCloud, m_Q, false, CV_32F);
}

cv::Rect DisparityMapper::_computeRegionOfInterest(cv::Size2i _size, cv::Ptr<cv::StereoMatcher> _matcher)
{
	int min_disparity = _matcher->getMinDisparity();
	int num_disparities = _matcher->getNumDisparities();
	int block_size = _matcher->getBlockSize();

	int bs2 = block_size / 2;
	int minD = min_disparity, maxD = min_disparity + num_disparities - 1;

	int xmin = maxD + bs2;
	int xmax = _size.width + minD - bs2;
	int ymin = bs2;
	int ymax = _size.height - bs2;

	cv::Rect r(xmin, ymin, xmax - xmin, ymax - ymin);
	return r;
}


bool DisparityMapper::_getCalibrationImages()
{
	m_CalibrationImages.clear();
	cv::FileStorage fs(m_CalibrationImagesFilename, cv::FileStorage::READ);
	if (!fs.isOpened())
		return false;
	cv::FileNode n = fs.getFirstTopLevelNode();
	if (n.type() != cv::FileNode::SEQ)
		return false;
	for (auto it : n)
	{
		cv::Mat image = cv::imread(it, cv::IMREAD_COLOR);
		m_CalibrationImages.push_back(image);
	}
	return true;
}
void DisparityMapper::_rectifyImages()
{
	// get rectification (rotation), projection, and disparity to depth (Q) matrices
	cv::Mat R1, R2, P1, P2;
	cv::Rect validRoi[2];
	cv::Size imageSize = m_LeftOriginal.size();

	cv::stereoRectify(m_CameraMatrix[0], m_DistortionCoef[0],
		m_CameraMatrix[1], m_DistortionCoef[1],
		imageSize, m_StereoRotation, m_StereoTranslation, R1, R2, P1, P2, m_Q,
		0, 1, imageSize, &validRoi[0], &validRoi[1]);

	m_FocalLength = m_Q.at<double>(2, 3);
	m_Baseline = 1.0 / m_Q.at<double>(3, 2);

	m_QMatSet = true;

	// calibration wasn't perfected by the time this needed to be done, resulting rotation
	// matrices rotated too much so using identity matrices instead
	cv::Mat rmap[2][2];
	cv::initUndistortRectifyMap(m_CameraMatrix[0], m_DistortionCoef[0], cv::Mat(), P1, imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
	cv::initUndistortRectifyMap(m_CameraMatrix[1], m_DistortionCoef[1], cv::Mat(), P2, imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);

	cv::remap(m_LeftOriginal, m_LeftRectified, rmap[0][0], rmap[0][1], CV_INTER_LINEAR, cv::BORDER_CONSTANT);
	cv::remap(m_RightOriginal, m_RightRectified, rmap[1][0], rmap[1][1], CV_INTER_LINEAR, cv::BORDER_CONSTANT);
}

void DisparityMapper::_getCalibrationQuality()
{
	int numberOfImages = m_CalibrationImages.size()*0.5;
	double err = 0;
	int npoints = 0;
	std::vector<cv::Vec3f> lines[2];
	for (int i = 0; i < numberOfImages; i++)
	{
		int npt = (int)m_ImagePoints[0][i].size();
		cv::Mat imgpt[2];
		for (int k = 0; k < 2; k++)
		{
			imgpt[k] = cv::Mat(m_ImagePoints[k][i]);
			undistortPoints(imgpt[k], imgpt[k], m_CameraMatrix[k], m_DistortionCoef[k], cv::Mat(), m_CameraMatrix[k]);
			computeCorrespondEpilines(imgpt[k], k + 1, m_Fundamental, lines[k]);
		}
		for (int j = 0; j < npt; j++)
		{
			double errij = fabs(m_ImagePoints[0][i][j].x*lines[1][j][0] +
				m_ImagePoints[0][i][j].y*lines[1][j][1] + lines[1][j][2]) +
				fabs(m_ImagePoints[1][i][j].x*lines[0][j][0] +
					m_ImagePoints[1][i][j].y*lines[0][j][1] + lines[0][j][2]);
			err += errij;
		}
		npoints += npt;
	}
	double avg = err / npoints;
}

void DisparityMapper::_calibrateCamera()
{
	if (m_CalibrationImagesFilename == NULL)
	{
		throw "Please set calibration images filename";
	}

	// get all calibration images
	_getCalibrationImages();

	int numberOfImages = m_CalibrationImages.size()*0.5;

	cv::Size imageSize = m_CalibrationImages[0].size(); // all sizes should match first image size

	m_ImagePoints[0].resize(numberOfImages);
	m_ImagePoints[1].resize(numberOfImages);

	// find image points
	// for each image
	for (int image_idx = 0; image_idx < numberOfImages; ++image_idx)
	{
		// for each eye
		for (int eye_idx = 0; eye_idx < 2; ++eye_idx)
		{
			cv::Mat view = m_CalibrationImages[image_idx * 2 + eye_idx];

			std::vector<cv::Point2f>& corners = m_ImagePoints[eye_idx][image_idx];

			bool found = findCirclesGrid(view, m_CalibrationBoardSize, corners, cv::CALIB_CB_ASYMMETRIC_GRID);

			/*if (found)
			{
				cv::drawChessboardCorners(view, m_CalibrationBoardSize, cv::Mat(corners), found);

				cv::imshow("distorted", view);
				cv::waitKey(0);
			}*/
		}
	}

	// calculate object points
	m_ObjectPoints.clear();
	m_ObjectPoints.resize(numberOfImages);
	for (int image_idx = 0; image_idx < numberOfImages; ++image_idx)
	{
		for (int i = 0; i < m_CalibrationBoardSize.height; ++i)
		{
			for (int j = 0; j < m_CalibrationBoardSize.width; ++j)
			{
				m_ObjectPoints[image_idx].push_back(cv::Point3f((2 * j + i % 2)*m_CalibrationSquareSize, i*m_CalibrationSquareSize, 0));
			}
		}
	}

	// calibrate camera matrices and get distortion coefficients
	m_CameraMatrix[0] = cv::Mat::eye(3, 3, CV_64F);
	m_CameraMatrix[1] = cv::Mat::eye(3, 3, CV_64F);
	double rms = cv::stereoCalibrate(m_ObjectPoints, m_ImagePoints[0], m_ImagePoints[1],
		m_CameraMatrix[0], m_DistortionCoef[0],
		m_CameraMatrix[1], m_DistortionCoef[1],
		imageSize, 
		m_StereoRotation,
		m_StereoTranslation, 
		m_Essential, 
		m_Fundamental,
		cv::CALIB_FIX_K3
		+ cv::CALIB_FIX_K5
		+ cv::CALIB_ZERO_TANGENT_DIST
		+ cv::CALIB_FIX_ASPECT_RATIO
		+ cv::CALIB_SAME_FOCAL_LENGTH
		+ cv::CALIB_FIX_PRINCIPAL_POINT,
		cv::TermCriteria::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 100, 1e-5));
}