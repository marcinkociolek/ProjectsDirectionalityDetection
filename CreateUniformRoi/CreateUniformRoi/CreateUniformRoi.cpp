// CreateUniformRoi.cpp : Defines the entry point for the console application.
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
	FolderName = "E:\\ROIcollections\\ROIs512x512c51x51xCount181\\"; // folder where to save outpute test images (use double slashes, even at the end)
	FileNameBase = "ROI"; // beginning of test file name
	FileNameExtension = ".tif"; // test file name extension

	bool saveResult = 1; // 1 --> files saved
	bool displayResult = 1;
	bool writeColor = 0;

	int roiShape = 0;
	// the resulting image size if always half of these values, if cropping is on (see above)
	int maxX = 512;
	int maxY = 512;

	int roiSizeX = 51;
	int roiSizeY = 51;

	int roiOffsetX = 26;
	int roiOffsetY = 26;

	int roiStepX = 51;
	int roiStepY = 51;

	int roiCount = 181;

	unsigned short roiIndex = 1;

	Mat ROI = Mat::zeros(maxX, maxY, CV_16U);
	for (int y = roiOffsetY; y < maxY - roiSizeY/2; y += roiStepY)
	{
		switch (roiShape)
		{
		case 1:
			for (int x = roiOffsetX; x < maxX - roiSizeX / 2; x += roiStepX)
			{
				Point PointCenter(x, y);
				ellipse(ROI, PointCenter, Size(roiSizeX / 2, roiSizeY / 2), 0.0, 0.0, 360.0, roiIndex, -1);

				roiIndex++;
			}
			break;
		default:
			for (int x = roiOffsetX; x < maxX - roiSizeX / 2; x += roiStepX)
			{
				Point PointStart(x - roiSizeX / 2, y - roiSizeY / 2);
				Point PointStop(x - roiSizeX / 2 + roiSizeX - 1, y - roiSizeY / 2 + roiSizeY - 1);

				rectangle(ROI, PointStart, PointStop, roiIndex, -1);

				roiIndex++;
			}
			break;
		}
	}

	imshow("Image", ShowRegion(ROI));
	waitKey(50);
	

	for (int i = 0; i < roiCount; i++)
	{
		FileName = FolderName;
		FileName += FileNameBase;
		switch (roiShape)
		{
		case 1:
			FileName += "Cir";
			break;
		default:
			FileName += "Rect";
			break;
		}
		
		FileName += "Size";
		FileName += to_string(roiSizeY);
		FileName += "x";
		FileName += to_string(roiSizeY);
		FileName += "Nr";
		FileName += ItoStrLZ(i, 3);
		
		if (writeColor)
			imwrite(FileName + ".bmp", ShowRegion(ROI));
		FileName += FileNameExtension;

		imwrite(FileName, ROI);
		

	}

}

