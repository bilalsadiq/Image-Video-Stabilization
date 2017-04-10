#include <sstream>
#include <string>
#include <iostream>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

using namespace cv;

//Default Width and Height of the Frame in which we will be capturing 640X480
const int widthFrame = 640;
const int heightFrame = 480;


//Initialize the Maximum number of Objects that can be detected in the frame.
const int maxNumberOfDetObjects = 50;


// We will initialize minimum and maximum values for the HSV filter.
//We are going to change these values using the track bars
int minimum_H = 0;
int minimum_S = 0;
int minimum_V = 0;
int maximum_H = 256;
int maximum_S = 256;
int maximum_V = 256;




//THe minimum and Maximum Areas of the Object.

const int minAreaOfObj = 20 * 20;
const int maxAreaOfObj = heightFrame*widthFrame / 1.5;





//Titles of the window that will appear at the Top of it.


const string Title_Window = "Original Image";
const string Title_Window1 = "HSV Image";
const string Title_Window2 = "Thresholded Image";
const string Title_Window3 = "After Morphological Operations";
const string trackbarTitle_Window = "Track Bars";

string intStrConverter(int number) {


	std::stringstream ss;
	ss << number;
	return ss.str();
}

//This function will get called whenever a track bar position is changed

void on_trackbar(int, void*)
{




}

//end on_TrackBar Function

void createTrackbars() {
	//create window for trackbars


	namedWindow(trackbarTitle_Window, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	//sprintf_s(TrackbarName, "minimum_H", minimum_H);
	//sprintf_s(TrackbarName, "maximum_H", maximum_H);
	//sprintf_s(TrackbarName, "minimum_S", minimum_S);
	//sprintf_s(TrackbarName, "maximum_S", maximum_S);
	//sprintf_s(TrackbarName, "minimum_V", minimum_V);
	//sprintf_s(TrackbarName, "maximum_V", maximum_V);


	/*
	We are creating track bars and inserting them into a window
	The 3 parameters are as followed:
	1) The address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	2) The max value the track bar can move (eg. H_HIGH),
	3) The function that is called whenever the track bar is moved (on_trackbar)
	*/

	createTrackbar("minimum_H", trackbarTitle_Window, &minimum_H, maximum_H, on_trackbar);
	createTrackbar("maximum_H", trackbarTitle_Window, &maximum_H, maximum_H, on_trackbar);
	createTrackbar("minimum_S", trackbarTitle_Window, &minimum_S, maximum_S, on_trackbar);
	createTrackbar("maximum_S", trackbarTitle_Window, &maximum_S, maximum_S, on_trackbar);
	createTrackbar("minimum_V", trackbarTitle_Window, &minimum_V, maximum_V, on_trackbar);
	createTrackbar("maximum_V", trackbarTitle_Window, &maximum_V, maximum_V, on_trackbar);


}





//We use drawing functions in this function to track our object
void TrackObject(int x, int y, Mat &frame) {


	circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2);
	if (y - 25>0)
		line(frame, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, 0), Scalar(0, 255, 0), 2);
	if (y + 25<heightFrame)
		line(frame, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, heightFrame), Scalar(0, 255, 0), 2);
	if (x - 25>0)
		line(frame, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(0, y), Scalar(0, 255, 0), 2);
	if (x + 25<widthFrame)
		line(frame, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(widthFrame, y), Scalar(0, 255, 0), 2);

	putText(frame, intStrConverter(x) + "," + intStrConverter(y), Point(x, y + 30), 1, 1, Scalar(0, 255, 0), 2);

}





//This function will create structuring element that will be used to "dilate" and "erode" image. The element chosen here is a 3px by 3px rectangle


void morphOps(Mat &thresh) {


	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));





	//make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);


	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);



}


void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed) {

	Mat temp;
	threshold.copyTo(temp);

	//Output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;


	//finding the contours of filtered image
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);



	//Find the filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();



		//if number of objects greater than maxNumberOfDetObjects, it means we have a noisy filter
		if (numObjects<maxNumberOfDetObjects) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				/*
				if the area is less than 20px X 20px
				then it is probably just noise
				if the area is the same as the 1.5 of the image size,
				probably just a bad filter
				*/


				if (area>minAreaOfObj && area<maxAreaOfObj && area>refArea) {
					x = moment.m10 / area;
					y = moment.m01 / area;
					objectFound = true;
					refArea = area;
				}
				else objectFound = false;


			}


			//Show that Object is Found
			if (objectFound == true) {
				putText(cameraFeed, "Tracking Object", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);


				//Track Object by Drawing on screen.
				TrackObject(x, y, cameraFeed);
			}

		}
		else putText(cameraFeed, "ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}








int main(int argc, char* argv[])
{

	//Set these values true or false as you need.

	//We chose track objects to be true by default since it is the main purpose.
	bool trackObjects = true;
	bool useMorphOps = false;


	//Declaring a Matrix to store each frame of the camera feed
	Mat cameraFeed;


	//Storage for the HSV image
	Mat HSV;



	//Storage for the binary threshold image
	Mat threshold;



	//x and y values for the location of the object to Track.
	int x = 0, y = 0;




	//creating Track bars for HSV filtering
	createTrackbars();



	//video vidCapture object
	VideoCapture capture;




	//open the vidCapture object at location zero (This is the default location for the camera)
	capture.open(0);



	//These functions are setting height and width of vidCapture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH, widthFrame);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, heightFrame);


	//All of our operations will be performed within this loop. We start an infinite loop where cam feed is copied to cameraFeed matrix.

	while (1) {

		//We store the image into the Matrix
		capture.read(cameraFeed);


		//We convert the frame from BGR to HSV colorspace
		cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);


		//We filter HSV image between values and store filtered image to
		//threshold matrix
		inRange(HSV, Scalar(minimum_H, minimum_S, minimum_V), Scalar(maximum_H, maximum_S, maximum_V), threshold);


		//eliminate noise and emphasize the filtered object(s)
		if (useMorphOps)
			morphOps(threshold);




		//this function will return the x and y coordinates of the filtered object
		if (trackObjects)
			trackFilteredObject(x, y, threshold, cameraFeed);



		//show frames
		imshow(Title_Window2, threshold);
		imshow(Title_Window, cameraFeed);
		imshow(Title_Window1, HSV);


		//We delay for 30ms in order for the screen to refresh.
		waitKey(30);
	}






	return 0;
}
