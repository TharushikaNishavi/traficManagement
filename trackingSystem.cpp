#include "trackingSystem.h"



trackingSystem::trackingSystem()
{
	window = cv::imread("E:\\C++\\OpenCV\\trackinSoftware\\image\\window1.jpg", CV_LOAD_IMAGE_COLOR);
	cap.open("C:\\Users\\Sandeepana\\Desktop\\222.mp4");

	if (!cap.isOpened()) {
		cout << "error" << endl;
		system("pause");
	}

}




void trackingSystem::process() {

	countEachline = new int[size / 2 - 1];

	cv::Point currentCenter;
	cv::Mat maskdilate = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
	cv::Scalar color = cv::Scalar(255, 255, 255);
	cv::Rect objectBoundingRectangle = cv::Rect(0, 0, 0, 0);

	cv::namedWindow(WINDOW_NAME, cv::WINDOW_AUTOSIZE);

	//save video
	cv::VideoWriter video("track4.avi", CV_FOURCC('M', 'J', 'P', 'G'), 5, cv::Size(800, 600), true);

	bool blnFirstFrame = true;


	while (1) {
		vector<Blob> currentFrameBlobs;
		cap >> frame1;
		cap >> frame2;

		//color change the video
		/*for (int r = 0; r < frame1.rows; r++) {
			for (int c = 0; c < frame1.cols; c++) {
				frame1.at<cv::Vec3b>(r, c)[2] = frame1.at<cv::Vec3b>(r, c)[2] * 0;
				frame1.at<cv::Vec3b>(r, c)[0] = frame1.at<cv::Vec3b>(r, c)[0] * 0;
			}
		}


		for (int r = 0; r < frame2.rows; r++) {
			for (int c = 0; c < frame2.cols; c++) {
				frame2.at<cv::Vec3b>(r, c)[2] = frame2.at<cv::Vec3b>(r, c)[2] * 0;
				frame2.at<cv::Vec3b>(r, c)[0] = frame2.at<cv::Vec3b>(r, c)[0] * 0;
			}
		}*/

		cv::cvtColor(frame1, gray1, cv::COLOR_BGR2GRAY);
		cv::cvtColor(frame2, gray2, cv::COLOR_BGR2GRAY);
		cv::GaussianBlur(gray1, gray1, cv::Size(5, 5), 5);
		cv::GaussianBlur(gray2, gray2, cv::Size(5, 5), 5);
		cv::absdiff(gray1, gray2, diff);
		cv::threshold(diff, thres, 20, 255, cv::THRESH_BINARY);
		//cv::imshow("threshold", thres);

		cv::dilate(thres, thres, maskdilate);
		cv::dilate(thres, thres, maskdilate);
		cv::erode(thres, thres, maskdilate);
		cv::dilate(thres, thres, maskdilate);
		//dilate(thres, thres, maskdilate);
		cv::erode(thres, thres, maskdilate);
		//cv::imshow("dilate", thres);
		
		vector< vector<cv::Point> > contours;
		vector<vector<cv::Point> > lastContours;
		vector<cv::Vec4i> hierarchy;
		findContours(thres, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		vector< vector<cv::Point> > hull(contours.size());
		for (int i = 0; i < contours.size(); i++) {
			convexHull(cv::Mat(contours[i]), hull[i], false);
		}

		for (int i = 0; i < hull.size(); i++) {
			Blob checker;

			checker.currentContour = hull[i];
			checker.currentBoundingRect = boundingRect(checker.currentContour);
			checker.dblCurrentDiagonalSize = sqrt(pow(checker.currentBoundingRect.width, 2) + pow(checker.currentBoundingRect.height, 2));
			checker.dblCurrentAspectRatio = (float)checker.currentBoundingRect.width / (float)checker.currentBoundingRect.height;

			checker.blnCurrentMatchFoundOrNewBlob = true;

			checker.intNumOfConsecutiveFramesWithoutAMatch = 0;

			currentCenter.x = (checker.currentBoundingRect.x + checker.currentBoundingRect.x + checker.currentBoundingRect.width) / 2;
			currentCenter.y = (checker.currentBoundingRect.y + checker.currentBoundingRect.y + checker.currentBoundingRect.height) / 2;

			checker.centerPositions.push_back(currentCenter);

			if (currentCenter.y > 200) {

				if (checker.currentBoundingRect.area() > 400 &&
					checker.dblCurrentAspectRatio > 0.2 &&
					checker.dblCurrentAspectRatio < 4.0 &&
					checker.currentBoundingRect.width > 30 &&
					checker.currentBoundingRect.height > 30 &&
					checker.dblCurrentDiagonalSize > 60.0 &&
					(contourArea(checker.currentContour) / (double)checker.currentBoundingRect.area()) > 0.50) 
				{

						lastContours.push_back(checker.currentContour);
						currentFrameBlobs.push_back(checker);

				}
			}
			else {
				if (checker.currentBoundingRect.area() > 150 &&
					checker.dblCurrentAspectRatio > 0.2 &&
					checker.dblCurrentAspectRatio < 4.0 &&
					checker.currentBoundingRect.width > 15 &&
					checker.currentBoundingRect.height > 15 &&
					checker.dblCurrentDiagonalSize > 30.0 &&
					(contourArea(checker.currentContour) / (double)checker.currentBoundingRect.area()) > 0.50)
				{
					
						lastContours.push_back(checker.currentContour);
						currentFrameBlobs.push_back(checker);

				}
			}
		}



		if (blnFirstFrame) {
			for (auto &currentFrameBlob : currentFrameBlobs) {
				blobs.push_back(currentFrameBlob);
			}
		}
		else {

			matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);					// compare current frame and previouse frame
		}

		cv::Mat frame2Copy = frame2.clone();

		drawBlobInfoOnImage(blobs, frame2Copy);
		counting(blobs);

		cv::resize(frame2Copy, frame2Copy, cv::Size(), fx, fy, cv::INTER_AREA);
		
		frame2Copy.copyTo(window(cv::Rect(0, 50, 800, 550)));


		//cv::imshow(WINDOW_NAME, window);
		
		//save video
		video.write(window);

		cv::waitKey(10);

		frame1 = frame2.clone();
		blnFirstFrame = false;
		//end of the video
		if ((cap.get(CV_CAP_PROP_POS_FRAMES) + 1) < cap.get(CV_CAP_PROP_FRAME_COUNT)) {
			cap.read(frame2);
		}
		else {
			cv::destroyAllWindows();
			video.release();
			cout << "end of video\n";
			break;
		}

	}
	system("pause");


}

