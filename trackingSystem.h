#pragma once


#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <iostream>
#include <conio.h>
#include <math.h>
#include <string>

using namespace std;

#define WINDOW_NAME "Tracker"

struct Blob {

	vector<cv::Point> currentContour;
	cv::Rect currentBoundingRect;
	vector<cv::Point> centerPositions;

	double dblCurrentDiagonalSize;
	double dblCurrentAspectRatio;

	bool blnCurrentMatchFoundOrNewBlob;

	int intNumOfConsecutiveFramesWithoutAMatch;

	cv::Point predictedNextPosition;

};


class trackingSystem
{

public:

/*************************     public function      *****************************/
	trackingSystem();
	void process();
	
/*************************     public variable      *****************************/

	float fx, fy;
	int trackingYcoordinates[2];
	int size;
	cv::Point *line;


private:

/*************************     pprivat function      *****************************/

	int check(int x, int y);
	void matchCurrentFrameBlobsToExistingBlobs(vector<Blob> &existingBlobs, vector<Blob> &currentFrameBlobs);
	double distanceBetweenPoints(cv::Point point1, cv::Point point2);
	cv::Point predictNextPosition(vector<cv::Point> &centerPositions);
	void counting(vector<Blob> &existingBlobs);
	void addBlobToExistingBlobs(Blob &currentFrameBlob, vector<Blob> &existingBlobs, int &intIndex);
	void addNewBlob(Blob &currentFrameBlob, vector<Blob> &existingBlobs);
	void drawBlobInfoOnImage(vector<Blob> &blobs, cv::Mat &imgFrame2Copy);

/*************************     privat variable      *****************************/

	cv::Mat frame1, frame2, gray1, gray2;
	cv::Mat diff, thres;
	cv::Mat window;
	int numberOfCars = 0;
	int numberOfCarsInArea = 0;
	vector<Blob> blobs;
	cv::VideoCapture cap;
	int *countEachline;

};