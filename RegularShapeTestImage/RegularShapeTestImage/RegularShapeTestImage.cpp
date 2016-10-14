// RegularShapeTestImage.cpp : Defines the entry point for the console application.
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
	FolderName = "C:\\Data\\GroundTrueForTextFeat\\Circles\\"; // folder where to save outpute test images (use double slashes, even at the end)
	FileNameBase = "Test"; // beginning of test file name
	FileNameExtension = ".tif"; // test file name extension

	bool saveResult = 1; // 1 --> files saved
	bool displayResult = 1;

	int shape = 1;
	// the resulting image size if always half of these values, if cropping is on (see above)
	int maxX = 1024;
	int maxY = 1024;

	int shapeSizeX = 8;
	int shapeSizeY = 8;

	int shapeSpacingX = 12;
	int shapeSpacingY = 12;

	int shapeOffsetX = shapeSpacingX / 2 + 1;
	int shapeOffsetY = shapeSpacingX / 2 + 1;

	unsigned short bacgroundIntensity = 65535 / 4;
	unsigned short foregroundIntensity = bacgroundIntensity * 3;


	Mat Im = Mat::ones(maxX, maxY, CV_16U) * bacgroundIntensity;
	for (int y = shapeOffsetY; y < maxY - shapeOffsetY; y += shapeSpacingY)
	{
		for (int x = shapeOffsetX; x < maxX - shapeOffsetY; x += shapeSpacingX)
		{
			
			switch (shape)
			{
			case 1:
				{
					Point PointCenter(x, y);
					ellipse(Im, PointCenter, Size(shapeSizeX / 2, shapeSizeY / 2), 0.0, 0.0, 360.0, foregroundIntensity, -1);
					break;
				}
			default:
				{
					Point PointStart(x - shapeSizeX / 2, y - shapeSizeY / 2);
					Point PointStop(x - shapeSizeX / 2 + shapeSizeX - 1, y - shapeSizeY / 2 + shapeSizeY - 1);
					rectangle(Im, PointStart, PointStop, foregroundIntensity, -1);
					break;
				}
			}
		}
	}

	imshow("Image", ShowImage16PseudoColor(Im,0.0,65535.0));

	waitKey(0);

	FileName = FolderName;
	FileName += FileNameBase;
	switch(shape)
	{
	case 1:
		FileName += "Ellipse";
		break;
	default:
		FileName += "Rectangle";
		break;
	}
	FileName += "Size";
	FileName += to_string(shapeSizeY);
	FileName += "x";
	FileName += to_string(shapeSizeX);
	FileName += "Spacing";
	FileName += to_string(shapeSizeY);
	FileName += "x";
	FileName += to_string(shapeSizeX);

	FileName += FileNameExtension;

		imwrite(FileName, Im);


}

