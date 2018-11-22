#include <stdio.h>
#include <opencv/cv.hpp>
#include <math.h>

using namespace cv;

void convolution(cv::Mat &input, Mat &kernel, int size, cv::Mat &blurredOutput) {
	// intialise the output using the input
	blurredOutput.create(input.size(), input.type());

	// create the Gaussian kernel in 1D
	// cv::Mat kX = cv::getGaussianKernel(size, -1);
	// cv::Mat kY = cv::getGaussianKernel(size, -1);

	// make it 2D multiply one by the transpose of the other
	// cv::Mat kernel = kX * kY.t();

	//CREATING A DIFFERENT IMAGE kernel WILL BE NEEDED
	//TO PERFORM OPERATIONS OTHER THAN GUASSIAN BLUR!!!

	// we need to create a padded version of the input
	// or there will be border effects
	int kernelRadiusX = ( kernel.size[0] - 1 ) / 2;
	int kernelRadiusY = ( kernel.size[1] - 1 ) / 2;

	cv::Mat paddedInput;
	cv::copyMakeBorder( input, paddedInput,
		kernelRadiusX, kernelRadiusX, kernelRadiusY, kernelRadiusY,
		cv::BORDER_REPLICATE );

	// now we can do the convoltion
	for ( int i = 0; i < input.rows; i++ )
	{
		for( int j = 0; j < input.cols; j++ )
		{
			double sum = 0.0;
			for( int m = -kernelRadiusX; m <= kernelRadiusX; m++ )
			{
				for( int n = -kernelRadiusY; n <= kernelRadiusY; n++ )
				{
					// find the correct indices we are using
					int imagex = i + m + kernelRadiusX;
					int imagey = j + n + kernelRadiusY;
					int kernelx = m + kernelRadiusX;
					int kernely = n + kernelRadiusY;

					// get the values from the padded image and the kernel
					int imageval = ( int ) paddedInput.at<uchar>( imagex, imagey );
					double kernalval = kernel.at<double>( kernelx, kernely );

					// do the multiplication
					sum += imageval * kernalval;
				}
			}
			// set the output value as the sum of the convolution
			blurredOutput.at<uchar>(i, j) = (uchar) sum;
		}
	}
}

void calc_gradient_mag(Mat &left, Mat &right, Mat &out) {

  out.create(left.size(), left.type());

  for (int y = 0; y < left.rows; y++) {
    for (int x = 0; x < left.cols; x++) {
      int result = (int) sqrt( pow((int)left.at<uchar>(y,x),2) + pow((int)right.at<uchar>(y,x),2) );
      out.at<uchar>(y,x) = (uchar) result;
    }
  }
}

void calc_gradient_dir(Mat &left, Mat &right, Mat &out) {

  out.create(left.size(), left.type());

  for (int y = 0; y < left.rows; y++) {
    for (int x = 0; x < left.cols; x++) {
      double result = atan(((double)right.at<uchar>(y,x)) / ((double)left.at<uchar>(y,x))) - M_PI/2;
      out.at<uchar>(y,x) = (uchar) result;
    }
  }
}

void sobel(Mat &image) {
  // // create the Gaussian kernel in 1D
  cv::Mat kX = cv::getGaussianKernel(3, -1);
  cv::Mat kY = cv::getGaussianKernel(3, -1);

  // // make it 2D multiply one by the transpose of the other
  cv::Mat kernelG = kX * kY.t();

  Mat kernelX = (Mat_<double>(3,3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
  Mat kernelY = (Mat_<double>(3,3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);

  Mat outputX, outputY, gradient_mag, gradient_dir;
  convolution(image, kernelG, 3, image);
  convolution(image, kernelX, 3, outputX);
  convolution(image, kernelY, 3, outputY);
  calc_gradient_mag(outputX, outputY, gradient_mag);
  imwrite("outputMag.jpg", gradient_mag);
  calc_gradient_dir(outputX, outputY, gradient_dir);
  imwrite("outputDir.jpg", gradient_dir);
}

int main(int argc, char** argv) {
 char* imageName = argv[1];

 Mat image;
 image = imread(imageName, IMREAD_GRAYSCALE);

 if (argc != 2 || !image.data) {
   printf(" No image data \n ");
   return -1;
 }

 sobel(image);

 // imwrite("output.jpg", image);

 return 0;
}
