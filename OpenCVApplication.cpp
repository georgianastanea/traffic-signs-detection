#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>
#include <random>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256
#define ACCEPTED_OBJECT_RATIO_MIN 0.8
#define ACCEPTED_OBJECT_RATIO_MAX 1.2

wchar_t* projectPath;

vector<int*> templateImagesHistograms;

int* computeHistogram(Mat_<uchar>img) {
	int* h = (int*)malloc(sizeof(int) * 256);

	for (int i = 0; i < 256; i++) {
		h[i] = 0;
	}

	for (int j = 0; j < img.cols; j++)
	{
		for (int i = 0; i < img.rows; i++)
		{
			if (img(i, j) == 255)
				h[j]++;
		}
	}

	return h;
}

void showHist(int* hist, int height, int pixels, char* name) {
	Mat_<Vec3b> src(height, pixels, 1);
	int max_hist = 0;

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			src(i, j)[0] = 255;
			src(i, j)[1] = 255;
			src(i, j)[2] = 255;
		}
	}

	for (int i = 0; i < pixels; i++)
		if (hist[i] > max_hist)
			max_hist = hist[i];

	double scale = 1.0;
	scale = (double)height / max_hist;

	for (int j = 0; j < pixels; j++) {
		for (int i = 0; i < hist[j] * scale; i++) {
			src(i, j)[0] = 0;
			src(i, j)[1] = 0;
			src(i, j)[2] = 255;
		}
	}
	imshow(name, src);

}

Mat_<uchar> binarizeImage(Mat_<Vec3b> img) {
	Mat_<uchar> result(img.rows, img.cols);
	result.setTo(0);

	for(int i = 0; i < img.rows; i++)
		for (int j = 0; j < img.cols; j++) {
			// thresholding for red
			if(img(i,j)[2] > 200 && img(i,j)[1] < 100 && img(i,j)[0] < 100)
				result(i, j) = 255;
			// thresholding for blue
			if (img(i, j)[0] > 100 && img(i, j)[1] < 100 && img(i, j)[2] < 100)
				result(i, j) = 255;
			// thresholding for yellow
			if (img(i, j)[2] > 200 && img(i, j)[1] > 200 && img(i, j)[0] < 100)
				result(i, j) = 255;
		}
	/*imshow("binarized", result);
	waitKey(0);*/

	return result;
}

void loadTemplateImages() {
	char* path = "C:\\Facultate\\PRS\\proiect\\OpenCVApplication-VS2022_OCV460_basic\\Images\\templates\\*.png";
	vector<String> filenames;

	glob(path, filenames);
	for (int i = 0; i < filenames.size(); i++) {
		Mat_<Vec3b> img = imread(filenames[i], IMREAD_COLOR);
		int* histogram = computeHistogram(binarizeImage(img));
		templateImagesHistograms.push_back(histogram);
	}
}


bool isInside(Mat img, int i, int j){
	if (i < 0 || j < 0)
		return false;
	if (i < img.rows && j < img.cols)
		return true;
	return false;
}


