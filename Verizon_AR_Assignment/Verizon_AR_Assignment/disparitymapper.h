#pragma once
#include <opencv2\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\calib3d\calib3d.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\ximgproc\disparity_filter.hpp>
#include <opencv2\xfeatures2d\nonfree.hpp>

enum class DISPARITY_MAPPER_QUALITY { DISPARITY_MAPPER_QUALITY_VERY_FAST, DISPARITY_MAPPER_QUALITY_FAST, DISPARITY_MAPPER_QUALITY_QUALITY };

class DisparityMapper
{
public:
	DisparityMapper(cv::Mat _left, cv::Mat _right, int _numDisparities, int _wsize, bool _rectify = false, DISPARITY_MAPPER_QUALITY _quality = DISPARITY_MAPPER_QUALITY::DISPARITY_MAPPER_QUALITY_FAST);
	DisparityMapper(const DisparityMapper& _other) = default;
	~DisparityMapper() = default;

	void Compute();

	inline cv::Mat GetDisparity()							{ return m_Disparity; }
	inline cv::Mat GetCroppedDisparity()					{ return m_Disparity(m_LeftRegionOfInterest); }
	inline cv::Mat GetLeftOriginal()						{ return m_LeftOriginal; }
	inline cv::Mat GetCroppedLeftOriginal()					{ return m_LeftOriginal(m_LeftRegionOfInterest); }
	inline cv::Mat GetRightOriginal()						{ return m_RightOriginal; }
	inline cv::Mat GetCroppedRightOriginal()				{ return m_RightOriginal(m_LeftRegionOfInterest); }

	inline void SetNumDisparities(int _value)				{ m_NumDisparities = _value; }
	inline void SetMinDisparity(int _value)					{ m_MinDisparity = _value; }
	inline void SetSADWindowSize(int _value)				{ m_SADWindowSize = _value; }
	inline void SetUniquenessRatio(int _value)				{ m_UniquenessRatio = _value; }
	inline void SetDisp12MaxDiff(int _value)				{ m_Disp12MaxDiff = _value; }
	inline void SetP1(int _value)							{ m_P1 = _value; }
	inline void SetP2(int _value)							{ m_P2 = _value; }
	inline void SetSpeckleWindowSize(int _value)			{ m_SpeckleWindowSize = _value; }
	inline void SetMode(int _value)							{ m_Mode = _value; }
	inline void SetLambdaValue(double _value)				{ m_LambdaValue = _value; }
	inline void SetSigmaColor(double _value)				{ m_SigmaColor = _value; }
	inline void SetUseConfidence(bool _value)				{ m_UseConfidence = _value; }
	inline void SetQuality(DISPARITY_MAPPER_QUALITY _value)	{ m_Quality = _value; }
	inline void SetDownscale(bool _value)					{ m_Downscale = _value; }
	inline void SetQMatrix(cv::Mat _value)					{ m_Q = _value; m_QMatSet = true; }
	inline void SetCalibrationImageFilename(char* _value)	{ m_CalibrationImagesFilename = _value; }

	inline int		GetNumDisparities()						{ return m_NumDisparities; }
	inline int		GetMinDisparity()						{ return m_MinDisparity; }
	inline int		GetSADWindowSize()						{ return m_SADWindowSize; }
	inline int		GetUniquenessRatio()					{ return m_UniquenessRatio; }
	inline int		GetDisp12MaxDiff()						{ return m_Disp12MaxDiff; }
	inline int		GetP1()									{ return m_P1; }
	inline int		GetP2()									{ return m_P2; }
	inline int		GetSpeckleWindowSize()					{ return m_SpeckleWindowSize; }
	inline int		GetMode()								{ return m_Mode; }
	inline double	GetLambdaValue()						{ return m_LambdaValue; }
	inline double	GetSigmaColor()							{ return m_SigmaColor; }
	inline bool		GetUseConfidence()						{ return m_UseConfidence; }
	inline DISPARITY_MAPPER_QUALITY GetQuality()			{ return m_Quality; }
	inline bool		GetDownscale()							{ return m_Downscale; }
	inline cv::Mat	GetQMatrix()							{ return m_Q; }
	inline cv::Mat	GetPointCloud()							{ return m_PointCloud; }
	inline double GetBaseline()								{ return m_Baseline; }
	inline double GetFocalLength()							{ return m_FocalLength; }

private:
	void _computeQuality();
	void _computeFast();
	void _computeVeryFast();
	void _createPointCloud();
	cv::Rect _computeRegionOfInterest(cv::Size2i _size, cv::Ptr<cv::StereoMatcher> _matcher);
	bool _getCalibrationImages();
	void _calibrateCamera();
	void _rectifyImages();
	void _getCalibrationQuality();

private:
	cv::Mat m_LeftOriginal;
	cv::Mat m_RightOriginal;
	cv::Mat m_LeftRectified;
	cv::Mat m_RightRectified;
	cv::Mat m_Disparity;

	cv::Rect m_LeftRegionOfInterest;
	cv::Rect m_RightRegionOfInterest;

	cv::Mat m_CameraMatrix[2];
	cv::Mat m_DistortionCoef[2];

	cv::Mat m_Q;
	cv::Mat m_PointCloud;

	int m_NumDisparities;
	int m_MinDisparity;
	int m_SADWindowSize;
	int m_UniquenessRatio;
	int m_Disp12MaxDiff;
	int m_P1;
	int m_P2;
	int m_SpeckleWindowSize;
	int m_Mode;
	double m_LambdaValue;
	double m_SigmaColor;
	bool m_UseConfidence;
	bool m_Downscale;
	bool m_RectifyImages;
	DISPARITY_MAPPER_QUALITY m_Quality;
	bool m_QMatSet;

	double m_FocalLength;
	double m_Baseline;

	char* m_CalibrationImagesFilename;
	std::vector<cv::Mat> m_CalibrationImages;
	cv::Size m_CalibrationBoardSize;
	int m_CalibrationSquareSize;

	cv::Mat m_StereoRotation;
	cv::Mat m_StereoTranslation;
	cv::Mat m_Fundamental;
	cv::Mat m_Essential;

	std::vector<std::vector<cv::Point2f>> m_ImagePoints[2];
	std::vector<std::vector<cv::Point3f>> m_ObjectPoints;
};