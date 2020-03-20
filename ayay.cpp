// Compile with: (change your -I and -L path to your corresponding locations)
// g++ ayay2.cpp -I/usr/local/include/opencv4 -L/usr/local/lib -lopencv_optflow -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_video -o ayay2

// The original example's code can be found at:
// https://funvision.blogspot.com/2016/02/opencv-31-tutorial-optical-flow.html

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
    vector<string> files = globVector("./new_files/*");
    for(int i; i < files.size(); i++){
        cout << files[i] << endl;
    }

    // Select number of figure to calculate optical flow
    int N = 2;

    Mat original = imread(files[N]);
    Mat inputImage1 = imread(files[N], IMREAD_GRAYSCALE);
    Mat inputImage2 = imread(files[N+1], IMREAD_GRAYSCALE);
    Mat target_image1;
    Mat target_image2;

    float resolution_step_down = 5.0;
    float lower_width = original.cols/resolution_step_down;
    float lower_height = original.rows/resolution_step_down;

    UMat flowUmat;
    resize(inputImage1, target_image1, Size((int)lower_width, (int)lower_height), 0, 0, 1);
    float start = getTickCount();
    resize(inputImage2, target_image2, Size((int)lower_width, (int)lower_height), 0, 0, 1);


    // Calculate optical flow
    calcOpticalFlowFarneback(target_image1, target_image2, flowUmat, 0.4, 1, 12, 2, 8, 1.2, 0);
    float time_taken = (getTickCount() - start)/getTickFrequency();
    cout << "Time taken: " << time_taken << endl;
    
    // Copy Umat container to standard Mat
    Mat flow;
    flowUmat.copyTo(flow);
    cout << "Flow Rows: " << flow.rows << ", Flow Columns: " << flow.cols << endl;

        // For some reason here, after calculating the flow in the examples they sepecify the "grid" (y+=5 and x+=5)ç
        // which does not make a lot of sense to me. What role does the resolution play then?
    for (int y = 0; y < flow.rows; y += 1) {
        for (int x = 0; x < flow.cols; x += 1)
        {
                // Get the flow from y, x position. Change the * 2 to modify size of arrows
                const Point2f flowatxy = flow.at<Point2f>(y, x) * 2;
                // Draw line at flow direction
        line(original, Point(x*(int)resolution_step_down, y*(int)resolution_step_down), Point(cvRound(x*(int)resolution_step_down + flowatxy.x), cvRound(y*(int)resolution_step_down + flowatxy.y)), Scalar(255,0,0));
                // Draw initial point
        circle(original, Point(x*(int)resolution_step_down, y*(int)resolution_step_down), 1, Scalar(0, 0, 0), -1);
        }
    }
    // Draw the results
    namedWindow("prew", WINDOW_AUTOSIZE);
    imshow("prew", original);
    cv::waitKey(0);
}

