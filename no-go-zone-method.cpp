// Compile with: (change your -I and -L path to your corresponding locations)
// g++ ayay2.cpp -I/usr/local/include/opencv4 -L/usr/local/lib -lopencv_optflow -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_video -o ayay2

// The original example's code can be found at:
// https://funvision.blogspot.com/2016/02/opencv-31-tutorial-optical-flow.html


// Initial includes
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/video/tracking.hpp"
#include <vector>
#include <stdio.h>
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
    vector<string> files = globVector("/home/marina/Dropbox/TUDelft/Subjects/Q3/MAVs/ourProject/AE4317_2019_datasets/sim_poles_panels/20190121-161422/*");

    //----------------------------------------------------//
    //----------------- SOME INPUTS ----------------------//
    //----------------------------------------------------//
    int N = 300;                            // Figure number
    float resolution_step_down = 3.0;       // Factor to low resolution
    int GB_parameter = 33;                  // Kernel size for Gaussian Blur (must be an odd number)
    float goTo_threshold = 0.10;            // OF threshold below which the heading is considered a safe go-to area

    // Parameters for Farneback OF calculation:
    float imScaleParam = 0.4; // Image pyramid or simple image scale
    int pyrLayers = 1;        // No. Pyramid layers. [#1 means that flow is calculated only from previous image]
    int winSize = 24;         // Flow is computed over the window. Larger value is more robust to the noise
    int meanIt = 5;           // Mean number of iterations of algorithm
    int polDegree = 8;        // Polynomial degree of expansion. Recommended values are between 5 - 7
    float stDev = 1.2;        // Standard deviation used to smooth used derivatives. Recommended values are between 1.1 - 1,5
    int extra = 0;

    //----------------------------------------------------//
    //----------------- END OF INPUTS --------------------//
    //----------------------------------------------------//

    Mat original = imread(files[N]);        // Mat that stores inputImage1 in colour
    Mat original2 = imread(files[N+1]);     // Mat that stores inputImage2 in colour
    Mat initial_flow;                       // Mat that stores the initial (non-smoothed) normalised flow calculation  
    Mat smooth;                             // Mat that stores the smooth OF after applying the GaussianBlur
    Mat min_area;                           // Mat that stores the highlighted recommended go-to areas (less flow than a certain threshold)

    // Initializing variables (reading files, etc.) ->
    original.copyTo(smooth);
    original.copyTo(min_area);
    cout << "Image 1 Name: " << files[N] << endl;
    Mat inputImage1 = imread(files[N], IMREAD_GRAYSCALE);
    Mat inputImage2 = imread(files[N+1], IMREAD_GRAYSCALE);
    Mat target_image1;
    Mat target_image2;

    // Lowering resolution ->
    float lower_width = original.cols/resolution_step_down;  // Calculating new width
    float lower_height = original.rows/resolution_step_down; // Calculating new height

    original.copyTo(initial_flow);

    UMat flowUmat;
    resize(inputImage1, target_image1, Size((int)lower_width, (int)lower_height), 0, 0, 1);
    float start = getTickCount();
    resize(inputImage2, target_image2, Size((int)lower_width, (int)lower_height), 0, 0, 1);


    // Optical Flow calculation ->
    calcOpticalFlowFarneback(target_image1, target_image2, flowUmat, imScaleParam, pyrLayers, winSize, meanIt, polDegree, stDev, extra);
    
    Mat flow; // Copy Umat container to standard Mat
    flowUmat.copyTo(flow);
    cout << "Flow Rows: " << flow.rows << ", Flow Columns: " << flow.cols << endl;

    std::vector<double> sum_flow_row ((int)lower_height, 0);
    float temp = 0.0;
    for (int y = 0; y < flow.rows; y++) 
    {
        for (int x = 0; x < flow.cols; x++)
        {
            const Point2f flowatxy = flow.at<Point2f>(y, x);
            temp += hypot(flowatxy.y,flowatxy.x);               // Calculating the sum of all OF magnitudes per row (direction perpendicular to columns)
        }
        sum_flow_row[y] = temp;                                 // Storing OF sums in a vector (dimension: number of rows)
        temp = 0.0;
    }

    // Normalising OF between values of 0 and 1 ->

    float max_value{0};
    for(int i = 0; i < sum_flow_row.size(); i++)
    {
        if (sum_flow_row[i] > max_value)
        {
            max_value = sum_flow_row[i];    // Calculating MAX value of sum_flow_row
        }
    }

    float min_value{max_value};
    for(int i = 0; i < sum_flow_row.size(); i++)
    {
        if (sum_flow_row[i] < min_value)
        {
            min_value = sum_flow_row[i];    // Calculating MIN value of sum_flow_row
        }
    }

    float time_taken = (getTickCount() - start)/getTickFrequency();
    cout << "Time taken: " << time_taken << endl; // Computation time

    std::vector<float> normFlow ((int)lower_height, 0);
    for(int i = 0; i < (int)lower_height; i++) {
    normFlow[i] = ((sum_flow_row[i]-min_value)/(max_value-min_value)); // Normalisation between 0 and 1
    std::cout << normFlow[i] <<std::endl;
    }

    // Applying smoothing filter to normalised flow (Gaussian Blur) ->

    std::vector<float> normFlow_smooth ((int)lower_height, 0);
    GaussianBlur(normFlow, normFlow_smooth, Size(GB_parameter,GB_parameter), 0);

    std::vector<float> normFlow_smooth_goTo ((int)lower_height, 0);
    for(int i = 0; i < (int)lower_height; i++) {
        if(normFlow_smooth[i] > goTo_threshold){
            normFlow_smooth_goTo[i] = normFlow_smooth[i];
        }
        else{
            normFlow_smooth_goTo[i] = 0;
        }
    }

    // Nice plots ->

    float color_intensity{0};
    float color_intensity_smooth{0};
    float color_intensity_smooth_goTo{0};
    
    for (int y = 0; y < flow.rows; y++) {
        color_intensity = (int)((normFlow[y])*255);
        color_intensity_smooth = (int)((normFlow_smooth[y])*255);
        color_intensity_smooth_goTo = (int)((normFlow_smooth_goTo[y])*255);
        for (int x = 0; x < flow.cols; x++)
        {
                // Get the flow from y, x position. Change the * 2 to modify size of arrows
                // const Point2f flowatxy = flow.at<Point2f>(y, x) * 2;
                // Draw line at flow direction
        
        // line(original, Point(x*(int)resolution_step_down, y*(int)resolution_step_down), Point(cvRound(x*(int)resolution_step_down + flowatxy.x), cvRound(y*(int)resolution_step_down + flowatxy.y)), Scalar(255,0,color_intensity));
                // Draw initial point
        // line(original, Point(x*(int)resolution_step_down, y*(int)resolution_step_down), Point(cvRound(x*(int)resolution_step_down + flowatxy.x), cvRound(y*(int)resolution_step_down + flowatxy.y)), Scalar(0,0,255));
        // circle(original, Point(x*(int)resolution_step_down, y*(int)resolution_step_down), 1, Scalar(0, 0, 0), -1);
        }
        line(initial_flow, Point(0,y*resolution_step_down), Point(original.cols,y*resolution_step_down), Scalar(255,color_intensity,0),1*resolution_step_down);
        line(smooth, Point(0,y*resolution_step_down), Point(original.cols,y*resolution_step_down), Scalar(255,color_intensity_smooth,0),1*resolution_step_down);
        line(min_area, Point(0,y*resolution_step_down), Point(original.cols,y*resolution_step_down), Scalar(255,color_intensity_smooth_goTo,0),1*resolution_step_down);
    }
    // line(original, Point(0,original.rows/2), Point(original.cols,original.rows/2), Scalar(0,255,0),2); // green midline
    
    // Draw the results
    //namedWindow("prew", WINDOW_AUTOSIZE);
    imshow("initial_flow", initial_flow);
    imshow("smooth", smooth);
    imshow("min_area", min_area);
    imshow("inputImage1", original);
    imshow("inputImage2", original2);
    cv::waitKey(0);
}