vector<Mat_<uchar>> mapRegionsOfInterest() {
	char fname[MAX_PATH];

	while (openFileDlg(fname)) {
		Mat img = imread(fname, IMREAD_COLOR);
		vector<Mat_<uchar>> mappedRegions;

		Mat_<Vec3b> imgHSV(img.rows, img.cols);
		Mat_<uchar> resultRed(img.rows, img.cols);
		Mat_<uchar> resultBlue(img.rows, img.cols);
		Mat_<uchar> resultYellow(img.rows, img.cols);

		// convert to HSV
		cvtColor(img, imgHSV, COLOR_BGR2HSV);

		// define thresholds for red
		int lower1_H_RED = 0;
		int upper1_H_RED = 10;
		int lower1_S_RED = 100;
		int upper1_S_RED = 255;
		int lower1_V_RED = 20;
		int upper1_V_RED = 255;

		int lower2_H_RED = 160;
		int upper2_H_RED = 179;
		int lower2_S_RED = 100;
		int upper2_S_RED = 255;
		int lower2_V_RED = 20;
		int upper2_V_RED = 255;

		// define thresholds for blue
		int lower1_H_BLUE = 100;
		int upper1_H_BLUE = 140;
		int lower1_S_BLUE = 100;
		int upper1_S_BLUE = 255;
		int lower1_V_BLUE = 20;
		int upper1_V_BLUE = 255;
			
		// define thresholds for yellow
		int lower1_H_YELLOW = 10;
		int upper1_H_YELLOW = 50;
		int lower1_S_YELLOW = 100;
		int upper1_S_YELLOW = 255;
		int lower1_V_YELLOW = 100;
		int upper1_V_YELLOW = 255;


		// Apply thresholds for red
		for (int i = 0; i < imgHSV.rows; i++) {
			for (int j = 0; j < imgHSV.cols; j++) {
				if ((imgHSV(i, j)[0] >= lower1_H_RED && imgHSV(i, j)[0] <= upper1_H_RED && imgHSV(i, j)[1] >= lower1_S_RED && imgHSV(i, j)[1] <= upper1_S_RED && imgHSV(i, j)[2] >= lower1_V_RED && imgHSV(i, j)[2] <= upper1_V_RED) ||
					(imgHSV(i, j)[0] >= lower2_H_RED && imgHSV(i, j)[0] <= upper2_H_RED && imgHSV(i, j)[1] >= lower2_S_RED && imgHSV(i, j)[1] <= upper2_S_RED && imgHSV(i, j)[2] >= lower2_V_RED && imgHSV(i, j)[2] <= upper2_V_RED)) {
					resultRed(i, j) = 255;
				}
				else {
					resultRed(i, j) = 0;
				}
			}
		}
		mappedRegions.push_back(resultRed);

		// Apply thresholds for yellow
		for (int i = 0; i < imgHSV.rows; i++) {
			for (int j = 0; j < imgHSV.cols; j++) {
				if (imgHSV(i, j)[0] >= lower1_H_YELLOW && imgHSV(i, j)[0] <= upper1_H_YELLOW && imgHSV(i, j)[1] >= lower1_S_YELLOW && imgHSV(i, j)[1] <= upper1_S_YELLOW && imgHSV(i, j)[2] >= lower1_V_YELLOW && imgHSV(i, j)[2] <= upper1_V_YELLOW) {
					resultYellow(i, j) = 255;
				}
				else {
					resultYellow(i, j) = 0;
				}
			}
		}
		mappedRegions.push_back(resultYellow);

		// Apply thresholds for blue
		for (int i = 0; i < imgHSV.rows; i++) {
			for (int j = 0; j < imgHSV.cols; j++) {
				if (imgHSV(i, j)[0] >= lower1_H_BLUE && imgHSV(i, j)[0] <= upper1_H_BLUE && imgHSV(i, j)[1] >= lower1_S_BLUE && imgHSV(i, j)[1] <= upper1_S_BLUE && imgHSV(i, j)[2] >= lower1_V_BLUE && imgHSV(i, j)[2] <= upper1_V_BLUE) {
					resultBlue(i, j) = 255;
				}
				else {
					resultBlue(i, j) = 0;
				}
			}
		}
		mappedRegions.push_back(resultBlue);


		imshow("resulted red areas", resultRed);
		imshow("resulted yellow areas", resultYellow);
		imshow("resulted blue areas", resultBlue);
		waitKey();

		return mappedRegions;
	}
}

Mat_<uchar> get_labels(Mat_<uchar> img) {
	int di[] = { -1, 0, 1, 0 };
	int dj[] = { 0, -1, 0, 1 };

	Mat_<uchar> labels(img.rows, img.cols);
	labels.setTo(0);

	int label = 0;
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (img(i, j) == 255 && labels(i, j) == 0) {
				label++;
				queue<Point> q;
				q.push(Point(i, j));
				labels(i, j) = label;

				while (!q.empty()) {
					Point p = q.front();
					q.pop();
					for (int k = 0; k < 4; k++) {
						if (isInside(img, p.x + di[k], p.y + dj[k]) && img(p.x + di[k], p.y + dj[k]) == 255 && labels(p.x + di[k], p.y + dj[k]) == 0) {
							labels(p.x + di[k], p.y + dj[k]) = label;
							q.push(Point(p.x + di[k], p.y + dj[k]));
						}
					}
				}
			}
		}
	}

	// show labels
	Mat afis(img.rows, img.cols, CV_8UC3, Scalar(255, 255, 255));
	std::default_random_engine gen;
	std::uniform_int_distribution<int> d(0, 255);
	for (int k = 1; k <= label; k++) {
		uchar red = d(gen);
		uchar blue = d(gen);
		uchar green = d(gen);
		Vec3b color = Vec3b(blue, green, red);
		for (int i = 1; i < img.rows - 1; i++) {
			for (int j = 1; j < img.cols - 1; j++) {
				if (labels.at<uchar>(i, j) == k)
					afis.at<Vec3b>(i, j) = color;
			}
		}
	}

	return labels;
}