int trackingSystem::check(int x, int y) {

	for (int i = 0; i < size - 2; i += 2) {
		int x2 = line[i].x - ( line[i].x * y ) / line[i + 1].x;
		int x3 = line[i + 2].x - (line[i + 2].x * y) / line[i + 3].x;
		if (x2 > x && x > x3) {
			return i / 2;
		}
	}
	return -1;
}


void trackingSystem::matchCurrentFrameBlobsToExistingBlobs(vector<Blob> &existingBlobs, vector<Blob> &currentFrameBlobs) {

	for (auto &existingBlob : existingBlobs) {					// loop to get all current blob and predict next blob position

		existingBlob.blnCurrentMatchFoundOrNewBlob = false;
		existingBlob.predictedNextPosition = predictNextPosition(existingBlob.centerPositions);
	}

	for (auto &currentFrameBlob : currentFrameBlobs) {			// loop each Blob object (current frame) and check distance with predict of exiting blob

		int intIndexOfLeastDistance = 0;
		double dblLeastDistance = 100000.0;						// minimum distance betweent actual current Blob (current framce) compare with predict Blob

		for (unsigned int i = 0; i < existingBlobs.size(); i++) {

			double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

			if (dblDistance < dblLeastDistance) {			// if distance between current frame Blob to existing blob is mimimum the count it as correct predicted of existing Blob
				dblLeastDistance = dblDistance;
				intIndexOfLeastDistance = i;
			}

		}

		if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.5) {					// if the close distance of existing blob and current blob < current blob size 
			addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);	// add blob over existing blob 
		}
		else {
			addNewBlob(currentFrameBlob, existingBlobs);
		}

	}

	for (int i = 0; i < existingBlobs.size(); i++) {

		if (!existingBlobs[i].blnCurrentMatchFoundOrNewBlob) {
			existingBlobs[i].intNumOfConsecutiveFramesWithoutAMatch++;
		}

		if (existingBlobs[i].intNumOfConsecutiveFramesWithoutAMatch >= 5) {
			existingBlobs.erase(existingBlobs.begin() + i);
		}

	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
cv::Point trackingSystem::predictNextPosition(vector<cv::Point> &centerPositions) {				// this method will get x,y Point of predict position 
	cv::Point  predictedNextPosition;
	int numPositions = (int)centerPositions.size();

	if (numPositions == 1) {

		predictedNextPosition.x = centerPositions.back().x;
		predictedNextPosition.y = centerPositions.back().y;

	}
	else if (numPositions == 2) {

		int deltaX = centerPositions[1].x - centerPositions[0].x;
		int deltaY = centerPositions[1].y - centerPositions[0].y;

		predictedNextPosition.x = centerPositions.back().x + deltaX;
		predictedNextPosition.y = centerPositions.back().y + deltaY;

	}
	else if (numPositions == 3) {

		int sumOfXChanges = ((centerPositions[2].x - centerPositions[1].x) * 2) +
			((centerPositions[1].x - centerPositions[0].x) * 1);

		int deltaX = (int)round((float)sumOfXChanges / 3.0);

		int sumOfYChanges = ((centerPositions[2].y - centerPositions[1].y) * 2) +
			((centerPositions[1].y - centerPositions[0].y) * 1);

		int deltaY = (int)round((float)sumOfYChanges / 3.0);

		predictedNextPosition.x = centerPositions.back().x + deltaX;
		predictedNextPosition.y = centerPositions.back().y + deltaY;

	}
	else if (numPositions == 4) {

		int sumOfXChanges = ((centerPositions[3].x - centerPositions[2].x) * 3) +
			((centerPositions[2].x - centerPositions[1].x) * 2) +
			((centerPositions[1].x - centerPositions[0].x) * 1);

		int deltaX = (int)round((float)sumOfXChanges / 6.0);

		int sumOfYChanges = ((centerPositions[3].y - centerPositions[2].y) * 3) +
			((centerPositions[2].y - centerPositions[1].y) * 2) +
			((centerPositions[1].y - centerPositions[0].y) * 1);

		int deltaY = (int)round((float)sumOfYChanges / 6.0);

		predictedNextPosition.x = centerPositions.back().x + deltaX;
		predictedNextPosition.y = centerPositions.back().y + deltaY;

	}
	else if (numPositions >= 5) {

		int sumOfXChanges = ((centerPositions[numPositions - 1].x - centerPositions[numPositions - 2].x) * 4) +
			((centerPositions[numPositions - 2].x - centerPositions[numPositions - 3].x) * 3) +
			((centerPositions[numPositions - 3].x - centerPositions[numPositions - 4].x) * 2) +
			((centerPositions[numPositions - 4].x - centerPositions[numPositions - 5].x) * 1);

		int deltaX = (int)round((float)sumOfXChanges / 10.0);

		int sumOfYChanges = ((centerPositions[numPositions - 1].y - centerPositions[numPositions - 2].y) * 4) +
			((centerPositions[numPositions - 2].y - centerPositions[numPositions - 3].y) * 3) +
			((centerPositions[numPositions - 3].y - centerPositions[numPositions - 4].y) * 2) +
			((centerPositions[numPositions - 4].y - centerPositions[numPositions - 5].y) * 1);

		int deltaY = (int)round((float)sumOfYChanges / 10.0);

		predictedNextPosition.x = centerPositions.back().x + deltaX;
		predictedNextPosition.y = centerPositions.back().y + deltaY;

	}
	else {
		// should never get here
	}

	return predictedNextPosition;

}






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double trackingSystem::distanceBetweenPoints(cv::Point point1, cv::Point point2) {

	int intX = abs(point1.x - point2.x);
	int intY = abs(point1.y - point2.y);

	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void trackingSystem::addBlobToExistingBlobs(Blob &currentFrameBlob, vector<Blob> &existingBlobs, int &intIndex) {

	existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
	existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;

	existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());

	existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
	existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;

	existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void trackingSystem::addNewBlob(Blob &currentFrameBlob, vector<Blob> &existingBlobs) {

	currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;

	existingBlobs.push_back(currentFrameBlob);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void trackingSystem::drawBlobInfoOnImage(vector<Blob> &blobs, cv::Mat &imgFrame2Copy) {

	for (unsigned int i = 0; i < blobs.size(); i++) {
		if (blobs[i].blnCurrentMatchFoundOrNewBlob)
			rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, cv::Scalar(255, 0, 0), 2);

		for (int i = 0; i < size; i += 2) {
			cv::line(imgFrame2Copy, line[i], line[i+1], cv::Scalar(255, 255, 255), 4);
		}

		cv::line(imgFrame2Copy, line[0], line[size - 2], cv::Scalar(255, 255, 255), 4);
		cv::line(imgFrame2Copy, line[1], line[size - 1], cv::Scalar(255, 255, 255), 4);

		//string text = "Total number of cars = %d ", numberOfCars;

		//cv::displayOverlay(WINDOW_NAME, "total cars",0 );
		putText(imgFrame2Copy, to_string(numberOfCars), cv::Point(50, 50), CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 255, 255), 2);

	}


}

void trackingSystem::counting(vector<Blob> &existingBlobs) {
	int size;
	for (auto existingBlob : existingBlobs) {
		size = existingBlob.centerPositions.size();
		if (size > 1) {
			if (existingBlob.centerPositions[size - 2].y > trackingYcoordinates[1] &&
				existingBlob.centerPositions[size - 1].y < trackingYcoordinates[1]) {

				numberOfCars++;
				numberOfCarsInArea++;

				int num = check(existingBlob.centerPositions[size - 1].x, existingBlob.centerPositions[size - 1].y);
				countEachline[num]++;


			}
			else if (existingBlob.centerPositions[size - 2].y > trackingYcoordinates[0] &&
				existingBlob.centerPositions[size - 1].y < trackingYcoordinates[0]) {

				numberOfCarsInArea--;
				int num = check(existingBlob.centerPositions[size - 2].x, existingBlob.centerPositions[size - 2].y);
				countEachline[num]++;

			}
		}
		
	}
}