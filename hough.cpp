#include <stdio.h>
#include <opencv/cv.hpp>
#include <math.h>
#include <iostream>

#define sqr(x)((x)*(x))

using namespace cv;

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

    out.create(left.size(), CV_8U);
		Mat unNormalised;
		left.copyTo(unNormalised);

    for (int y = 0; y < left.rows; y++) {
        for (int x = 0; x < left.cols; x++) {
            double result = sqrt( pow((int)left.at<double>(y,x),2) + (int)pow(right.at<double>(y,x),2) );
            unNormalised.at<double>(y,x) = result;
        }
	}
	normalize(unNormalised, out);
}

void calc_gradient_dir(Mat &left, Mat &right, Mat &out)  {
    out.create(left.size(), CV_8U);
		Mat unNormalised;
		left.copyTo(unNormalised);

    for (int y = 0; y < left.rows; y++) {
        for (int x = 0; x < left.cols; x++) {
            double result = atan((right.at<double>(y,x)) / left.at<double>(y,x));
            unNormalised.at<double>(y,x) = result;
        }
    }
	normalize(unNormalised, out);
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
    cv::Mat kX = cv::getGaussianKernel(15, -1);
    cv::Mat kY = cv::getGaussianKernel(15, -1);

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
	Mat uX, uY;
	normalize(outputX, uX);
    imwrite("outputX.jpg", uX);
    convolution(blurredImage, kernelY, 3, outputY);
	normalize(outputY, uY);
    imwrite("outputY.jpg", uY);
    calc_gradient_mag(outputX, outputY, gradient_mag);
    calc_gradient_dir(outputX, outputY, gradient_dir);
}

void houghTransform(Mat &image, Mat &gradient_mag, Mat &gradient_dir, double thresh_val)  {
    Mat gradient_thresh, double_mag, double_dir;
    imageToDouble(gradient_mag, double_mag);
    imageToDouble(gradient_dir, double_dir);
    Mat new_gradient = double_mag.mul(double_dir);
    int min = round(sqrt(sqr(image.cols) + sqr(image.rows)));
    Mat hough_space(Size(2 * min, 180), CV_64F, Scalar(0));
    for (int y = 0; y < new_gradient.rows; y++) {
        for (int x = 0; x < new_gradient.cols; x++) {
            if (new_gradient.at<double>(y, x) > 128) {
                for (double t = 0; t < hough_space.rows; t++) {
                    int p = round(x * cos(t * M_PI / 180) + y * sin(t * M_PI / 180)) + min;
                    hough_space.at<double>(t, p) = hough_space.at<double>(t, p) + 1;
                }
            }
        }
    }
    Mat hough_out;
    normalize(hough_space, hough_out);
    imwrite("houghSpace.jpg", hough_out);
}

int main(int argc, char** argv) {
    char* imageName = argv[1];

    Mat image;
    image = imread(imageName, IMREAD_GRAYSCALE);

    if (argc != 2 || !image.data) {
        printf(" No image data \n ");
        return -1;
    }
	Mat grad_mag, grad_dir;
    sobel(image, grad_mag, grad_dir);
	houghTransform(image, grad_mag, grad_dir, 230);

    return 0;
}
