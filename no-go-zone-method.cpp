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
#include <cmath>

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
    // for(int i; i < files.size(); i++){
    //     cout << files[i] << endl;
    // }

    // Select number of figure to calculate optical flow
    int N = 410;

    Mat original = imread(files[N]);
    Mat original_copy;
    original.copyTo(original_copy);
    cout << "Image 1 Name: " << files[N] << endl;
    Mat original_2 = imread(files[N+1]);
    Mat inputImage1 = imread(files[N], IMREAD_GRAYSCALE);
    Mat inputImage2 = imread(files[N+1], IMREAD_GRAYSCALE);
    Mat target_image1;
    Mat target_image2;

    float resolution_step_down = 3.0;
    float lower_width = original.cols/resolution_step_down;
    float lower_height = original.rows/resolution_step_down;

    UMat flowUmat;
    resize(inputImage1, target_image1, Size((int)lower_width, (int)lower_height), 0, 0, 1);
    float start = getTickCount();
    resize(inputImage2, target_image2, Size((int)lower_width, (int)lower_height), 0, 0, 1);


    // Calculate optical flow
    calcOpticalFlowFarneback(target_image1, target_image2, flowUmat, 0.4, 1, 24, 5, 8, 1.2, 0);
    
    // Copy Umat container to standard Mat
    Mat flow;
    flowUmat.copyTo(flow);
    cout << "Flow Rows: " << flow.rows << ", Flow Columns: " << flow.cols << endl;

        // For some reason here, after calculating the flow in the examples they sepecify the "grid" (y+=5 and x+=5)ç
        // which does not make a lot of sense to me. What role does the resolution play then?
    std::vector<double> sum_flow_row ((int)lower_height, 0);
    float temp = 0.0;
    for (int y = 0; y < flow.rows; y++) {

        for (int x = 0; x < flow.cols; x++)
        {
            const Point2f flowatxy = flow.at<Point2f>(y, x);
            temp += hypot(flowatxy.y,flowatxy.x);
        }
        sum_flow_row[y] = temp;
        // cout << "Row :" << y << ", value: " << temp << endl;
        temp = 0.0;
    }

    
    float max_value{0};
    for(int i = 0; i < sum_flow_row.size(); i++)
    {
        if (sum_flow_row[i] > max_value)
        {
            max_value = sum_flow_row[i];
        }
    }
    float time_taken = (getTickCount() - start)/getTickFrequency();
    cout << "Time taken: " << time_taken << endl;

    std::vector<float> normalised_heading_flow ((int)lower_height, 0);
    for(int i = 0; i < (int)lower_height; i++) {
    normalised_heading_flow[i] = (sum_flow_row[i]/max_value); // Become percentage from 0 to 1
    std::cout << normalised_heading_flow[i] <<std::endl;
    }


    float color_intensity{0};
    
    for (int y = 0; y < flow.rows; y++) {
        color_intensity = (int)((sum_flow_row[y]/max_value)*255);
        for (int x = 0; x < flow.cols; x++)
        {
                // Get the flow from y, x position. Change the * 2 to modify size of arrows
                const Point2f flowatxy = flow.at<Point2f>(y, x) * 2;
                // Draw line at flow direction
        
        // line(original, Point(x*(int)resolution_step_down, y*(int)resolution_step_down), Point(cvRound(x*(int)resolution_step_down + flowatxy.x), cvRound(y*(int)resolution_step_down + flowatxy.y)), Scalar(255,0,color_intensity));
                // Draw initial point
        // line(original, Point(x*(int)resolution_step_down, y*(int)resolution_step_down), Point(cvRound(x*(int)resolution_step_down + flowatxy.x), cvRound(y*(int)resolution_step_down + flowatxy.y)), Scalar(0,0,255));
        // circle(original, Point(x*(int)resolution_step_down, y*(int)resolution_step_down), 1, Scalar(0, 0, 0), -1);
        }
        line(original, Point(0,y*resolution_step_down), Point(original.cols,y*resolution_step_down), Scalar(255,0,color_intensity),1*resolution_step_down);
    }
    // line(original, Point(0,original.rows/2), Point(original.cols,original.rows/2), Scalar(0,255,0),2); // green midline
    
    // Draw the results
    namedWindow("prew", WINDOW_AUTOSIZE);
    imshow("prew", original);
    imshow("orignal_1", original_copy);
    imshow("original_2", original_2);
    cv::waitKey(0);
}

