#include <stdio.h>
#include <opencv/cv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <string>
#include <iostream>
#include "circle.cpp"
#include "Line.cpp"

#define sqr(x)((x)*(x))

using namespace cv;
using namespace std;

void convolution(cv::Mat &input, Mat &kernel, int size, cv::Mat &blurredOutput) {
    // intialise the output using the input
    blurredOutput.create(input.size(), CV_64F);

    // we need to create a padded version of the input
    // or there will be border effects
    int kernelRadiusX = ( kernel.size[0] - 1 ) / 2;
    int kernelRadiusY = ( kernel.size[1] - 1 ) / 2;

    cv::Mat paddedInput;
    cv::copyMakeBorder( input, paddedInput,
                        kernelRadiusX, kernelRadiusX, kernelRadiusY, kernelRadiusY,
                        cv::BORDER_REPLICATE );

    // now we can do the convoltion
    for ( int i = 0; i < input.rows; i++ ) {
        for( int j = 0; j < input.cols; j++ ) {
            double sum = 0.0;
            for( int m = -kernelRadiusX; m <= kernelRadiusX; m++ ) {
                for( int n = -kernelRadiusY; n <= kernelRadiusY; n++ ) {
                    // find the correct indices we are using
                    double imagex = i + m + kernelRadiusX;
                    double imagey = j + n + kernelRadiusY;
                    double kernelx = m + kernelRadiusX;
                    double kernely = n + kernelRadiusY;

                    // get the values from the padded image and the kernel
                    double imageval = ( double ) paddedInput.at<double>( imagex, imagey );
                    double kernalval = kernel.at<double>( kernelx, kernely );

                    // do the multiplication
                    sum += imageval * kernalval;
                }
            }
            // set the output value as the sum of the convolution
            blurredOutput.at<double>(i, j) = sum;
        }
    }
}

void normalize(Mat &M, Mat &out)  {
    out.create(M.size(), CV_8U);


	  double min = std::numeric_limits<double>::max();
	  double max = -std::numeric_limits<double>::max();


	  for (int i = 0; i < M.rows; i++ )  {
			  for (int j = 0; j < M.cols; j++ )  {
					  if ( M.at<double>(i, j) < min)  min = M.at<double>(i, j);
						else if(M.at<double>(i, j) > max) max =  M.at<double>(i, j);
				}
		}
		double range = max-min;
		printf("Range %f %f %f\n", min, max, range);


		for (int i = 0; i < M.rows; i++ )  {
			  for (int j = 0; j < M.cols; j++ )  {
						out.at<uchar>(i, j) = (uchar)((M.at<double>(i, j) - min)*255/range);
				}
		}
}

void calc_gradient_mag(Mat &left, Mat &right, Mat &out) {

    // out.create(left.size(), CV_8U);
    out.create(left.size(), CV_64F);
		Mat unNormalised;
		left.copyTo(unNormalised);

    for (int y = 0; y < left.rows; y++) {
        for (int x = 0; x < left.cols; x++) {
            double result = sqrt( pow((int)left.at<double>(y,x),2) + (int)pow(right.at<double>(y,x),2) );
            unNormalised.at<double>(y,x) = result;
        }
	}
    unNormalised.copyTo(out);
	// normalize(unNormalised, out);
}

void calc_gradient_dir(Mat &left, Mat &right, Mat &out)  {
    // out.create(left.size(), CV_8U);
    out.create(left.size(), CV_64F);
	Mat unNormalised;
	left.copyTo(unNormalised);

    for (int y = 0; y < left.rows; y++) {
        for (int x = 0; x < left.cols; x++) {
            double result = atan((right.at<double>(y,x)) / left.at<double>(y,x));
            unNormalised.at<double>(y,x) = result;
        }
    }
    unNormalised.copyTo(out);
	// normalize(unNormalised, out);
}

void imageToDouble(Mat &input, Mat &out)  {
    out.create(input.size(), CV_64F);


    for ( int i = 0; i < input.rows; i++ ) {
        for( int j = 0; j < input.cols; j++ ) {
						out.at<double>(i, j) = (double)input.at<uchar>(i, j);
				}
		}
}


