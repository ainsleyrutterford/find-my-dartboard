#include <stdio.h>
#include <opencv/cv.hpp>
#include <math.h>
#include <string>
#include <iostream>
#include "circle.cpp"
#include "Line.cpp"

#define sqr(x)((x)*(x))

using namespace cv;
using namespace std;

void calc_gradient_mag(Mat &left, Mat &right, Mat &out) {
    out.create(left.size(), CV_64F);
    for (int y = 0; y < left.rows; y++) {
        for (int x = 0; x < left.cols; x++) {
            double result = sqrt( pow((int)left.at<double>(y,x),2) + (int)pow(right.at<double>(y,x),2) );
            out.at<double>(y,x) = result;
        }
	}
}
void calc_gradient_dir(Mat &left, Mat &right, Mat &out)  {
    out.create(left.size(), CV_64F);

    for (int y = 0; y < left.rows; y++) {
        for (int x = 0; x < left.cols; x++) {
            double result = atan2((right.at<double>(y,x)) , left.at<double>(y,x));
            out.at<double>(y,x) = result;
        }
    }
}

void imageToDouble(Mat &input, Mat &out)  {
    out.create(input.size(), CV_64F);


    for ( int i = 0; i < input.rows; i++ ) {
        for( int j = 0; j < input.cols; j++ ) {
						out.at<double>(i, j) = (double)input.at<uchar>(i, j);
				}
		}
}
void convolution(cv::Mat &input, Mat &kernel, int size, cv::Mat &blurredOutput) {
    // intialise the output using the input
    blurredOutput.create(input.size(), CV_64F);

    // we need to create a padded version of the input
    // or there will be border effects
    int kernelRadiusX = ( kernel.size[0] - 1 ) / 2;
    int kernelRadiusY = ( kernel.size[1] - 1 ) / 2;

    cv::Mat paddedInput;
    cv::copyMakeBorder( input, paddedInput, kernelRadiusX, kernelRadiusX, kernelRadiusY, kernelRadiusY, cv::BORDER_REPLICATE );
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
    convolution(doubleImage, kernelG, 3, blurredImage);
    convolution(blurredImage, kernelX, 3, outputX);
    convolution(blurredImage, kernelY, 3, outputY);
    calc_gradient_mag(outputX, outputY, gradient_mag);
    calc_gradient_dir(outputX, outputY, gradient_dir);
	// Mat uX, uY, uGMag, uGDir;
    // normalize(outputX, uX);
    // imwrite("outputX.jpg", uX);
    // normalize(outputY, uY);
    // imwrite("outputY.jpg", uY);
    // normalize(gradient_dir, uGDir);
    // imwrite("outputDir.jpg", uGDir);
    // normalize(gradient_mag, uGMag);
    // imwrite("outputMag.jpg", uGMag)
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
    for (int i = 0; i < M.rows; i++ )  {
        for (int j = 0; j < M.cols; j++ )  {
            out.at<uchar>(i, j) = (uchar)((M.at<double>(i, j) - min)*255/range);
        }
    }
}
void getGradients(Mat image, Mat &grad_mag, Mat &grad_dir)  {
    sobel(image, grad_mag, grad_dir);
    cout << "sobel finished\n";
}

void fullLine(cv::Mat &img, cv::Point a, cv::Point b, cv::Scalar color, double m, Mat &gradient_mag, double c) {
     Point p(0,0), q(img.cols,img.rows);

     p.y = -(a.x - p.x) * m + a.y;
     q.y = -(b.x - q.x) * m + b.y;

    line(img,p,q,color,1,8,0);

    Point p0(-1, -1);
    Point p1(-1, -1);

    //  for (int x = 0; x < img.cols; x+=3)  {
    //     int y = m*x + c;
    //     if(y < img.rows && y >=0)  {
    //         if(gradient_mag.at<uchar>(y, x)> 80  || gradient_mag.at<uchar>(y+1, x)> 80 )  {
    //             if (p0.x != -1 && p0.y != -1)  {
    //                 p1 = Point(x, y);
    //             } else {
    //                 p0 = Point(x, y);
    //                 p1 = Point(x, y);
                    
    //             }
    //         } else {
    //             line(img,p0,p1,color,1,8,0);
    //             p0 = Point(-1, -1);
    //             p1 =Point(-1, -1);
    //         }
    //     }
        
    //  }
}

