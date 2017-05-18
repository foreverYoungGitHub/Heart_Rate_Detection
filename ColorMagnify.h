//

#ifndef COLORMODIFY_H
#define COLORMODIFY_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>


class ColorMagnify {
public:
	ColorMagnify();
	~ColorMagnify();

	//main process
	void EVM(std::vector<cv::Mat>&frames, std::vector<cv::Mat>& imgs);

	// build a gaussian pyramid
	void buildGaussianPyramid(const cv::Mat &img, const int levels,
		std::vector<cv::Mat> &pyramid);
	cv::Mat buildGaussianPyramid(const cv::Mat &img, const int levels);

	// concat images into a large Mat
	void concat(const std::vector<cv::Mat> &frames, cv::Mat &dst);
	cv::Mat concat(const std::vector<cv::Mat> &frames);

	// temporal ideal bandpass filtering
	void temporalIdealFilter(const cv::Mat &src,
		cv::Mat &dst);

	// create an ideal bandpass processor
	void createIdealBandpassFilter(cv::Mat &filter, double& fl, double& fh, double& rate);

	// amplify motion
	void amplify(const cv::Mat &src, cv::Mat &dst);
	cv::Mat amplify(const cv::Mat &src);

	// up-sampling an image from gaussian pyramid
	void upsamplingFromGaussianPyramid(const cv::Mat &src, const int levels,
		cv::Mat &dst);



	// de-concat the concatnate image into frames
	void deConcat(const cv::Mat &src, const cv::Size &frameSize, std::vector<cv::Mat> &frames);


	//
	double fl_;
	double fh_;
	double rate_;
	int levels_;
	float alpha_;
	int length_;//number of frames
};
#endif //COLORMODIFY_H#pragma once
