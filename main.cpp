#include "color_magnify/color_magnify.h"
#include "MTCNN/MTCNN.h"
#include "skcf/ktrackers.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;


/**
 * test main for color magnification
 * @return
 */
//int main() {
//
//	VideoCapture video("result/face.mp4");
//	video.set(CV_CAP_PROP_CONVERT_RGB, true);
//	int fps = video.get(CV_CAP_PROP_FPS);
//    vector<Mat> faces;
//    faces.resize(fps);
//
//    /**
//     * if you want to see the whole video
//     */
////    int frame_number = video.get(CV_CAP_PROP_FRAME_COUNT);
////    long frameToStart = 1;
////    video.set(CV_CAP_PROP_POS_FRAMES, frameToStart);
////	vector<Mat> faces;
////	faces.resize(frame_number-1);
//
//	for (auto & face : faces) {
//		video >> face;
//	}
//
//	ColorMagnify color_magnify;
//
//
//    /**
//     * get the filtered image
//     * the filtered result is a CV_32FC3 format
//     */
//    auto filtered = color_magnify.get_filtered_img(faces);
//    filtered.resize(100),
//    imshow("filtered", filtered);
//    waitKey(1000);
//
//    /**
//     * get the combined image
//     * the combined result is CV_8UC3
//     */
//    auto combined_imgs = color_magnify.get_combined_img(faces);
//
//    for(auto img : combined_imgs) {
//        imshow("combined imgs", img);
//        waitKey(33);
//    }
//
////    for (int i = 0; i < combined_imgs.size(); i++) {
////		char image_name[50];
////		sprintf(image_name, "%s%d%s", "result/", i, ".jpg");
////		imwrite(image_name, combined_imgs[i]);
////	}
//
//	return 0;
//}



/**
 * test main for MTCNN
 * @return
 */
//int main() {
//
//    MTCNN mtcnn;
//
//    VideoCapture cap(0);
//    Mat img;
//    int frame_count = 0;
//    while(cap.read(img))
//    {
//        vector<Rect> rectangles;
//        vector<float> confidences;
//        std::vector<std::vector<cv::Point>> alignment;
//        mtcnn.detection(img, rectangles, confidences, alignment);
//
//        for(int i = 0; i < rectangles.size(); i++)
//        {
//            int green = confidences[i] * 255;
//            int red = (1 - confidences[i]) * 255;
//            rectangle(img, rectangles[i], cv::Scalar(0, green, red), 3);
//            for(int j = 0; j < alignment[i].size(); j++)
//            {
//                cv::circle(img, alignment[i][j], 5, cv::Scalar(255, 255, 0), 3);
//            }
//        }
//
//        frame_count++;
//        cv::putText(img, std::to_string(frame_count), cvPoint(3, 13),
//                    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 255, 0), 1, CV_AA);
//        imshow("Live", img);
//        waitKey(1);
//    }
//
//    return 0;
//}


/**
 * test main for MTCNN and skcf
 * @return
 */

int main() {

    MTCNN mtcnn;


    VideoCapture cap(0);
	int fps = cap.get(CV_CAP_PROP_FPS);

    Mat img;

    vector<std::shared_ptr<KTrackers>> kcfs;

    float scale_factor = 0.15;

    int frame_count = 0;
    while(cap.read(img))
    {
        if(frame_count % fps == 0 || kcfs.size() == 0){
            vector<Rect> rectangles;
            rectangles.clear();
            kcfs.clear();
            mtcnn.detection(img, rectangles);

            for(auto rect : rectangles) {
                rect = cv::Rect((rect.x - rect.width * scale_factor * 0.2) * scale_factor, 
                                (rect.y - rect.height * scale_factor * 0.2) * scale_factor,
                                rect.width * scale_factor * 1.4, rect.height * scale_factor * 1.4);
                auto skcf = std::make_shared<KTrackers>(false);
                skcf->set_area(rect);
                kcfs.push_back(skcf);
            }
        }

        cv::Mat resized;
        resize(img, resized, cv::Size(0, 0), scale_factor, scale_factor);

        double time_profile_counter = cv::getCPUTickCount();
        for(auto skcf : kcfs) {
            auto rect = skcf->get_area(resized);
            rect = cv::Rect(rect.x / scale_factor, rect.y / scale_factor, rect.width / scale_factor, rect.height / scale_factor);
            cv::rectangle(img, rect, cv::Scalar(0, 255, 0), 3);
        }
        time_profile_counter = cv::getCPUTickCount() - time_profile_counter;
        std::cout << "  -> speed : " <<  time_profile_counter/((double)cvGetTickFrequency()*1000) << "ms. per frame" << std::endl;

        frame_count++;
        cv::putText(img, std::to_string(frame_count), cvPoint(3, 13),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 255, 0), 1, CV_AA);
        imshow("Live", img);
        waitKey(1);
    }

    return 0;
}