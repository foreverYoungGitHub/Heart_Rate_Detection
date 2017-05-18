// Created by Ying on 05/15/2017

#include "ColorMagnify.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;


/**
 * This code can be used to capture the frame per second (fps) of the video or the webcam
 * the Key function of this is
 *      - auto fps = video.get(CV_CAP_PROP_FPS);
 * video is an object of VideoCapture in OpenCV.
 */
int main() {

    VideoCapture video(0);

    double fps = video.get(CV_CAP_PROP_FPS);

    cout << "Frames per second using video.get(CV_CAP_PROP_FPS) : " << fps << endl;

    // Number of frames to capture
    int num_frames = 120;

    // Start and end times
    time_t start, end;

    // Variable for storing video frames
    Mat frame;

    cout << "Capturing " << num_frames << " frames" << endl ;

    // Start time
    time(&start);

    // Grab a few frames
    for(int i = 0; i < num_frames; i++)
    {
        video >> frame;
    }

    // End Time
    time(&end);

    // Time elapsed
    double seconds = difftime (end, start);
    cout << "Time taken : " << seconds << " seconds" << endl;

    // Calculate frames per second
    fps  = num_frames / seconds;
    cout << "Estimated frames per second : " << fps << endl;

    // Release video
    video.release();
    return 0;
}

//int main() {
//	ColorMagnify evm;
//	//VideoCapture cap(0);
//	ifstream in("C:/Users/Ying Qiu/desktop/result/photo.txt");
//	string line;
//	char image_name[50];
//	Mat src;
//	Mat temp;
//	vector<Mat> frames;
//	vector<Mat> imgs;
//	//if (!cap.isOpened())
//	//	cout << "fail to open!" << endl;
//	//while (cap.read(src))
//	if (in) {
//		for (int i = 0;i < 30;i++)
//		{
//			//cap >> src;
//			getline(in, line);
//			src=imread(line);
//			src.convertTo(temp, CV_32FC3);
//			frames.push_back(temp.clone());
//			//evm.EVM(frames, imgs);
//			//imshow("a", temp);
//			//waitKey(10000);
//		}
//		evm.EVM(frames, imgs);
//		/*for (int i = 0; i < imgs.size(); i++)
//		{
//			sprintf(image_name, "%s%d%s", "C:/Users/Ying Qiu/desktop/result/", i+1, ".jpg");
//			imwrite(image_name, imgs[i]);
//		}*/
//		Mat dst;
//		cv::Size frameSize = cv::Size(10, 10);//imgs[0].size();
//		int length_ = imgs.size();
//		cv::Mat temp(frameSize.width*frameSize.height, length_, CV_32FC3);
//
//
//		for (int i = 0; i < length_; i++) {
//			auto input = imgs[i];
//			input.convertTo(input, CV_32FC3);
//			cv::resize(input, input, frameSize);
//
//			//cout << input << endl;
//			// reshape the frame into one column
//			cv::Mat reshaped = input.reshape(3, input.cols*input.rows).clone();
//			//cout << reshaped << endl;
//			cv::Mat line = temp.col(i);
//			// save the reshaped frame to one column of the destinate big image
//			reshaped.copyTo(line);
//		}
//
//		//for (int i = 0; i < length_; i++) {
//		//	//cout << temp << endl;
//
//		//	// get a frame if any
//		//	cv::Mat input = imgs.at(i);
//		//	input.convertTo(input, CV_32FC3);
//		//	cv::resize(input, input, frameSize);
//
//		//	//cout << input << endl;
//		//	// reshape the frame into one column
//		//	cv::Mat reshaped = input.reshape(3, input.cols*input.rows).clone();
//		//	//cout << reshaped << endl;
//		//	cv::Mat line = temp.col(i);
//		//	// save the reshaped frame to one column of the destinate big image
//		//	reshaped.copyTo(line);
//		//	//cout << temp << endl;
//		//}
//		temp.convertTo(temp, CV_8UC3);
//		temp.copyTo(dst);
//		while (true) {
//			imshow("a", dst);
//			waitKey(1);
//		}
//
//		////cout << dst << endl;
//	}
//
//
//	return 0;
//}
