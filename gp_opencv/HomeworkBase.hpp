#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include "GL.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/aruco.hpp>

class HomeworkBase
{
public:
	HomeworkBase()
	{
		this->featureType = 0;
		this->renderTeapot = true;
	}

	void Run()
	{
		this->CalibrateCamera(this->cameraMatrix, this->distCoeffs);
		this->Init();

		if (!GL::Init(cameraMatrix.at<double>(0, 0),
			cameraMatrix.at<double>(1, 1),
			cameraMatrix.at<double>(0, 2),
			cameraMatrix.at<double>(1, 2),
			this->featureType))
			return;
		if (!GL::CreateBackground()) return;
		if (!GL::CreateTeapot()) return;
		glUseProgram(0);

		cv::Mat frame;

		float timer = 0.0f;
		const float waitTime = 0.0f;
		const float startRenderTime = 0.5f;
		const double FPS = 29.99;
		while (!glfwWindowShouldClose(GL::window))
		{
			clock_t before;
			before = clock();

			// User Input
			GL::ProcessInput();

			if (timer > waitTime)
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// Render background
				glDisable(GL_DEPTH_TEST);
				if (!GrabVideo(this->video, frame)) break;
				GL::backShader->use();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, GL::backTexture);
				glBindVertexArray(GL::backVAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}

			if (timer > startRenderTime)
			{
				cv::Vec3d rvec, tvec;
				this->EstimatePose(frame, rvec, tvec);

				if (renderTeapot)
				{
					cv::Mat rot;
					double rodrigues[9], t[3];
					cv::Rodrigues(rvec, rot);
					rodrigues[0] = rot.at<double>(0, 0);
					rodrigues[1] = rot.at<double>(1, 0);
					rodrigues[2] = rot.at<double>(2, 0);
					rodrigues[3] = rot.at<double>(0, 1);
					rodrigues[4] = rot.at<double>(1, 1);
					rodrigues[5] = rot.at<double>(2, 1);
					rodrigues[6] = rot.at<double>(0, 2);
					rodrigues[7] = rot.at<double>(1, 2);
					rodrigues[8] = rot.at<double>(2, 2);
					t[0] = tvec[0];
					t[1] = tvec[1];
					t[2] = tvec[2];
					GL::MakeViewMatrix(rodrigues, t);

					// render teapot
					glEnable(GL_DEPTH_TEST);
					GL::teapotShader->use();
					GL::teapotShader->setMat4("view", GL::viewMatrix);
					GL::teapotShader->setVec3("viewPos", glm::vec3(-t[0], -t[1], -t[2]));
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, GL::teapotTexture);
					glBindVertexArray(GL::teapotVAO);
					glDrawElements(GL_TRIANGLES, GL::indexCount, GL_UNSIGNED_INT, 0);
				}
			}

			// swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			glfwSwapBuffers(GL::window);
			glfwPollEvents();

			double time = (double)(clock() - before) / CLOCKS_PER_SEC;
			if (timer < startRenderTime) timer += time;
			if (1.0f / FPS > time)
			{
				float s = (1.0f / FPS - time) * 1000;
				if (timer < startRenderTime) timer += (s / 1000);
				Sleep(s);
			}
		}

		GL::Dispose();
	}


protected:
	void CalibrateCamera(cv::Mat& out_cameraMatrix, cv::Mat& out_distCoeffs)
	{
		// 필요할 경우 값을 수정하여 사용하세요
		const cv::Size NUM_CORNERS = cv::Size(8, 6);
		const float CALIB_SQUARE_SIZE = 25; // mm
		const int NUM_CALIB_IMAGES = 7;
		const cv::Size CALIB_IMAGE_SIZE = cv::Size(1280, 720);

		// ------------------------------------------------------------------------
		//
		// 주어진 코드를 참고하여
		// Camera Calibration을 수행하는 코드를 여기에 작성하세요
		// 결과값은 각각 out_cameraMatrix, out_distCoeffs 매개변수에 대입하세요
		//
		// ------------------------------------------------------------------------

		std::vector<cv::Point3f> object_points;
		for (int i = 0; i < NUM_CORNERS.height; i++)
			for (int j = 0; j < NUM_CORNERS.width; j++)
				object_points.push_back(cv::Point3f(j * CALIB_SQUARE_SIZE, i * CALIB_SQUARE_SIZE, 0));

		std::vector<std::vector<cv::Point3f>> list_of_object_points;
		std::vector<std::vector<cv::Point2f>> list_of_image_points;

		for (unsigned int idx = 1; idx <= NUM_CALIB_IMAGES; idx += 1) {
			std::stringstream file_name;
			file_name << "res\\" << idx << ".jpg";

			cv::Mat image;
			image = cv::imread(file_name.str());

			std::vector<cv::Point2f> image_points;
			bool found = cv::findChessboardCorners(image, NUM_CORNERS, image_points);

			if (found) {
				list_of_object_points.push_back(object_points);
				list_of_image_points.push_back(image_points);
			}
		}
		std::vector<cv::Mat> rvecs, tvecs;

		out_cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
		out_distCoeffs = cv::Mat::zeros(8, 1, CV_64F);

		cv::calibrateCamera(list_of_object_points, list_of_image_points, CALIB_IMAGE_SIZE, out_cameraMatrix, out_distCoeffs, rvecs, tvecs);
	}

	// Grab image from the video, create texture and generate mipmaps
	bool GrabVideo(cv::VideoCapture& video, cv::Mat& frame)
	{
		video.read(frame);
		if (frame.empty()) {
			std::cout << "The video has reached to its end." << std::endl;
			return false;
		}

		cv::Mat frameRGB;
		cv::cvtColor(frame, frameRGB, cv::COLOR_BGR2RGB);

		glBindTexture(GL_TEXTURE_2D, GL::backTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameRGB.cols, frameRGB.rows,
			0, GL_RGB, GL_UNSIGNED_BYTE, frameRGB.ptr());

		return true;
	}

	virtual void Init() = 0;
	virtual void EstimatePose(cv::Mat in_frame, cv::Vec3d& out_rvec, cv::Vec3d& out_tvec) = 0;

protected:
	int featureType;
	bool renderTeapot;
	cv::VideoCapture video;
	cv::Mat cameraMatrix, distCoeffs;
};