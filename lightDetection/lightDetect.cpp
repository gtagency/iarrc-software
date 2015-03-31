#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main()
{
    VideoCapture cap(1);
    if (!cap.isOpened()) {
        return -1;
    }

    for (;;) {
        Mat frame, noWhite, noWhiteMask, redLight, greenLight;
        cap >> frame;
	
	cvtColor(frame, noWhiteMask, CV_BGR2GRAY);
	threshold(noWhiteMask, noWhiteMask, 250, 255, THRESH_BINARY_INV);
	frame.copyTo(noWhite, noWhiteMask);

	imshow("No White", noWhite);

	inRange(noWhite, Scalar(0, 80, 240), Scalar(50, 150, 255), redLight);
	inRange(noWhite, Scalar(50, 150, 70), Scalar(150, 250, 170), greenLight);

	int size = 11;
	Mat element = getStructuringElement(MORPH_RECT, Size(2*size+1, 2*size+1), Point(size, size));

	dilate(redLight, redLight, element);
	dilate(greenLight, greenLight, element);
	erode(redLight, redLight, element);
	erode(greenLight, greenLight, element);


	int redCount = sum(redLight).val[0];
	int greenCount = sum(greenLight).val[0];

	if (redCount > greenCount) {
	  std::cout << "Current Color: Red  " << "\r";
	} else {
	  std::cout << "Current Color: Green" << "\r";
	}

	std::cout.flush();

        if (waitKey(30) >= 0) break;
    }
		
    return 0;
    
}
