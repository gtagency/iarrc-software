#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv)
{
    VideoCapture cap(1);
    if (!cap.isOpened()) {
        return -1;
    }

    for (;;) {
      // Intialize variables
      Mat frame, noWhite, noWhiteMask, redLight, greenLight;
      cap >> frame;
	
    
      cvtColor(frame, noWhiteMask, CV_BGR2GRAY);
      threshold(noWhiteMask, noWhiteMask, 250, 255, THRESH_BINARY_INV);
      frame.copyTo(noWhite, noWhiteMask);

      inRange(noWhite, Scalar(0, 80, 240), Scalar(50, 150, 255), redLight);
      inRange(noWhite, Scalar(50, 150, 70), Scalar(150, 250, 170), greenLight);

      int size = 11;
      Mat element = getStructuringElement(MORPH_RECT, Size(2*size+1, 2*size+1), Point(size, size));

      dilate(redLight, redLight, element);
      dilate(greenLight, greenLight, element);
      erode(redLight, redLight, element);
      erode(greenLight, greenLight, element);

      imshow("redLight", redLight);
      imshow("greenLight", greenLight);


      //grayscale and blur
      Mat img;
      cvtColor(frame, img, CV_BGR2GRAY);
      Mat cimg;
      medianBlur(img, img, 5);
      cvtColor(img, cimg, CV_GRAY2BGR);

      vector<Vec3f> circles;
      HoughCircles(img, circles, 3, 1, 10,
                   100, 30, 1, 30 // change the last two parameters
                   // (min_radius & max_radius) to detect larger circles
                   );

      int redCircle = 0;
      int greenCircle = 0;
      for( size_t i = 0; i < circles.size(); i++ )
        {
          Vec3i c = circles[i];
          redCircle = redCircle || redLight.at<uchar>(c[1], c[0]);
          greenCircle = greenCircle || greenLight.at<uchar>(c[1], c[0]);
          if (redLight.at<uchar>(c[1], c[0]) || greenLight.at<uchar>(c[1], c[0])) {
            circle( frame, Point(c[0], c[1]), c[2], Scalar(0,0,255), 3, 16);
            circle( frame, Point(c[0], c[1]), 2, Scalar(0,255,0), 3, 16);
          }
        }

      if (redCircle && greenCircle)
        {
          std::cout << "Light Changed!" << std::endl;
        }


      imshow("detected circles", frame);

      if (waitKey(30) >= 0) break;
    }
    
    return 0;
}
