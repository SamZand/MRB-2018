#pragma once

#include "iostream"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "thread"

using namespace cv;
using namespace std;

class Tracker : public std::thread{
private:
    int posX;
    int posY;
    
public:
    int getY()
	{ 
		return posY;
    }

    int getX()
	{
		return posX;
    }

	/** void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
        cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
        cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    else if  ( event == EVENT_MBUTTONDOWN )
    {
        cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    else if ( event == EVENT_MOUSEMOVE )
    {
        cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
    }    
} **/

    void run()
    {
        VideoCapture cap(1); //capture the video from webcam

        if ( !cap.isOpened() )  // if not success, exit program
        {
            cout << "Cannot open the web cam" << endl;
            std::terminate;
        }

        namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"
		MouseCallback();

		int iLowH = 0;
		int iHighH = 123;

		int iLowS = 0; 
		int iHighS = 96;

		int iLowV = 0;
		int iHighV = 34;

		//Create trackbars in "Control" window
		createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
		createTrackbar("HighH", "Control", &iHighH, 179);

		createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
		createTrackbar("HighS", "Control", &iHighS, 255);

		createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
		createTrackbar("HighV", "Control", &iHighV, 255);

		int iLastX = -1; 
		int iLastY = -1;

		//Capture a temporary image from the camera
		Mat imgTmp;
		cap.read(imgTmp); 

		//Create a black image with the size as the camera output
		Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );;

        while (true)
        {
            Mat imgOriginal;

            bool bSuccess = cap.read(imgOriginal); // read a new frame from video

            if (!bSuccess) //if not success, break loop
            {
                cout << "Cannot read a frame from video stream" << endl;
                break;
            }

			Mat imgHSV;

			cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
        
			Mat imgThresholded;			

			GaussianBlur(imgHSV, imgHSV, Size(9, 9), 2, 2 );

			inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
            
			//morphological opening (removes small objects from the foreground)
			erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
			dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

			//morphological closing (removes small holes from the foreground)
			dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
			erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

			//Calculate the moments of the thresholded image
			Moments oMoments = moments(imgThresholded);

			double dM01 = oMoments.m01;
			double dM10 = oMoments.m10;
			double dArea = oMoments.m00;

			// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
			if (dArea > 10000)
			{
				//calculate the position of the ball
				posX = dM10 / dArea;
				posY = dM01 / dArea;         
                
				if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
				{
					//Draw a red line from the previous point to the current point
					line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0,0,255), 2);
				}

				iLastX = posX;
				iLastY = posY;
			}

			imgOriginal = imgOriginal + imgLines;
			imshow("Original", imgOriginal); //show the original image

			if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
			{
				cout << "esc key is pressed by user" << endl;
				break; 
			}
		}
    }
};