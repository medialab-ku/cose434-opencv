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
		try {
			std::vector<int> markerIds;
			std::vector<std::vector<cv::Point2f>> markerCorners;
			std::vector<cv::Vec3d> rvecs, tvecs;
			cv::aruco::detectMarkers(in_frame, dictionary, markerCorners, markerIds, detectorParams);
			cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);
			if (markerIds.size() > 0) {
				out_rvec = rvecs[0];
				out_tvec = tvecs[0];
				cv::aruco::drawAxis(in_frame, cameraMatrix, distCoeffs, out_rvec, out_tvec, 0.1);
				cv::imshow("out", in_frame);
			}
		}
		catch (cv::Exception& e) {
			std::cerr << e.msg << std::endl;
		}
	}

private:
	// Parameters for marker detection. Use default.
	cv::Ptr<cv::aruco::DetectorParameters> detectorParams;
	// Pre-defined ArUco markers
	cv::Ptr<cv::aruco::Dictionary> dictionary;
};