vector<Mat_<uchar>> extractObjects(Mat_<uchar> labels) {
	vector<Mat_<uchar>> objects;
	int maxLabel = INT_MIN;

	for (int i = 0; i < labels.rows; i++) {
		for (int j = 0; j < labels.cols; j++) {
			if (labels(i, j) > maxLabel) {
				maxLabel = labels(i, j);
			}
		}
	}

	for (int k = 1; k <= maxLabel; k++) {
		int minX = INT_MAX;
		int minY = INT_MAX;
		int maxX = INT_MIN;
		int maxY = INT_MIN;

		for (int i = 0; i < labels.rows; i++) {
			for(int j =0; j< labels.cols; j++)
				if (labels(i, j) == k) {
					if(i < minY)
						minY = i;
					if (i > maxY)
						maxY = i;
					if (j < minX)
						minX = j;
					if (j > maxX)
						maxX = j;
				}
		}

		Mat_<uchar> object(maxY - minY + 1, maxX - minX + 1);
		object.setTo(0);
		int area = 0;

		for (int i = minY; i <= maxY; i++) {
			for (int j = minX; j <= maxX; j++) {
				if (labels(i, j) == k) {
					object(i - minY, j - minX) = 255;
					area++;
				}
			}
		}

		float ratio = (float)object.rows / (float)object.cols;
		if (ratio > ACCEPTED_OBJECT_RATIO_MIN && ratio < ACCEPTED_OBJECT_RATIO_MAX && area > 2000)
		{
			objects.push_back(object);
			imshow("extracted obj ", object);
			waitKey(0);
		}
	}

	return objects;
}

int histogramSimilarityScore(int* templateHist, int* imageHist) {
	int score = 0;

	for (int i = 0; i < 256; i++) {
		score += abs(templateHist[i] - imageHist[i]);
	}

	return score;
}


String getImageName(int index) {
	char* path = "C:\\Facultate\\PRS\\proiect\\OpenCVApplication-VS2022_OCV460_basic\\Images\\templates\\*.png";
	vector<String> filenames;

	glob(path, filenames);
	// get filename from path
	return filenames[index].substr(filenames[index].find_last_of("\\") + 1);
}


int main() 
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
    projectPath = _wgetcwd(0, 0);

	loadTemplateImages();

	vector<Mat_<uchar>> mappedRegions = mapRegionsOfInterest();
	vector<Mat_<uchar>> objects;

	// get red objects
	Mat_<uchar> convertedRedImage = mappedRegions[0];
	Mat_<uchar> redLabels = get_labels(convertedRedImage);
	vector<Mat_<uchar>> redObjects = extractObjects(redLabels);
	objects.insert(objects.end(), redObjects.begin(), redObjects.end());

	// get yellow objects
	Mat_<uchar> convertedYellowImage = mappedRegions[1];
	Mat_<uchar> yellowLabels = get_labels(convertedYellowImage);
	vector<Mat_<uchar>> yellowObjects = extractObjects(yellowLabels);
	objects.insert(objects.end(), yellowObjects.begin(), yellowObjects.end());

	// get blue objects
	Mat_<uchar> convertedBlueImage = mappedRegions[2];
	Mat_<uchar> blueLabels = get_labels(convertedBlueImage);
	vector<Mat_<uchar>> blueObjects = extractObjects(blueLabels);
	objects.insert(objects.end(), blueObjects.begin(), blueObjects.end());


	for (int i = 0; i < objects.size(); i++) {
		float minScore = INT_MAX;
		int resultIndex = -1;

		// resize object to template size
		Mat_<uchar> resizedObject;
		resize(objects[i], resizedObject, Size(IMAGE_WIDTH, IMAGE_HEIGHT));
		imshow("resized object", resizedObject);
		waitKey(0);

		// compute histogram
		int* objectHistogram = computeHistogram(resizedObject);

		for (int j = 0; j < templateImagesHistograms.size(); j++) {
			// compute score
			int score = histogramSimilarityScore(templateImagesHistograms[j], objectHistogram);

			if (score < minScore) {
				minScore = score;
				resultIndex = j;
			}
		}
		String resultedSign = getImageName(resultIndex);
		resultedSign = resultedSign.substr(0, resultedSign.find_last_of("."));

		cout << "Object is most likely a " << resultedSign << " sign"<< endl;
	}

	return 0;
}