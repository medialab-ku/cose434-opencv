#pragma once

#include "HomeworkBase.hpp"

class HomeworkNatural : public HomeworkBase
{
public:
	HomeworkNatural()
	{
		this->featureType = GL::FEATURE_TYPE_NATURAL;
		//this->featureType = GL::FEATURE_TYPE_MARKER;
	}

protected:
	void Init()
	{
		// Load your target image
		cv::Mat targetImage = cv::imread("res\\natural.jpg");
		cv::cvtColor(targetImage, this->targetImageGray, cv::COLOR_BGR2GRAY);

		// Load your video
		this->video.open("res\\video_natural.mp4");

		// Init ORB feature
		this->feature = cv::ORB::create();
		this->feature->detectAndCompute(targetImageGray, cv::Mat(), this->targetKeyPoints, this->targetDescriptors);

		// Init brute-force matcher
		this->matcher = cv::BFMatcher(cv::NORM_HAMMING);
	}

	void EstimatePose(cv::Mat in_frame, cv::Vec3d &out_rvec, cv::Vec3d &out_tvec)
	{
		/////////////////////////////////////////////////////////////////////////////////////////
		//
		// ��� ������ �־��� �ڵ带 �����Ͽ� Feature Extraction, Feature Matching �� 
		// Pose Estimation�� �����ϴ� �ڵ带 �ۼ��ϼ���
		// ���� �������� �̹����� in_frame �Ű������� ���� ������ �� �ֽ��ϴ�
		//
		/////////////////////////////////////////////////////////////////////////////////////////

		// ----------------------------------------------------------
		// ���⿡ ORB Feature�� �����ϴ� �ڵ带 �ۼ��ϼ���
		std::vector<cv::KeyPoint> keyPoints;
		cv::Mat descriptors;
		this->feature->detectAndCompute(in_frame, cv::Mat(), keyPoints, descriptors);
		// ----------------------------------------------------------


		// ----------------------------------------------------------
		// ���⿡ ORB Feature Matching�� �����ϴ� �ڵ带 �ۼ��ϼ���
		std::vector<cv::DMatch> matches;
		this->matcher.match(this->targetDescriptors, descriptors, matches);
		// ----------------------------------------------------------

		FilterBadMatches(matches);
		cv::Mat image;
		cv::drawMatches(this->targetImageGray, this->targetKeyPoints, in_frame, keyPoints, matches, image);
		cv::imshow("good", image);
		//cv::waitKey(0);
		// ----------------------------------------------------------
		// ���⿡ Pose Estimation�� �����ϴ� �ڵ带 �ۼ��ϼ���
		cv::Vec3d rvec, tvec;
		std::vector<cv::Point3f> objectPoints;
		std::vector<cv::Point2f> imagePoints;
		
		for (int i = 0; i < matches.size(); i++)
		{
			objectPoints.push_back(cv::Point3f(this->targetKeyPoints[matches[i].queryIdx].pt)*0.000215);
			imagePoints.push_back(keyPoints[matches[i].trainIdx].pt);
		}
		// ----------------------------------------------------------
		if (matches.size() > 10)
		{
			cv::solvePnPRansac(objectPoints, imagePoints, this->cameraMatrix, this->distCoeffs, out_rvec, out_tvec);
			this->renderTeapot = true;
		}
		else this->renderTeapot = false;
	}

	void FilterBadMatches(std::vector<cv::DMatch> &matches)
	{
		std::sort(matches.begin(), matches.end());
		const int numGoodMatches = matches.size() * 0.2;
		matches.erase(matches.begin() + numGoodMatches, matches.end());
		for (int i = matches.size()-1; i >= 0; i--)
		{
			if (matches[i].distance > 30.0f)
				matches.pop_back();
			else break;
		}
	}

private:
	// Your detection target image as gray scale
	cv::Mat targetImageGray;

	// ORB feature module
	cv::Ptr<cv::ORB> feature;

	// Feature matching module (Use brute-force module)
	cv::BFMatcher matcher;

	// Extracted key points from target image
	std::vector<cv::KeyPoint> targetKeyPoints;

	// Extracted descriptors from target image
	cv::Mat targetDescriptors;
};