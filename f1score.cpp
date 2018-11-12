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

double f1score(double truePositives, double falsePostives, double falseNegatives);

int main(int n, char ** args)  {
    double truePositives  = (double)atoi(args[1]);
    double falsePostives  = (double)atoi(args[2]);
    double falseNegatives = (double)atoi(args[3]);
    f1score(truePositives, falsePostives, falseNegatives);
    return 0;
}

double f1score (double truePositives, double falsePostives, double falseNegatives)  {

    double precision = truePositives/(truePositives+falsePostives);
    double recall = truePositives/(truePositives+falseNegatives);

    double f1 =  2*precision*recall/(precision+recall);
    printf("%f\n", f1);
    return f1;
}
