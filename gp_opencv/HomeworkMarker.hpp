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
		std::vector<int> ids;
		std::vector<std::vector<cv::Point2f>> corners, rejectedImgPoints;
		std::vector<cv::Vec3d> rvec, tvec;
		cv::aruco::detectMarkers(in_frame, dictionary, corners, ids, detectorParams, rejectedImgPoints);
		cv::aruco::drawDetectedMarkers(in_frame, corners, ids);
		//cv::imshow("id: 100, size : 256x256", in_frame);
		if (corners.size() > 0)
		{
			cv::aruco::estimatePoseSingleMarkers(corners, 0.065f, this->cameraMatrix, this->distCoeffs, rvec, tvec);
			cv::aruco::drawAxis(in_frame, this->cameraMatrix, this->distCoeffs, rvec, tvec, 0.03f);
			//cv::imshow("asd", in_frame);
			out_rvec = rvec[0];
			out_tvec = tvec[0];
			this->renderTeapot = true;
		}
		else
		{
			this->renderTeapot = false;
		}
	}

private:
	// Parameters for marker detection. Use default.
	cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
	// Pre-defined ArUco markers
	cv::Ptr<cv::aruco::Dictionary> dictionary;
};