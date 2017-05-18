// Created by Ying on 05/15/2017

#include "ColorMagnify.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

int main() {
	ColorMagnify evm;
	//VideoCapture cap(0);
	ifstream in("C:/Users/Ying Qiu/desktop/result/photo.txt");
	string line;
	char image_name[50];
	Mat src;
	Mat temp;
	vector<Mat> frames;
	vector<Mat> imgs;
	//if (!cap.isOpened())
	//	cout << "fail to open!" << endl;
	//while (cap.read(src))
	if (in) {
		for (int i = 0;i < 30;i++)
		{
			//cap >> src;
			getline(in, line);
			src=imread(line);
			src.convertTo(temp, CV_32FC3);
			frames.push_back(temp.clone());
			//evm.EVM(frames, imgs);
			//imshow("a", temp);
			//waitKey(10000);
		}
		evm.EVM(frames, imgs);
		/*for (int i = 0; i < imgs.size(); i++)
		{
			sprintf(image_name, "%s%d%s", "C:/Users/Ying Qiu/desktop/result/", i+1, ".jpg");
			imwrite(image_name, imgs[i]);
		}*/
		Mat dst;
		cv::Size frameSize = cv::Size(10, 10);//imgs[0].size();
		int length_ = imgs.size();
		cv::Mat temp(frameSize.width*frameSize.height, length_, CV_32FC3);


		for (int i = 0; i < length_; i++) {
			auto input = imgs[i];
			input.convertTo(input, CV_32FC3);
			cv::resize(input, input, frameSize);

			//cout << input << endl;
			// reshape the frame into one column
			cv::Mat reshaped = input.reshape(3, input.cols*input.rows).clone();
			//cout << reshaped << endl;
			cv::Mat line = temp.col(i);
			// save the reshaped frame to one column of the destinate big image
			reshaped.copyTo(line);
		}

		//for (int i = 0; i < length_; i++) {
		//	//cout << temp << endl;

		//	// get a frame if any
		//	cv::Mat input = imgs.at(i);
		//	input.convertTo(input, CV_32FC3);
		//	cv::resize(input, input, frameSize);

		//	//cout << input << endl;
		//	// reshape the frame into one column
		//	cv::Mat reshaped = input.reshape(3, input.cols*input.rows).clone();
		//	//cout << reshaped << endl;
		//	cv::Mat line = temp.col(i);
		//	// save the reshaped frame to one column of the destinate big image
		//	reshaped.copyTo(line);
		//	//cout << temp << endl;
		//}
		temp.convertTo(temp, CV_8UC3);
		temp.copyTo(dst);
		while (true) {
			imshow("a", dst);
			waitKey(1);
		}

		////cout << dst << endl;
	}

	
	return 0;
}
