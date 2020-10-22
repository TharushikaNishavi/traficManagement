#include "trackingSystem.h"

#define CVUI_IMPLEMENTATION

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "cvui.h"
#include <iostream>
#include <Windows.h>
#include <stdlib.h>

#define WINDOW_NAME "Tracker"

using namespace std;
using namespace cv;

trackingSystem trackingObject;

Mat creatTwotrackingPoint(Mat window);
Mat selct_lines(Mat window);
int coordinate[2];

int row = 466;
int colum = 932;


int main() {

	namedWindow(WINDOW_NAME, WINDOW_AUTOSIZE);
	cvui::init(WINDOW_NAME);

	

	Mat frame;
	Mat window;
	window = imread("E:\\C++\\OpenCV\\trackinSoftware\\image\\window1.jpg",CV_LOAD_IMAGE_COLOR);

	VideoCapture cap("C:\\Users\\Sandeepana\\Desktop\\222.mp4");

	if (!cap.isOpened()) {
		cout << "error" << endl;
		system("pause");
	}

	cap >> frame;

	trackingObject.fx = 800.0 / colum;
	trackingObject.fy = 550.0 / row;

	resize(frame, frame, Size(), trackingObject.fx, trackingObject.fy, INTER_AREA);
	frame.copyTo(window(Rect(0, 50, 800, 550)));

	bool tracked = false;

	while (1) {

		frame = window.clone();

		if (cvui::button(window, 10, 15, "Play")) {

			cout << "open" << endl;
			trackingObject.process();

		}
		if (cvui::button(window, 90, 15, "Tracker ")) {

			cout << "Tracker" << endl;
			window = creatTwotrackingPoint(window);
			tracked = true;

		}
		if (cvui::button(window, 190, 15, "Lines")) {

			cout << "Lines" << endl;
			if(tracked)
				selct_lines(window);

		}

		cvui::update();
		imshow(WINDOW_NAME, window);
		waitKey(1);

	}


	return 0;
}


Mat creatTwotrackingPoint(Mat window){

	bool first = false;
	Point position;
	Mat frame;

	while (1) {
		frame = window.clone();

		int state = cvui::iarea(50, 50, 800, 550);

		if (state == cvui::CLICK) {
			position = cvui::mouse();

			if (first && trackingObject.trackingYcoordinates[0] != position.y) {
				
				coordinate[1] = position.y;
				trackingObject.trackingYcoordinates[1] = ((position.y - 50)*row) / 550;
				line(window, Point(0, position.y), Point(800, position.y), Scalar(0, 0, 255), 2);
				break;
			}
			else {

				coordinate[0] = position.y;
				trackingObject.trackingYcoordinates[0] = ((position.y - 50)*row) / 550;
				line(window, Point(0, position.y), Point(800, position.y), Scalar(0, 255, 0), 2);
			}

			frame = window.clone();
			first = true;
		}
		else if (state == cvui::OVER) {
			position = cvui::mouse();
			line(frame, Point(0, position.y), Point(800, position.y), Scalar(255, 255, 255), 2);
		}

		cvui::update();
		imshow(WINDOW_NAME, frame);
		waitKey(1);

	}

	return window;

}

Mat selct_lines(Mat window) {
	Mat frame = window.clone();
	int count = 2;
	while (1) {
		Mat frame = window.clone();
		cvui::counter(frame, 400, 300, &count);
		if (cvui::button(frame, 400, 350, " OK ")) {
			break;
		}

		cvui::update();
		imshow(WINDOW_NAME, frame);
		waitKey(1);
	}

	count = (count + 1) * 2;
	cout << count << endl;
	trackingObject.line = new Point[count ];
	trackingObject.size = count;
	Point position;
	Point *lines = new Point[count];

	for (int i = 0; i <= count;) {
		
		frame = window.clone();
		int state = cvui::iarea(50, 50, 800, 550);

		if (state == cvui::OVER) {

			position = cvui::mouse();
			position = Point(position.x, coordinate[i % 2]);
			circle(frame, position, 6, Scalar(0, 255, 0), 2);

		}
		if (state == cvui::CLICK ) {

			position = cvui::mouse();
			position = Point(position.x, coordinate[i % 2] );

			if (i > 0) {
				if (trackingObject.line[i - 2] != trackingObject.line[i - 1]) {
	
					circle(window, position, 6, Scalar(0, 255, 0), 2);
					lines[i - 1] = position;
					position = Point( (position.x*colum)/800, ((position.y-50)*row)/550 );
					trackingObject.line[i - 1] = position;
					i++;

					if (i % 2 == 1 ) {
						line(window, lines[i - 3], lines[i - 2], Scalar(255, 255, 255), 2);
					}

				}

			}
			else {
				i++;
			}

		}

		cvui::update();
		imshow(WINDOW_NAME, frame);
		waitKey(1);

	}

	return window;
}