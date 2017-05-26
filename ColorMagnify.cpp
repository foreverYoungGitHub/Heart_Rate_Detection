/**
 * Questions:
 * 	1. I didn't see the color transformation for the original image, which I think it may be necessary.
 * 	2. I not familiar with the space filter, so I can not confirm about whether the Gaussian pyramid is correct,
 * 		do we need to keep the image as the same size.
 * 	3. Could we directly use the amplified result from space and time filter?
 * 	4. Let us specify the mathematics in this afternoon.
 *
 * Others:
 * 	1. I think we could transform the RGB color space to the YUV color space rather than the YIQ color space;
 * 	2. In the near future, we should also add the other two amplify function (motion and phase).
 *
 */

#include "ColorMagnify.h"

ColorMagnify::ColorMagnify() :
	fl_(0.83),fh_(1.0),rate_(30), levels_(4), alpha_(100), length_(30) {
}

ColorMagnify::~ColorMagnify() {}

// main process of color magnification
void ColorMagnify::EVM(std::vector<cv::Mat>&frames, std::vector<cv::Mat>& imgs)
{
	length_ = frames.size();

	// down sampled frames - space filter
	std::vector<cv::Mat> downSampledFrames;
	//int abcd = 0;
	for(auto & frame : frames) {
		//abcd++;
		//if (abcd == 1)
			//continue;
		//cv::resize(frame, frame, cv::Size(10, 10));
		//std::cout << frame << std::endl;
		
		frame.convertTo(frame, CV_32FC3);
		//std::cout << frame << std::endl;
		

		auto pyramid = buildGaussianPyramid(frame, levels_);
		//std::cout << pyramid << std::endl;
		//cv::cvtColor(pyramid, pyramid, CV_RGB2BGR);
		//std::cout << pyramid << std::endl;
		//pyramid = rgb2ntsc(pyramid);//convert to yiq
		//cv::cvtColor(pyramid, pyramid, CV_BGR2RGB);
		//std::cout << pyramid << std::endl;
		//cv::imshow("1", pyramid);
		//cv::waitKey(0);
		downSampledFrames.push_back(pyramid);


	}

//	for (int i=0; i < frames.size(); i++) {
//		// Gaussian pyramid
//		std::vector<cv::Mat> pyramid;
//		buildGaussianPyramid(frames[i], levels_, pyramid);
//		downSampledFrames.push_back(pyramid[levels_ - 1]);
//	}

	// concatenated image of all the down sampled frames
	auto videoMat = concat(downSampledFrames);

	

//	concat(downSampledFrames, videoMat);

	// filtered image after concatenate
	cv::Mat filtered;
	temporalIdealFilter(videoMat, filtered);

    cv::imshow("0",filtered);
    cv::waitKey(0);
	
	// amplified image
	auto amplified=amplify(filtered);

//	double minVal, maxVal;
//	minMaxLoc(amplified, &minVal, &maxVal);
//	amplified.convertTo(amplified, CV_8UC3, 255.0 / (maxVal - minVal),
//		(-minVal * 255.0 / (maxVal - minVal)));
//	cv::Mat _;
////	cv::resize(amplified, _, cv::Size(200, 200));
//	//std::cout << _ << std::endl;
//	cv::imshow("b", _);
//	cv::waitKey(0);


	// frames after temporal filtering
	std::vector<cv::Mat > filteredFrames;
	deConcat(amplified, downSampledFrames[0].size(), filteredFrames);
	for (int j=0; j < frames.size(); j++) {

		// motion image
		cv::Mat motion;

		upsamplingFromGaussianPyramid(filteredFrames[j], levels_, motion);
		resize(motion, motion, frames[j].size());
		
		// temp image
		cv::Mat temp = frames[j] + motion;
		
		double minVal, maxVal;
		minMaxLoc(temp, &minVal, &maxVal);//find minimun and maximum intensities
		//temp = ntsc2rgb(temp);
		
		// output frame
		cv::Mat output;
		temp.convertTo(output, CV_8UC3, 255.0 / (maxVal - minVal),
			(-minVal * 255.0 / (maxVal - minVal)));
		/*cv::Mat _;
		cv::resize(output, _, cv::Size(10, 10));
		std::cout << _ << std::endl;
		cv::imshow("b", output);
		cv::waitKey(0);*/
		imgs.push_back(output);
	}
}

