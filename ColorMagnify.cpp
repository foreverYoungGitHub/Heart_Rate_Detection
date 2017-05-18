//Created by Ying on 06/15/2017

#include "ColorMagnify.h"

ColorMagnify::ColorMagnify() : 
	 fl_ (0.83), fh_(1.0),rate_(30),levels_(3),alpha_(50),length_(30){
}

ColorMagnify::~ColorMagnify() {}

// main process of color magnification
void ColorMagnify::EVM(std::vector<cv::Mat>&frames, std::vector<cv::Mat>& imgs)
{
	for (int i=0; i < frames.size(); i++) {
		buildGaussianPyramid(frames[i], levels_, pyramid);
		downSampledFrames.push_back(pyramid[levels_ - 1]);
	}
	concat(downSampledFrames, videoMat);
	temporalIdealFilter(videoMat, filtered);
	amplify(filtered, amplified);
	deConcat(amplified, downSampledFrames[0].size(), filteredFrames);

	int n = filteredFrames.size();
	int m = frames.size();

	for (int j=0; j < frames.size(); j++) {

		upsamplingFromGaussianPyramid(filteredFrames[j], levels_, motion);
		resize(motion, motion, frames[j].size());
		temp = frames[j] + motion;
		double minVal, maxVal;
		minMaxLoc(temp, &minVal, &maxVal);//find minimun and maximum intensities
		temp.convertTo(output, CV_8UC3, 255.0 / (maxVal - minVal),
			-minVal * 255.0 / (maxVal - minVal));
		imgs.push_back(output);
	}
}
/**
* buildGaussianPyramid	-	construct a gaussian pyramid from a given image
*
* @param img		-	source image
* @param levels	-	levels of the destinate pyramids
* @param pyramid	-	destinate image
*
* @return true if success
*/
void ColorMagnify::buildGaussianPyramid(const cv::Mat &img, const int levels,
	std::vector<cv::Mat> &pyramid)
{
	if (levels < 1) {
		perror("Levels should be larger than 1");
	}
	pyramid.clear();
	cv::Mat currentImg = img;
	for (int l = 0; l<levels; l++) {
		cv::Mat down;
		cv::pyrDown(currentImg, down);
		pyramid.push_back(down);
		currentImg = down;
	}
}

/**
* temporalIdealFilter	-	temporal IIR filtering an image pyramid of concat-frames
*                          (Thanks to Daniel Ron & Alessandro Gentilini)
*
* @param pyramid	-	source pyramid of concatenate frames
* @param filtered	-	concatenate filtered result
*
*/
void ColorMagnify::temporalIdealFilter(const cv::Mat &src,
	cv::Mat &dst)
{
	cv::Mat channels[3];

	// split into 3 channels
	cv::split(src, channels);

	for (int i = 0; i < 3; ++i) {

		cv::Mat current = channels[i];  // current channel
		cv::Mat tempImg;

		int width = cv::getOptimalDFTSize(current.cols);
		int height = cv::getOptimalDFTSize(current.rows);

		cv::copyMakeBorder(current, tempImg,
			0, height - current.rows,
			0, width - current.cols,
			cv::BORDER_CONSTANT, cv::Scalar::all(0));

		// do the DFT
		//tempImg.convertTo(tempImg, CV_32FC1);
		cv::dft(tempImg, tempImg, cv::DFT_ROWS | cv::DFT_SCALE);

		// construct the filter
		cv::Mat filter = tempImg.clone();
		createIdealBandpassFilter(filter, fl_, fh_, rate_);

		// apply filter
		cv::mulSpectrums(tempImg, filter, tempImg, cv::DFT_ROWS);

		// do the inverse DFT on filtered image
		cv::idft(tempImg, tempImg, cv::DFT_ROWS | cv::DFT_SCALE);

		// copy back to the current channel
		tempImg(cv::Rect(0, 0, current.cols, current.rows)).copyTo(channels[i]);
	}
	// merge channels
	cv::merge(channels, 3, dst);

	// normalize the filtered image
	cv::normalize(dst, dst, 0, 1, CV_MINMAX);
}

/**
* createIdealBandpassFilter	-	create a 1D ideal band-pass filter
*
* @param filter    -	destinate filter
* @param fl        -	low cut-off
* @param fh		-	high cut-off
* @param rate      -   sampling rate(i.e. video frame rate)
*/
void ColorMagnify::createIdealBandpassFilter(cv::Mat &filter, double& _fl, double& _fh, double& _rate)
{
	int width = filter.cols;
	int height = filter.rows;

	_fl = 2 * _fl * width / _rate;
	_fh = 2 * _fh * width / _rate;

	double response;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			// filter response
			if (j >= _fl && j <= _fh)
				response = 1.0f;
			else
				response = 0.0f;
			filter.at<float>(i, j) = response;
		}
	}
}

/**
* amplify	-	ampilfy the motion
*
* @param filtered	- motion image
*/
void ColorMagnify::amplify(const cv::Mat &src, cv::Mat &dst)
{
	dst = src * alpha_;
}

/**
* upsamplingFromGaussianPyramid	-	up-sampling an image from gaussian pyramid
*
* @param src		-	source image
* @param levels	-	levels of the pyramid
* @param dst		-	destinate image
*/
void ColorMagnify::upsamplingFromGaussianPyramid(const cv::Mat &src, const int levels,
	cv::Mat &dst)
{
	cv::Mat currentLevel = src.clone();
	for (int i = 0; i < levels; ++i) {
		cv::Mat up;
		cv::pyrUp(currentLevel, up);
		currentLevel = up;
	}
	currentLevel.copyTo(dst);
}
/**
* concat	-	concat all the frames into a single large Mat
*              where each column is a reshaped single frame
*
* @param frames	-	frames of the video sequence
* @param dst		-	destinate concatnate image
*/
void ColorMagnify::concat(const std::vector<cv::Mat> &frames, cv::Mat &dst)
{
	cv::Size frameSize = frames[0].size();
	int length_ = frames.size();
	cv::Mat temp(frameSize.width*frameSize.height, length_, CV_32FC3);
	for (int i = 0; i < length_; i++) {
		// get a frame if any
		cv::Mat input = frames.at(i);
		// reshape the frame into one column
		cv::Mat reshaped = input.reshape(3, input.cols*input.rows).clone();
		cv::Mat line = temp.col(i);
		// save the reshaped frame to one column of the destinate big image
		reshaped.copyTo(line);
	}
	temp.copyTo(dst);
}
/**
* deConcat	-	de-concat the concatnate image into frames
*
* @param src       -   source concatnate image
* @param framesize	-	frame size
* @param frames	-	destinate frames
*/
void ColorMagnify::deConcat(const cv::Mat &src, const cv::Size &frameSize, std::vector<cv::Mat> &frames)
{
	for (int i = 0; i < length_; ++i) {    // get a line if any
		cv::Mat line = src.col(i).clone();
		cv::Mat reshaped = line.reshape(3, frameSize.height).clone();
		frames.push_back(reshaped);
	}
}