vector<Line> houghTransformLines(Mat &image, Mat &gradient_mag, Mat &gradient_dir)  {
    Mat normalized;
    normalize(gradient_mag, normalized);
    //Shift negative angle values, so will always be in positive array location
    int min = round(sqrt(sqr(image.cols) + sqr(image.rows)));
    Mat hough_space(Size(360, 2*min), CV_64F, Scalar(0));
    for (int y = 0; y < gradient_mag.rows; y++) {
        for (int x = 0; x < gradient_mag.cols; x++) {
            //Increment hough space if gradient_mag above thresh AND t matches gradient_dir angle
            if (normalized.at<uchar>(y, x) > 128) {
                for (double t = 0; t < hough_space.cols; t++) {
                    double b2w_angle = gradient_dir.at<double>(y, x)+2*M_PI;
                    double w2b_angle = gradient_dir.at<double>(y, x) + M_PI;
                    if(b2w_angle >= 2*M_PI) b2w_angle -= 2*M_PI; 

                    if (abs(b2w_angle*180/M_PI - t)  <= 0.5) {
                        int p = (int)(x * cos(t*M_PI/180) + y * sin(t*M_PI/180)) + min;
                        hough_space.at<double>(p, t) = hough_space.at<double>(p, t) + 1;
                    }
                    if (abs(w2b_angle*180/M_PI - t)  <= 0.5) {
                        int p = (int)(x * cos(t*M_PI/180) + y * sin(t*M_PI/180)) + min;
                        hough_space.at<double>(p, t) = hough_space.at<double>(p, t) + 1;
                    }
                }
            }
        }
    }
    Mat hough_out   ;
    normalize(hough_space, hough_out);
    // imwrite("houghSpace.jpg", hough_out);
    // printf("In lines\n");
    vector<Line> lines;
    for (int p = 0; p < hough_out.rows; p++) {
        for (int t = 0; t < hough_out.cols; t++) {
            if (hough_out.at<uchar>(p, t) > 40) {
                double m = - cos(t * M_PI / 180) / sin(t * M_PI / 180);
                double c = (p - min) / sin(t * M_PI / 180);
                Line line = Line(m, c);
                Point p1(200, line.getY(200)); 
                Point p2(300, line.getY(300));
                lines.push_back(line);
            }
        }
    }
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

vector<Circle> HoughTransformCircles(Mat image, Mat &gradient_mag, Mat &gradient_dir)  {
    //Maximum circle radius is rLen = image.width / 2
    int rLen = image.rows/2;
    int ***houghSpace = allocate3DArray(image.rows, image.cols, rLen);
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            for (int r = 0; r < rLen; r++)     {
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
    //
    vector<Circle> circles;
    for (int y = 0; y < image.rows; y++)    {
        for (int x = 0; x < image.cols; x++)  {
            for (int r = 0; r < rLen; r++)  {
                if (houghSpace[y][x][r] > 30)  {
                    Circle temp  = Circle(x, y, r);
                    circles.push_back(temp);
                }
            }
        }
    }

    //Filter similar circles
    vector<Circle> filterCircles;
    if(circles.size() != 0)  filterCircles.push_back(circles.at(0));
    for (int c = 1; c < circles.size(); c++)  {
        bool similar = false;
        for (int fc = 0; fc < filterCircles.size() && similar == false; fc++)  {
            if( circles.at(c).isSimilarTo(filterCircles.at(fc)))  {
                similar = true;
            }
        }
        if (!similar)   filterCircles.push_back(circles.at(c));
    }
    free3d(houghSpace, image.rows, image.cols, rLen);
    return filterCircles;
}


