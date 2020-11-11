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
		/////////////////////////////////////////////////////////////////////////////////////////

		cv::Mat markerImage;
		cv::aruco::drawMarker(dictionary, 100, 256, markerImage, 1);
		cv::imshow("id: 100, size: 256x256", markerImage);

		cv::imwrite("res\\marker.jpg", markerImage);

		std::vector<std::vector<cv::Point2f>> corners, rejected;
		std::vector<int> ids;
		cv::aruco::detectMarkers(in_frame, dictionary, corners, ids, detectorParams, rejected, cameraMatrix, distCoeffs);

		std::vector<cv::Vec3d> rvecs, tvecs;
		if (ids.size() > 0) {
			cv::aruco::estimatePoseSingleMarkers(
				corners, 0.07, cameraMatrix, distCoeffs, rvecs, tvecs);
		}

		//if (rvecs.size() > 0)
			out_rvec = rvecs[0];
		//if (tvecs.size() > 0)
			out_tvec = tvecs[0];

		//out_rvec = rvecs[0];
		//out_tvec = tvecs[0];
		//cv::aruco::drawAxis(in_frame, cameraMatrix, distCoeffs, rvecs, tvecs, 25);
	}

private:
	// Parameters for marker detection. Use default.
	cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
	// Pre-defined ArUco markers
	cv::Ptr<cv::aruco::Dictionary> dictionary;
};