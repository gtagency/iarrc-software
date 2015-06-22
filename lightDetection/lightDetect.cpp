#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/opencv.hpp>

#define HIST_FRAMES 5     // Number of frames to keep in history
#define LIGHT_DIST_PIX 45 // Pixels between the center of red and green lights
#define MAXSUMRESULTSRED2GREEN 3400000 // experimental value of the max of the sum of all results pixels during red2green
#define TRIGGERPERCENTAGE .75 // The percentage an event must be from experimental, accounting for noise, to trigger

using namespace cv;
using namespace std;

// What happens when the stoplight changes from red to green?
// The stoplight has a red light on from the start
// This light is very bright, and can wash out the image
// It typically looks like a reddish halo around a white circle
// The stoplight then turns on the green light, and both are on
// This looks like two white circles one over the other,
// The top having a red halo, and the bottom a green halo
// This lasts for a few frames, then the red light turns off
int main(int argc, char** argv)
{
    VideoCapture cap(1);
    if (!cap.isOpened()) {
        return -1;
    }

    for (;;) {
          // Intialize variables
        Mat frame, noWhite, noWhiteMask, redLight, greenLight;
        static Mat history[HIST_FRAMES];
        static int counttostart = 1;

        cap >> frame;

        for(int i = HIST_FRAMES-1; i > 0; i--) {
            history[i] = history[i - 1];
        }
        history[0] = frame;
        if (!(counttostart == HIST_FRAMES)) {
            counttostart++;
            continue;
        }

        Mat past, cur;

        past = history[HIST_FRAMES - 1];
        cur = history[0];

        Mat translate = (Mat_<double>(2, 3) << 1 , 0 , 0 , 0 , 1 , LIGHT_DIST_PIX);

        Mat gaining = cur - past;
        Mat losing = past - cur;
        warpAffine(losing, losing, translate, past.size());

        Mat channel[3];
        split(gaining, channel);
        Mat gaininggreen = channel[0] - channel[2]; // we use blue because green is more blue than green
        split(losing, channel);
        Mat losingred = channel[2] - channel[0]/2 - channel[1]/2;

        Mat geometricmean;
        gaininggreen.convertTo(gaininggreen, CV_16UC1, 1, 0);
        losingred.convertTo(losingred, CV_16UC1, 1, 0);
        geometricmean = gaininggreen.mul(losingred);
        Mat result(gaining.size(), CV_8UC1);
//        geometricmean.convertTo(result, CV_8UC1, 1/255.0, 0);
//        Mat both = gaining/2 + losing/2;

        double sumresult = sum(result)[0];
        if (sumresult > TRIGGERPERCENTAGE * MAXSUMRESULTSRED2GREEN)
            cout << "GO GO GO!!!!!!" << endl;
        else
            cout << "Wait for it...";
        cout << "   " << sumresult << endl;

//        imshow("losing", losing);
//        imshow("gaining", gaining);
//        imshow("lightchange", result);
//        imshow("both", both);
//        imshow("frame", frame);

      if (waitKey(30) >= 0) break;
    }
    
    return 0;
}
