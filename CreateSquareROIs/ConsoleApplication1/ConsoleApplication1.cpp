// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"

#include <boost\filesystem.hpp>
#include <boost/regex.hpp>

#include <iostream>
#include <fstream>


#include "..\..\..\ProjectsLib\LibMarcin\Functions.h"
#include "..\..\..\ProjectsLib\LibMarcin\NormalizationLib.h"
#include "..\..\..\ProjectsLib\LibMarcin\HaralickLib.h"
#include "..\..\..\ProjectsLib\LibMarcin\RegionU16Lib.h"

#include "..\..\..\ProjectsLib\LibMarcin\ParamFromXML.h"
#include "..\..\..\ProjectsLib\LibMarcin\DispLib.h"
#include "..\..\..\ProjectsLib\LibMarcin\StringFcLib.h"

//#include <tinyxml.h>
//#include <tinystr.h>

#define PI 3.14159265

using namespace cv;
using namespace std;
using namespace boost;
using namespace boost::filesystem;


//const int stepNr = 180;


int main(int argc, char* argv[])
{
	std::string FileName, FolderName, FileNameBase, FileNameExtension;
	FolderName = "C:\\Data\\GroundTrueForTextFeat\\ROIs512x512r51x51xCount361\\"; // folder where to save outpute test images (use double slashes, even at the end)
	FileNameBase = "ROI"; // beginning of test file name
	FileNameExtension = ".tif"; // test file name extension

	bool saveResult = 1; // 1 --> files saved
	bool displayResult = 1;

	// the resulting image size if always half of these values, if cropping is on (see above)
	int maxX = 512;
	int maxY = 512;

	int roiSizeX = 51;
	int roiSizeY = 51;

	int roiOffsetX = 1;
	int roiOffsetY = 1;

	int roiStepX = 51;
	int roiStepY = 51;

	int roiCount = 361;

	unsigned short roiIndex = 1;

	Mat ROI = Mat::zeros(maxX, maxY, CV_16U);
	for (int y = roiOffsetY; y < maxY + 1- roiSizeY; y += roiStepY)
	{
		for (int x = roiOffsetX; x < maxX + 1 - roiSizeX; x += roiStepX)
		{
			rectangle(ROI, Point(x, y), Point(x + roiSizeX, y + roiSizeY), roiIndex, -1);

			roiIndex++;
		}
	}
	
	imshow("Image", ShowRegion(ROI));

	waitKey(0);
	
	for (int i = 0; i < roiCount; i++)
	{
		FileName = FolderName;
		FileName += FileNameBase;
		FileName += "Size";
		FileName += to_string(roiSizeY);
		FileName += "x";
		FileName += to_string(roiSizeY);
		FileName += "Nr";
		FileName += ItoStrLZ(i,3);
		FileName += FileNameExtension;

		imwrite(FileName, ROI);

	}

}

