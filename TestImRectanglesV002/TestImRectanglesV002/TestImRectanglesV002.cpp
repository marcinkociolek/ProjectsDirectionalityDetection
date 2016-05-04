// TestImRectanglesV002.cpp : Defines the entry point for the console application.
// multiple rectangles on image
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
	FolderName = "C:\\EigenTexturesData\\ForHaralick001\\";
	FileNameBase = "Rectange008";
	FileNameExtension = ".tif";
	bool saveResult = 0;
	bool displayResult = 1;

	int spacingX = 32;
	int spacingY = 90;
	int offsetX = 0;
	int offsetY = 0;

	int maxX, maxY;
	maxX = 256;
	maxY = 256;

	for (int size = 1; size <= 32; size++)
	{
		int rectSizeX = size -1;
		int rectSizeY = 32;
		Mat Im = Mat::zeros(maxY, maxX, CV_16U);
		for (int y = offsetY; y < maxY - offsetY; y += spacingY)
		{
			for (int x = offsetX; x < maxX - offsetX; x += spacingX)
			{
				Point start = Point(x, y);
				Point stop = Point(start.x + rectSizeX, start.y + rectSizeY);
				rectangle(Im, start, stop, 65535, CV_FILLED);
			}
		}
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

