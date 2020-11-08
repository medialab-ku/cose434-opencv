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
		const float WORLD_SQUARE_SIZE = 0.10; // meters

		std::vector<std::vector<cv::Point2f>> _corners;
		std::vector<int> _ids;
		cv::aruco::detectMarkers(in_frame, this->dictionary, _corners, _ids,
			this->detectorParams);

		std::vector<cv::Vec3d> rvecs, tvecs;
		if (_ids.empty())
		{
			out_rvec = cv::Vec3d(0, 0, 0);
			out_tvec = cv::Vec3d(-10, -10, -10);
			return;
		}

		cv::aruco::estimatePoseSingleMarkers(_corners, WORLD_SQUARE_SIZE,
			this->cameraMatrix, this->distCoeffs,
			rvecs, tvecs);

		if (rvecs.empty() || tvecs.empty())
		{
			out_rvec = cv::Vec3d(0, 0, 0);
			out_tvec = cv::Vec3d(-10, -10, -10);
			return;
		}

		out_rvec = rvecs.at(0);
		out_tvec = tvecs.at(0);
	}

private:
	// Parameters for marker detection. Use default.
	cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
	// Pre-defined ArUco markers
	cv::Ptr<cv::aruco::Dictionary> dictionary;
};