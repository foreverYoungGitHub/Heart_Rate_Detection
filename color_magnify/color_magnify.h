#ifndef COLORMODIFY_H
#define COLORMODIFY_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>


class ColorMagnify {
public:
    ColorMagnify() {}
    ~ColorMagnify() {}

    /**
     * ColorMagnify() faction is used to magnify the color of the face to detect heart rate
     *
     * @param src           : the input frames
     * @param dst           : the output frames with the magnified color
     * @param fps           : the frame per second of the video
     * @param magnify_coeff : the coefficient of the magnify
     * @param low_freq      : the low frequence cut-off. In face, it would better be set as 0.83
     * @param high_freq     : the high frequence cut-off. In face, it would better be set as 1.0
     * @param pyramid_level : the pyramid level used for down sampling
     */
    std::vector<cv::Mat> get_combined_img(std::vector<cv::Mat>src, int fps = 30, float magnify_alpha = 50.f,
                  double low_freq = 0.83f, double high_freq = 1.0f, int pyramid_level = 4);

    /**
     * ColorMagnify() faction is used to magnify the color of the face to detect heart rate
     *
     * @param src           : the input frames
     * @param fps           : the frame per second of the video
     * @param magnify_coeff : the coefficient of the magnify
     * @param low_freq      : the low frequence cut-off. In face, it would better be set as 0.83
     * @param high_freq     : the high frequence cut-off. In face, it would better be set as 1.0
     * @param pyramid_level : the pyramid level used for down sampling
     * @return              : return the filtered image
     */
    cv::Mat get_filtered_img(std::vector<cv::Mat>src, int fps = 30, float magnify_alpha = 50.f,
                             double low_freq = 0.83f, double high_freq = 1.0f, int pyramid_level = 4);

    /**
     * buildGaussianPyramid() is used to build a gaussian pyramid, which is always used in color magnify
     *
     * @param img           : the input frames
     * @param pyramid_level : the pyramid level used for down sampling
     * @return              : output the frame with down sampling
     */
	cv::Mat buildGaussianPyramid(const cv::Mat &img, const int pyramid_level);


    /**
     * upsamplingFromGaussianPyramid() is used to up-sampling an image from gaussian pyramid
     * @param src           : the input frames
     * @param pyramid_level : the pyramid level used for up sampling, which is same to down sampling
     * @return              : output the group of image get from
     */
    cv::Mat upsamplingFromGaussianPyramid(const cv::Mat src, const int pyramid_level);


    /**
     * concat() functin is used to concat images into a large Mat, which allows it to pass the dft and filter
     *
     * @param frames        : the input frames
     * @return              : output the concated image
     */
	cv::Mat concat(const std::vector<cv::Mat> frames);


    /**
     * deConcat() is used to de-concat the concatnate image into frames
     *
     * @param src           : source concatnate image
     * @param frameSize     : frame size
     * @return              : destination frames
     */
    std::vector<cv::Mat> deConcat(const cv::Mat src, const cv::Size frameSize);


    /**
     * temporalIdealFilter() is used to temporal IIR filtering an image pyramid of concat-frames
     *
     * @param src           : source pyramid of concatenate frames
     * @param fl            : low frequence cut-off
     * @param fh            : high frequence cut-off
     * @param fps           : the frame per second of the video
     * @return              : output concatenate filtered result
     */
    cv::Mat temporalIdealFilter(const cv::Mat &src, double fl, double fh, double fps);


    /**
     * createIdealBandpassFilter() is used to create a 1D ideal band-pass filter
     * @param filter        : source image to be filtered
     * @param fl            : low frequence cut-off
     * @param fh            : high frequence cut-off
     * @param fps           : the frame per second of the video
     */
	void createIdealBandpassFilter(cv::Mat &filter, double fl, double fh, double fps);


    /**
     * amplify() is used to amplify color
     *
     * @param src           : source filtered image
     * @param magnify_coeff : the coefficient of the magnify
     * @return              : return the amplified image
     */
	cv::Mat amplify(const cv::Mat src, const float magnify_coeff);
};
#endif //COLORMODIFY_H