// convert rgb to yiq
cv::Mat ColorMagnify::rgb2ntsc(const cv::Mat_<cv::Vec3f>& src)
{
	auto ret = src.clone();
	cv::Mat T = (cv::Mat_<float>(3, 3) << 1, 1, 1, 0.956, -0.272, -1.106, 0.621, -0.647, 1.703);
	T = T.inv(); //here inverse!

	for (int j = 0; j<src.rows; j++) {
		for (int i = 0; i<src.cols; i++) {
			ret.at<cv::Vec3f>(j, i)(0) = src.at<cv::Vec3f>(j, i)(0) * T.at<float>(0.0)
				+ src.at<cv::Vec3f>(j, i)(1) * T.at<float>(0, 1)
				+ src.at<cv::Vec3f>(j, i)(2) * T.at<float>(0, 2);

			ret.at<cv::Vec3f>(j, i)(1) = src.at<cv::Vec3f>(j, i)(0) * T.at<float>(1.0)
				+ src.at<cv::Vec3f>(j, i)(1) * T.at<float>(1, 1)
				+ src.at<cv::Vec3f>(j, i)(2) * T.at<float>(1, 2);

			ret.at<cv::Vec3f>(j, i)(2) = src.at<cv::Vec3f>(j, i)(0) * T.at<float>(2.0)
				+ src.at<cv::Vec3f>(j, i)(1) * T.at<float>(2, 1)
				+ src.at<cv::Vec3f>(j, i)(2) * T.at<float>(2, 2);
		}
	}
	return ret;
}
/**
* buildGaussianPyramid	-	construct a gaussian pyramid from a given image
*
* @param img		-	source image
* @param levels		-	levels of the destinate pyramids
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
* buildGaussianPyramid	-	construct a gaussian pyramid from a given image
*
* @param img		-	source image
* @param levels		-	levels of the destinate pyramids
*
* @return the destinate image
*/
cv::Mat ColorMagnify::buildGaussianPyramid(const cv::Mat &img, const int levels) {
	if (levels < 1) {
		perror("Levels should be larger than 1");
	}
	auto currentImg = img;
	for (int l = 0; l<levels; l++) {
		cv::Mat down;
		cv::pyrDown(currentImg, down);
		currentImg = std::move(down);
	}

	return currentImg;
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
void ColorMagnify::createIdealBandpassFilter(cv::Mat &filter, double _fl, double _fh, double _rate)
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
* amplify			-	ampilfy the motion
*
* @param filtered	- motion image
*/
cv::Mat ColorMagnify::amplify(const cv::Mat &src) {
	return src * alpha_;
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
* concat		-	concat all the frames into a single large Mat
*              where each column is a reshaped single frame
*
* @param frames	-	frames of the video sequence
*
* @return 		-	destinate concatnate image
*/
cv::Mat ColorMagnify::concat(const std::vector<cv::Mat> &frames) {
	cv::Size frameSize = frames[0].size();
	int length = frames.size();

	cv::Mat temp(frameSize.width*frameSize.height, length, CV_32FC3);
	for (int i = 0; i < length; i++) {
		// get a frame if any
		auto input = frames[i];
		// reshape the frame into one column
		cv::Mat reshaped = input.reshape(3, input.cols*input.rows).clone();
		cv::Mat line = temp.col(i);
		// save the reshaped frame to one column of the destinate big image
		reshaped.copyTo(line);
	}

	return temp;
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
// convert yiq to rgb
cv::Mat ColorMagnify::ntsc2rgb(const cv::Mat_<cv::Vec3f>& src)
{
	auto ret = src.clone();
	cv::Mat T = (cv::Mat_<float>(3, 3) << 1.0, 0.956, 0.621, 1.0, -0.272, -0.647, 1.0, -1.106, 1.703);
	T = T.t(); //here transpose!

	for (int j = 0; j<src.rows; j++) {
		for (int i = 0; i<src.cols; i++) {
			ret.at<cv::Vec3f>(j, i)(0) = src.at<cv::Vec3f>(j, i)(0) * T.at<float>(0.0)
				+ src.at<cv::Vec3f>(j, i)(1) * T.at<float>(0, 1)
				+ src.at<cv::Vec3f>(j, i)(2) * T.at<float>(0, 2);

			ret.at <cv::Vec3f > (j, i)(1) = src.at<cv::Vec3f>(j, i)(0) * T.at<float>(1.0)
				+ src.at<cv::Vec3f>(j, i)(1) * T.at<float>(1, 1)
				+ src.at<cv::Vec3f>(j, i)(2) * T.at<float>(1, 2);

			ret.at<cv::Vec3f>(j, i)(2) = src.at<cv::Vec3f>(j, i)(0) * T.at<float>(2.0)
				+ src.at<cv::Vec3f>(j, i)(1) * T.at<float>(2, 1)
				+ src.at<cv::Vec3f>(j, i)(2) * T.at<float>(2, 2);
		}
	}
	return ret;
}