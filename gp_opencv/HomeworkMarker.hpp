#pragma once
#include <iostream>
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
		this->dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_1000);
	}

	void EstimatePose(cv::Mat in_frame, cv::Vec3d& out_rvec, cv::Vec3d& out_tvec)
	{
		/////////////////////////////////////////////////////////////////////////////////////////
		//
		// ��� ������ �����Ͽ� Marker Detection �� Pose Estimation�� �����ϴ� �ڵ带 �ۼ��ϼ���
		// ���� �������� �̹����� in_frame �Ű������� ���� ������ �� �ֽ��ϴ�
		// ������� ���� out_rvec, out_tvec �Ű������� �����ϼ���
		//
		/////////////////////////////////////////////////////////////////////////////////////////

		// detect marker
		std::vector<int> ids;
		std::vector<std::vector<cv::Point2f>> corners, rejected;
		std::vector<cv::Vec3d> rvecs, tvecs;

		cv::aruco::detectMarkers(in_frame, dictionary, corners, ids, detectorParams, rejected);

		if (ids.size() > 0) {
			cv::aruco::estimatePoseSingleMarkers(corners, (float)(0.07), cameraMatrix, distCoeffs, rvecs, tvecs);

			// single marker
			out_rvec = rvecs[0];
			out_tvec = tvecs[0];
		}
	}

private:
	// Parameters for marker detection. Use default.
	cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
	// Pre-defined ArUco markers
	cv::Ptr<cv::aruco::Dictionary> dictionary;
};