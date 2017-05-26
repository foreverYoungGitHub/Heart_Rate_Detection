/**
 * Questions:
 * 	1. I didn't see the color transformation for the original image, which I think it may be necessary.
 *
 * Others:
 * 	1. I think we could transform the RGB color space to the YUV color space rather than the YIQ color space;
 * 	2. In the near future, we should also add the other two amplify function (motion and phase).
 *
 */

#include "color_magnify.h"

cv::Mat ColorMagnify::get_filtered_img(std::vector<cv::Mat>_src, int _fps, float _magnify_coeff,
								   double _low_freq, double _high_freq, int _pyramid_level) {
	// down sampled frames - space filter
	std::vector<cv::Mat> downSampledFrames;
	for(auto & src : _src) {
		//the _src img will change to float in flowing steps
		src.convertTo(src, CV_32FC3);
		// Gaussian pyramid
		auto pyramid = buildGaussianPyramid(src, _pyramid_level);
		downSampledFrames.push_back(std::move(pyramid));
	}

	// concatenated image of all the down sampled frames
	auto concated = concat(downSampledFrames);

	// filtered image after concatenate
	auto filtered = temporalIdealFilter(concated, _low_freq, _high_freq, _fps);

	return filtered;
}

std::vector<cv::Mat> ColorMagnify::get_combined_img(std::vector<cv::Mat>_src, int _fps, float _magnify_coeff,
                       			double _low_freq, double _high_freq, int _pyramid_level) {

	// down sampled frames - space filter
	std::vector<cv::Mat> downSampledFrames;
	for(auto & src : _src) {
		//the _src img will change to float in flowing steps
		src.convertTo(src, CV_32FC3);
		// Gaussian pyramid
		auto pyramid = buildGaussianPyramid(src, _pyramid_level);
		downSampledFrames.push_back(std::move(pyramid));
	}

	// concatenated image of all the down sampled frames
	auto concated = concat(downSampledFrames);

	// filtered image after concatenate
	auto filtered = temporalIdealFilter(concated, _low_freq, _high_freq, _fps);

	// amplified image
	auto amplified = amplify(filtered, _magnify_coeff);

	// frames after temporal filtering
	auto filteredFrames = deConcat(amplified, downSampledFrames[0].size());

	std::vector<cv::Mat> output;
	for (int j=0; j < _src.size(); j++) {

		// color image
		auto upSampled = upsamplingFromGaussianPyramid(filteredFrames[j], _pyramid_level);

		cv::resize(upSampled, upSampled, _src[j].size());
		
		// combined image
		cv::Mat combined = _src[j] + upSampled;
		
		double minVal, maxVal;
		cv::minMaxLoc(combined, &minVal, &maxVal);//find minimun and maximum intensities
		
		// int formated frame
		cv::Mat formated;
		combined.convertTo(formated, CV_8UC3, 255.0 / (maxVal - minVal),
			(-minVal * 255.0 / (maxVal - minVal)));

		output.push_back(std::move(formated));
	}

	return output;
}

cv::Mat ColorMagnify::temporalIdealFilter(const cv::Mat &src, double _fl, double _fh, double _rate) {
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
		createIdealBandpassFilter(filter, _fl, _fh, _rate);

		// apply filter
		cv::mulSpectrums(tempImg, filter, tempImg, cv::DFT_ROWS);

		// do the inverse DFT on filtered image
		cv::idft(tempImg, tempImg, cv::DFT_ROWS | cv::DFT_SCALE);

		// copy back to the current channel
		tempImg(cv::Rect(0, 0, current.cols, current.rows)).copyTo(channels[i]);
		
	}

	cv::Mat result;
	// merge channels
	cv::merge(channels, 3, result);
	
	// normalize the filtered image
	cv::normalize(result, result, 0, 1, CV_MINMAX);

	return result;
}


void ColorMagnify::createIdealBandpassFilter(cv::Mat &filter, double _fl, double _fh, double _rate) {
	int width = filter.cols;
	int height = filter.rows;

	auto fl = 2 * _fl * width / _rate;
	auto fh = 2 * _fh * width / _rate;

	double response;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			// filter response
			if (j >= fl && j <= fh)
				response = 1.0f;
			else
				response = 0.0f;
			filter.at<float>(i, j) = response;
		}
	}
}

cv::Mat ColorMagnify::buildGaussianPyramid(const cv::Mat &img, const int levels) {
	if (levels < 1) {
		perror("Levels should be larger than 1");
	}
	auto result = img;
	for (int l = 0; l<levels; l++) {
		cv::Mat down;
		cv::pyrDown(result, down);
		result = std::move(down);
	}

	return result;
}


cv::Mat ColorMagnify::upsamplingFromGaussianPyramid(const cv::Mat src, const int levels) {
	cv::Mat result = src.clone();
	for (int i = 0; i < levels; i++) {
		cv::Mat up;
		cv::pyrUp(result, up);
		result = std::move(up);
	}
	return result;
}


cv::Mat ColorMagnify::concat(const std::vector<cv::Mat> frames) {
	cv::Size frameSize = frames[0].size();
	int length = frames.size();

	cv::Mat temp(frameSize.width*frameSize.height, length, CV_32FC3);
	for (int i = 0; i < length; i++) {
		// get a frame if any
		auto input = frames[i];
		// reshape the frame into one column
		auto reshaped = input.reshape(3, input.cols*input.rows).clone();
		auto line = temp.col(i);
		// save the reshaped frame to one column of the destinate big image
		reshaped.copyTo(line);
	}

	return temp;
}


std::vector<cv::Mat> ColorMagnify::deConcat(const cv::Mat src, const cv::Size frameSize) {
	std::vector<cv::Mat> result;

	int length = src.cols;
	for(int i = 0; i < length; i++) {
		cv::Mat line = src.col(i).clone();
		cv::Mat reshaped = line.reshape(3, frameSize.height).clone();
		result.push_back(reshaped);
	}

	return result;
}

cv::Mat ColorMagnify::amplify(const cv::Mat src, const float magnify_coeff) {
	return src * magnify_coeff;
}