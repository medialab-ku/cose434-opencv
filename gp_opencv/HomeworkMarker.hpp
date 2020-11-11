#pragma once

#include "HomeworkBase.hpp"

class HomeworkMarker : public HomeworkBase
{
public:
	HomeworkMarker()
	{
		this->featureType = GL::FEATURE_TYPE_MARKER;
	}

protected:
	void Init()
	{
		// Load your video
		this->video.open("res\\video_marker.mp4");

		// Init pre-defined ArUco markers
		this->detectorParams = cv::aruco::DetectorParameters::create();
		this->dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	}

	void EstimatePose(cv::Mat in_frame, cv::Vec3d& out_rvec, cv::Vec3d& out_tvec)
	{
		/////////////////////////////////////////////////////////////////////////////////////////
		//
		// ��� ������ �����Ͽ� Marker Detection �� Pose Estimation�� �����ϴ� �ڵ带 �ۼ��ϼ���
		// ���� �������� �̹����� in_frame �Ű������� ���� ������ �� �ֽ��ϴ�
		// ������� ���� out_rvec, out_tvec �Ű������� �����ϼ���
		//
		std::vector<std::vector<cv::Point2f>>_corners;
		std::vector<std::vector<cv::Point>> _ids;
		std::vector<std::vector<cv::Point2f>> _rejectedImgPoints;
		cv::aruco::detectMarkers(in_frame, &this->dictionary, _corners, _ids, &this->detectorParams, _rejectedImgPoints, this->cameraMatrix, this->distCoeffs);
		
		std::vector<cv::OutputArray> _objPoints;
		cv::Vec3d _rvecs, _tvecs;
		cv::aruco::estimatePoseSingleMarkers(_corners, 68.0f, this->cameraMatrix, this->distCoeffs, _rvecs, _tvecs, _objPoints);
		out_rvec = _rvecs;
		out_tvec = _tvecs;
		/////////////////////////////////////////////////////////////////////////////////////////
	}

private:
	// Parameters for marker detection. Use default.
	cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
	// Pre-defined ArUco markers
	cv::Ptr<cv::aruco::Dictionary> dictionary;
};