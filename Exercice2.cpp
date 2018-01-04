﻿//#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/ml.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string> 
#include "MyForest.h"
#include "hog_visualization.cpp"

///set opencv and c++ namespaces
using namespace cv::ml;
using namespace cv;
using namespace std;

int main()
{
	///Variables(careful, not all of them)
	Mat src, src_gray, src_gray_resized;
	Mat grad;
	Mat features;
	Mat labels;
	Mat test;
	char* window_name = "HOG Descriptor";
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	vector<float> descriptors;

	/// Number of pictures
	///Final values
	int number_of_cat = 6;
	int nbr_pictures_cat[] = { 49, 66, 42, 53, 67, 110 };
	///Temp values
	//int number_of_cat = 2;
	//int nbr_pictures_cat[] = { 2, 2};
	/// Beginning and end of path to pictures
	string schemetrain = "C:/Users/Kamel GUERDA/Desktop/TDCV_exercice2/data/task2/train/";
	string schemetest = "C:/Users/Kamel GUERDA/Desktop/TDCV_exercice2/data/task2/test/";
	string extension = ".jpg";

	///get in right category/fold
	int image_cat;
	int image_index;
	string s_image_cat;
	string s_image_index;
	string path;

	///Create DTrees
	Ptr<DTrees> myDTree[5];// test succeeded, possible to instantiate multiple DTrees
	myDTree[0] = DTrees::create();

	///Set some parameters of the 1st DTree	
	myDTree[0]->setCVFolds(0); // the number of cross-validation folds
	myDTree[0]->setMaxDepth(8);
	myDTree[0]->setMinSampleCount(2);
	//

	///Get HOG descriptor of each picture and create two Mat (one with descriptors, second with labels)
	for (image_cat = 0; image_cat < number_of_cat; image_cat = image_cat+ 1) {
		for (image_index = 0; image_index < nbr_pictures_cat[image_cat]; image_index = image_index + 1) {

			///get 
			s_image_cat = to_string(image_cat);
			while (s_image_cat.length() <2) {
				s_image_cat = "0" + s_image_cat;
			}

			///get a specific image
			s_image_index = to_string(image_index);
			while (s_image_index.length() <4) {
				s_image_index = "0" + s_image_index;
			}
			///create complete path for one specific image
			path = schemetrain  + s_image_cat + "/" + s_image_index + extension;
			///example path="C:/Users/Kamel GUERDA/Desktop/TDCV_exercice2/data/task2/train/00/0000.jpg";
			src = imread(path);

			///check if picture exist
			if (!src.data)
			{
				return -1;
			}

			///Apply some blur 
			//GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);
			/// Convert it to gray
			cv::cvtColor(src, src_gray, CV_BGR2GRAY);
			
			///Scale to 120*120 image
			cv::resize(src_gray, src_gray_resized, Size(120, 120), 0, 0, INTER_AREA);

			HOGDescriptor hog(
				src_gray_resized.size(),//Size(20, 20), //winSize
				Size(20, 20), //blocksize
				Size(10, 10), //blockStride,
				Size(10, 10), //cellSize,
				9, //nbins,
				1, //derivAper,
				-1, //winSigma,
				0, //histogramNormType,
				0.2, //L2HysThresh,
				0,//gammal correction,
				64,//nlevels=64
				1);

			hog.compute(src_gray_resized, descriptors, Size(136, 136), Size(8, 8));
			//visualizeHOG(src_gray_resized, descriptors, hog, 6);
			//waitKey(0);
			Mat1f m1(1, descriptors.size(), descriptors.data());
			features.push_back(m1);
			labels.push_back(image_cat);
		}
	}
	
	///Train the DTree with all the data we have
	myDTree[0]->train(ml::TrainData::create(features, ml::ROW_SAMPLE, labels));


	///Test a picture
	///

	path = schemetest + "01" + "/" + "0071" + extension;
	///example path="C:/Users/Kamel GUERDA/Desktop/TDCV_exercice2/data/task2/train/00/0000.jpg";
	src = imread(path);

	///check if picture exist
	if (!src.data)
	{
		return -1;
	}
	/// Convert it to gray
	cv::cvtColor(src, src_gray, CV_BGR2GRAY);
	///Scale to 120*120 image
	cv::resize(src_gray, src_gray_resized, Size(120, 120), 0, 0, INTER_AREA);


	HOGDescriptor hog(
		src_gray_resized.size(),//Size(20, 20), //winSize
		Size(20, 20), //blocksize
		Size(10, 10), //blockStride,
		Size(10, 10), //cellSize,
		9, //nbins,
		1, //derivAper,
		-1, //winSigma,
		0, //histogramNormType,
		0.2, //L2HysThresh,
		0,//gammal correction,
		64,//nlevels=64
		1);
	hog.compute(src_gray_resized, descriptors, Size(136, 136), Size(8, 8));
	Mat1f m2(1, descriptors.size(), descriptors.data());
	test.push_back(m2);

	///Prediction from one tree for one picture
	int prediction;
	prediction = myDTree[0]->predict(test);

	MyForest testForest;
	testForest.create(20);
	testForest.train(features,labels,20);
	int results;
	results = testForest.predict(descriptors);
	//int classificationForest = results[0];

	//int percentageForest = results[1];
	return 0;

}	
