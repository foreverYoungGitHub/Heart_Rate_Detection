#include "color_magnify/color_magnify.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

int main() {

	VideoCapture video("result/face.mp4");
	video.set(CV_CAP_PROP_CONVERT_RGB, true);
	int fps = video.get(CV_CAP_PROP_FPS);
    vector<Mat> faces;
    faces.resize(fps);

    /**
     * if you want to see the whole video
     */
//    int frame_number = video.get(CV_CAP_PROP_FRAME_COUNT);
//    long frameToStart = 1;
//    video.set(CV_CAP_PROP_POS_FRAMES, frameToStart);
//	vector<Mat> faces;
//	faces.resize(frame_number-1);

	for (auto & face : faces) {
		video >> face;
	}

	ColorMagnify color_magnify;


    /**
     * get the filtered image
     * the filtered result is a CV_32FC3 format
     */
    auto filtered = color_magnify.get_filtered_img(faces);
    filtered.resize(50),
    imshow("filtered", filtered);
    waitKey(1000);

    /**
     * get the combined image
     * the combined result is CV_8UC3
     */
    auto combined_imgs = color_magnify.get_combined_img(faces);

    for(auto img : combined_imgs) {
        imshow("combined imgs", img);
        waitKey(33);
    }

//    for (int i = 0; i < combined_imgs.size(); i++) {
//		char image_name[50];
//		sprintf(image_name, "%s%d%s", "result/", i, ".jpg");
//		imwrite(image_name, combined_imgs[i]);
//	}

	return 0;
}
