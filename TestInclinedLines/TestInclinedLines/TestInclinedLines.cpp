// TestInclinedLines.cpp : Defines the entry point for the console application.
//

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\contrib\contrib.hpp"

#include "StringFcLib.h"

#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	string FolderName, FileNameBase, FileNameExtension;
	FolderName = "D:\\Testimages\\DirectionalityExtensiveTest\\";
	FileNameBase = "Lines001";
	FileNameExtension = ".tif";
	bool saveResult = 0;
	bool displayResult = 1;

	int spacingX = 32;
	int spacingY = 90;
	int offsetX = 0;
	int offsetY = 0;

	int intensityBright = 65535;
	int intensityDark = 0;

	int noiseMean = 0;
	int noiseSTD = 1;


	int maxX, maxY;
	maxX = 256;
	maxY = 256;

	for (int size = 1; size <= 1; size++)
	{
		int rectSizeX = size - 1;
		int rectSizeY = 32;
		Mat Im = Mat::ones(maxY, maxX, CV_16S)*intensityDark;
		for (int x = (maxX* -1); x < maxX; x += 5)
		{
			Point start = Point(x, 0);
			Point stop = Point(start.x +59, maxY);
			line(Im, start, stop, intensityBright);
		}
		Mat ImNoise = Mat::zeros(maxY, maxX, CV_16S);
		randn(ImNoise, noiseMean, noiseSTD);
		Im += ImNoise;
		Im.convertTo(Im, CV_16U);
		if (saveResult)
		{
			string FileName = FolderName + FileNameBase + "S" + ItoStrLZ(size, 3) + FileNameExtension;
			imwrite(FileName, Im);
		}
		if (displayResult)
		{
			namedWindow("Image", WINDOW_AUTOSIZE);
			imshow("Image", Im);
			waitKey(0);
		}
	}
	return 0;
}