void sobel(Mat &image, Mat &gradient_mag, Mat &gradient_dir) {
    // // create the Gaussian kernel in 1D
    cv::Mat kX = cv::getGaussianKernel(3, -1);
    cv::Mat kY = cv::getGaussianKernel(3, -1);

    // // make it 2D multiply one by the transpose of the other
    cv::Mat kernelG = kX * kY.t();

    Mat kernelX = (Mat_<double>(3,3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
    Mat kernelY = (Mat_<double>(3,3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);

    Mat outputX, outputY, doubleImage, blurredImage;

	imageToDouble(image, doubleImage);
	normalize(doubleImage, image);
    imwrite("test.jpg", image);


    convolution(doubleImage, kernelG, 3, blurredImage);
    convolution(blurredImage, kernelX, 3, outputX);
	Mat uX, uY, uGMag, uGDir;
	normalize(outputX, uX);
    imwrite("outputX.jpg", uX);
    convolution(blurredImage, kernelY, 3, outputY);
	normalize(outputY, uY);
    imwrite("outputY.jpg", uY);
    calc_gradient_mag(outputX, outputY, gradient_mag);
    normalize(gradient_mag, uGMag);
    imwrite("outputMag.jpg", uGMag);
    calc_gradient_dir(outputX, outputY, gradient_dir);
    normalize(gradient_dir, uGDir);
    imwrite("outputDir.jpg", uGDir);
}

void fullLine(cv::Mat &img, cv::Point a, cv::Point b, cv::Scalar color, double m) {
     Point p(0,0), q(img.cols,img.rows);

     p.y = -(a.x - p.x) * m + a.y;
     q.y = -(b.x - q.x) * m + b.y;

     line(img,p,q,color,1,8,0);
}

vector<Line> houghTransformLines(string imgName, Mat &gradient_mag, Mat &gradient_dir, double thresh_val)  {
    Mat image = imread(imgName, IMREAD_GRAYSCALE);

    Mat gradient_thresh, double_mag, double_dir;
    // imageToDouble(gradient_mag, double_mag);
    // imageToDouble(gradient_dir, double_dir);
    // Mat new_gradient = gradient_mag.mul(gradient_dir);
    Mat new_gradient = gradient_mag;
    Mat uNG;
    normalize(new_gradient, uNG);
    imwrite("outputNG.jpg", uNG);
    Mat normalized;
    normalize(new_gradient, normalized);
    int min = round(sqrt(sqr(image.cols) + sqr(image.rows)));
    Mat hough_space(Size(360,  min), CV_64F, Scalar(0));
    for (int y = 0; y < new_gradient.rows; y++) {
        for (int x = 0; x < new_gradient.cols; x++) {
            if (normalized.at<uchar>(y, x) > 128) {
                for (double t = 0; t < hough_space.cols; t++) {
                    // if (gradient_dir.at<double>(y, x) * 180 / M_PI + 90 - 1 <= t &&
                    //     (gradient_dir.at<double>(y, x) * 180 / M_PI) +90 + 1 >= t) {
                        
                    // }
                    int p = (int)(x * cos(t * M_PI / 180) + y * sin(t * M_PI / 180)) + min;
                    hough_space.at<double>(p, t) = hough_space.at<double>(p, t) + 1;
                }
            }
        }
    }
    Mat hough_out;
    normalize(hough_space, hough_out);
    imwrite("houghSpace.jpg", hough_out);
    printf("In lines\n");

    Mat colour_image;
    cvtColor( image, colour_image, CV_GRAY2BGR );
    vector<Line> lines;
    lines.reserve(100);
    int count = 0;
    for (int p = 0; p < hough_out.rows; p++) {
        for (int t = 0; t < hough_out.cols; t++) {
            if (hough_out.at<uchar>(p, t) > 128) {
                double m = - cos(t * M_PI / 180) / sin(t * M_PI / 180);
                double c = (p - min) / sin(t * M_PI / 180);
                Point p1(200, round(m * 200 + c));
                Point p2(300, round(m * 300 + c));
                Line line = Line(m, c);
                if(lines.size() < lines.capacity()) lines.push_back(line);
                else cout << "lines full\n";

                // line(image, p1, p2, Scalar(0, 255, 0), 2, 8, 0);
                fullLine(colour_image, p1, p2, Scalar(255, 255, 0), m);
            }
        }
    }
    imwrite("lines.jpg", colour_image);
    printf("Done lines\n");

    return lines;

}

int *** allocate3DArray(int y, int x, int r)  {
    int ***array = (int***)malloc(sizeof(int**)*y);
    for (int i = 0; i < y; i++)  {
        array[i] = (int**)malloc(sizeof(int*)*x);
        for (int j = 0; j < x; j++)  {
            array[i][j] = (int*)malloc(sizeof(int)*r);
            for(int k = 0; k < r; k++) {
                array[i][j][k] = 0;
            }
        }
    }
    return array;

}


void free3d(int ***arr, int y, int x, int r)  {
    for (int i = 0; i < y; i++)  {
        for (int j = 0; j < x; j++)  {
            free(arr[i][j]);
        }
        free(arr[i]);

    }
    free(arr);

}

vector<Circle> HoughTransformCircles(string imgName, Mat &gradient_mag, Mat &gradient_dir, double thresh_val)  {
    Mat image = imread(imgName, IMREAD_GRAYSCALE);

    int rLen = image.rows/2;
    int ***houghSpace = allocate3DArray(image.rows, image.cols, rLen);
    for (int y = 0; y < image.rows; y++)    {
        for (int x = 0; x < image.cols; x++)  {
            for (int r = 0; r < rLen; r++)  {
                int x0 = x - (int)(r*cos(gradient_dir.at<double>(y, x)));
                int y0 = y - (int)(r*sin(gradient_dir.at<double>(y, x)));
                if(x0 >= 0 && x0 < image.cols && y0 >= 0 && y0 < image.rows )  {
                    houghSpace[y0][x0][r]++; 
                }
                x0 = x + (int)(r*cos(gradient_dir.at<double>(y, x)));
                y0 = y + (int)(r*sin(gradient_dir.at<double>(y, x)));
                if(x0 >= 0 && x0 < image.cols && y0 >= 0 && y0 < image.rows )  {
                    houghSpace[y0][x0][r]++; 
                }
            
            }
        }
    }
    printf("In circles\n");
    vector<Circle> circles;
    circles.reserve(100);

    cout<<circles.capacity()<<std::endl;
    for (int y = 0; y < image.rows; y++)    {
        for (int x = 0; x < image.cols; x++)  {
            for (int r = 0; r < rLen; r++)  {
                
                if (houghSpace[y][x][r] > 45)  {
                    Circle temp  = Circle(x, y, r);
                    if(circles.size() < circles.capacity() ) circles.push_back(temp);
                    else cout << "circles full\n";
                    circle(image, Point(x, y), r, Scalar(255, 255, 255), 2, 0);
                }
            }
        }
    }
    free3d(houghSpace, image.rows, image.cols, rLen);
    imwrite("circles.jpg", image);
    printf("Done circles\n");

    return circles;    
}


void getGradients(string imgName, Mat &grad_mag, Mat &grad_dir)  {
    Mat image = imread(imgName, IMREAD_GRAYSCALE);
    sobel(image, grad_mag, grad_dir);
}

