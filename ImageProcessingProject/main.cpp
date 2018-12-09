#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

void convertFromRGB2GrayScale(Mat& image) {
	cvtColor(image, image, cv::COLOR_RGB2GRAY);
}

string findColor(Scalar bgr) {
	string name;
	int r = bgr[2];
	int g = bgr[1];
	int b = bgr[0];
	if (r < 5 && g < 5 && b < 5) {
		name = "Black";
	}
	else if (r > 150 && g < 30 && b < 30) {
		name = "Red";
	}
	else if (r > 160 && g > 150 && b < 5) {
		name = "Yellow";
	}
	else if (r < 10 && g > 50 && b > 120) {
		name = "Blue";
	}
	else if (r < 10 && g > 90 && b > 20) {
		name = "Green";
	}
	else {
		name = "White";
	}
	return name;
}

string findSize(double pixel) {
	string size;
	if (pixel < 8000) {
		size = "2x1";
	}else if (pixel < 14000) {
		size = "2x2";
	}
	else if (pixel < 20000) {
		size = "3x2";
	}
	else if (pixel < 26000) {
		size = "4x2";
	}
	else if (pixel < 36000) {
		size = "6x2";
	}
	
	return size;
}

int main(int argc, char **argv)
{
	Mat image, original;
	image = imread("C:/Users/Riccardo/Desktop/3Anno/Multimedia/img-serie-5/img-serie-5/lego.png", CV_LOAD_IMAGE_COLOR); // Read the file
	original = image;
	if (!image.data) // Check for invalid input
	{
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	
	
	// 1. convert in grayscale and compute a threshold
	convertFromRGB2GrayScale(image);

	// Set threshold and maxValue
	double thresh = 247;
	double maxValue = 255;
	threshold(image, image, thresh, maxValue, THRESH_BINARY_INV);

	// 2. filter the noise by using one or more morphological filters
	int erosion_size = 1;
	Mat element = getStructuringElement(MORPH_OPEN, Size(2 * erosion_size + 1, 2 * erosion_size + 1),	Point(erosion_size, erosion_size));	
	erode(image, image, element);

	// 3. find out how to use the findContours algorithm and use it to segment the lego blocks

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(image, contours, hierarchy, CV_RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE);
	
	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = boundingRect(Mat(contours_poly[i]));

		// find barycenter
		int x = boundRect[i].x + (boundRect[i].width) / 2;
		int y = boundRect[i].y + (boundRect[i].height) / 2;

		// find color of barycenter
		Scalar bgr = original.at<Vec3b>(Point(x, y));

		// draw label 
		putText(original, findColor(bgr) + " " + findSize(contourArea(contours[i])), Point(x, y-90), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 0));
	}

	// 5. put a text label near each block with its properties (size & color)
	cv::imshow("image", original);

	cv::waitKey(0);
	return 0;
}

