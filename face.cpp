/////////////////////////////////////////////////////////////////////////////
//
// COMS30121 - face.cpp
//
/////////////////////////////////////////////////////////////////////////////

// header inclusion
// header inclusion
#include <stdio.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv/cv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

/** Function Headers */
vector<Rect> detectAndDisplay( Mat frame );

/** Global variables */
String cascade_name = "dartcascade/cascade.xml";
CascadeClassifier cascade;


/** @function main */
vector<Rect> facemain( const char* img_name )
{
       // 1. Read Input Image
	Mat frame = imread(img_name, CV_LOAD_IMAGE_COLOR);

	// // 2. Load the Strong Classifier in a structure called `Cascade'
	vector<Rect> detected;
	if( !cascade.load( cascade_name ) ){ printf("--(!)Error loading\n"); return detected; };

	// // 3. Detect Faces and Display Result
	detected = detectAndDisplay( frame );

	// 4. Save Result Image
	imwrite( "subtask2/detected.jpg", frame );

	return detected;
}

/** @function detectAndDisplay */
vector<Rect> detectAndDisplay( Mat frame )
{
	std::vector<Rect> faces;
	Mat frame_gray;

	// 1. Prepare Image by turning it into Grayscale and normalising lighting
	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	// 2. Perform Viola-Jones Object Detection
	cascade.detectMultiScale( frame_gray, faces, 1.1, 1, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50), Size(500,500) );

       // 3. Print number of Faces found
	//std::cout << faces.size() << std::endl;

       // 4. Draw box around faces found
	for( int i = 0; i < faces.size(); i++ )
	{
		rectangle(frame, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), Scalar( 0, 255, 0 ), 2);
	}

	return faces;


}
