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
		std::vector<std::vector<cv::Point2f>>_corners;
		std::vector<int> _ids;
		std::vector<std::vector<cv::Point2f>> _rejectedImgPoints;
		std::vector<cv::Vec3d> _rvecs, _tvecs;
		cv::aruco::detectMarkers(in_frame, this->dictionary, _corners, _ids, this->detectorParams, _rejectedImgPoints, this->cameraMatrix, this->distCoeffs);
		if (_ids.size() > 0) {
			std::cout << "ID is" << _ids[0] << std::endl;
			std::cout << "Corner is " << _corners[0] << std::endl;

			cv::aruco::drawDetectedMarkers(in_frame, _corners, _ids,10.0f);
			
			cv::aruco::estimatePoseSingleMarkers(_corners, 68.0f, this->cameraMatrix, this->distCoeffs, _rvecs, _tvecs);
			cv::aruco::drawAxis(in_frame, this->cameraMatrix, this->distCoeffs, _rvecs[0], _tvecs[0], 10.0f);
			std::cout << "1th rvecs is " << _rvecs[0] << std::endl;
			
			out_rvec = _rvecs[0];
			out_tvec = _tvecs[0];
		}
		
		/////////////////////////////////////////////////////////////////////////////////////////
	}

private:
	// Parameters for marker detection. Use default.
	cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
	// Pre-defined ArUco markers
	cv::Ptr<cv::aruco::Dictionary> dictionary;
};