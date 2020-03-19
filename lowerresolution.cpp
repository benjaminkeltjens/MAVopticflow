#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/core/cvstd.hpp>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

int main(){
String imgFile = "/home/benjamin/git/MAVopticflow/jpg2png/80078093.png";
Mat inputImage = imread(imgFile, IMREAD_GRAYSCALE);
Mat target_image;
resize(inputImage, target_image, Size(28, 60), 0, 0, 1);
imwrite("/home/benjamin/git/MAVopticflow/test.png", target_image);
return 0;
}