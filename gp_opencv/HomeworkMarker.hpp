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
		// 멤버 변수를 참고하여 Marker Detection 및 Pose Estimation을 수행하는 코드를 작성하세요
		// 현재 프레임의 이미지는 in_frame 매개변수를 통해 접근할 수 있습니다
		// 결과값은 각각 out_rvec, out_tvec 매개변수에 대입하세요
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