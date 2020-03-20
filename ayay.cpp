// Compile with: (change your -I and -L path to your corresponding locations)
// g++ ayay2.cpp -I/usr/local/include/opencv4 -L/usr/local/lib -lopencv_optflow -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_video -o ayay2

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/video/tracking.hpp"
#include <vector>
#include <stdio.h>
//#include <Windows.h>
#include <iostream>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/core/cvstd.hpp>

#include <glob.h>

using namespace cv;
using namespace std;
using std::vector;

// Function that reads names of files in certain folder
vector<string> globVector(const string& pattern){
    glob_t glob_result;
    glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result);
    vector<string> files;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        files.push_back(string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    return files;
}

int main(int argc, const char** argv)
{
    // Collecting all filenames in ./jpg2png/ in files
    vector<string> files = globVector("./jpg2png/*");

    // Select number of figure to calculate optical flow
    int N = 1;

    Mat original = imread(files[N]);
    Mat inputImage1 = imread(files[N], IMREAD_GRAYSCALE);
    Mat inputImage2 = imread(files[N+1], IMREAD_GRAYSCALE);
    Mat target_image1;
    Mat target_image2;
    resize(inputImage1, target_image1, Size(28, 60), 0, 0, 1);
    resize(inputImage2, target_image2, Size(28, 60), 0, 0, 1);

    UMat flowUmat;

    // Calculate optical flow
    calcOpticalFlowFarneback(target_image1, target_image2, flowUmat, 0.4, 1, 12, 2, 8, 1.2, 0);
    
    // Copy Umat container to standard Mat
    Mat flow;
    flowUmat.copyTo(flow);

        // For some reason here, after calculating the flow in the examples they sepecify the "grid" (y+=5 and x+=5)ç
        // which does not make a lot of sense to me. What role does the resolution play then?
    for (int y = 0; y < original.rows; y += 5) {
        for (int x = 0; x < original.cols; x += 5)
        {
                // Get the flow from y, x position. Change the * 2 to modify size of arrows
                const Point2f flowatxy = flow.at<Point2f>(y, x) * 2;
                // Draw line at flow direction
        line(original, Point(x, y), Point(cvRound(x + flowatxy.x), cvRound(y + flowatxy.y)), Scalar(255,0,0));
                // Draw initial point
        circle(original, Point(x, y), 1, Scalar(0, 0, 0), -1);
        }
    }
    // Draw the results
    namedWindow("prew", WINDOW_AUTOSIZE);
    imshow("prew", original);
    cv::waitKey(0);
}

