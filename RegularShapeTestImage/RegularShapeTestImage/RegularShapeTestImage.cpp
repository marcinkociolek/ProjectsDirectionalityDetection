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
	FolderName = "C:\\Data\\GroundTrueForTextFeat\\CirclesSpacing12Size8RotatedAvgBlur3Tiled\\"; // folder where to save outpute test images (use double slashes, even at the end)
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

	bool rotateImage = 1; // Image rotation: 1-->yes; 0-->no
	float rotationAngleStart = 0;
	float rotationAngleStop = 180;
	float rotationAngleStep = 1;

	bool averageBlur = 1; // blur kernel if going to have constant value (1/number of pixels in the kernel)
	int kernelSize = 3;
	bool cropImage = 1; // Crop image after rotation: 1-->yes; 0-->no

	// section below for tiled images
	bool makeTiles = 1;
	int tileSizeX = 51;
	int tileSizeY = 51;

	int tileOffsetX = 26;
	int tileOffsetY = 26;

	int tileStepX = 51;
	int tileStepY = 51;
	// end of tile section



	unsigned short bacgroundIntensity = 65535 / 4;
	unsigned short foregroundIntensity = bacgroundIntensity * 3;
	
	if (!rotateImage)
		rotationAngleStop = 1;
	//iteration from here
	for (float rotationAngle = rotationAngleStart; rotationAngle <= rotationAngleStop; rotationAngle += rotationAngleStep)
	{

		Mat Im = Mat::ones(maxX, maxY, CV_32F) * ((float)bacgroundIntensity);
		Mat ImSave;
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
		//Rotation of image
		if (rotateImage)
		{

			Point rotationCenter = Point(Im.cols / 2, Im.rows / 2);
			Mat rotationMatrix = getRotationMatrix2D(rotationCenter, rotationAngle, 1);
			warpAffine(Im, Im, rotationMatrix, Im.size());
		}


		if (averageBlur)//(averageBlur && k)
		{
			Mat ImOut;
			//kernelSize = k * 2 + 1;
			
			float kernelPixelCount = kernelSize * kernelSize;
			Mat Kernel = Mat::ones(kernelSize, kernelSize, CV_32F) / kernelPixelCount;
			filter2D(Im, ImOut, -1, Kernel);

			Im = ImOut;
		}

		if (cropImage)
		{
			// croped image by chalf
			Im = Im.rowRange(Im.rows / 4, Im.rows / 4 * 3);
			Im = Im.colRange(Im.cols / 4, Im.cols / 4 * 3);
		}

		Im.convertTo(ImSave, CV_16U);
		imshow("Image", ShowImage16PseudoColor(ImSave, 0.0, 65535.0));

		waitKey(50);

		if (makeTiles)
		{
			for (int yTile = tileOffsetY; yTile < maxY / 2 - tileSizeY / 2; yTile += tileStepY)
				for (int xTile = tileOffsetX; xTile < maxX / 2- tileSizeX / 2; xTile += tileStepX)
				{
					Point PointStart(xTile - tileSizeX / 2, yTile - tileSizeY / 2);
					Point PointStop(xTile - tileSizeX / 2 + tileSizeX, yTile - tileSizeY / 2 + tileSizeY);
					Mat ImTile;
					ImSave(Rect(PointStart, PointStop)).copyTo(ImTile);

					FileName = FolderName;
					FileName += FileNameBase;
					switch (shape)
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
					FileName += to_string(shapeSpacingY);
					FileName += "x";
					FileName += to_string(shapeSpacingX);

					if (averageBlur)
						FileName += "AvgBlurKernel" + ItoStrLZ(kernelSize, 2);

					if (rotateImage)
						FileName += "Angle" + ItoStrLZ(rotationAngle, 3);
					else
						FileName += "Angle" + ItoStrLZ(0, 3);



					FileName += "Tile" + ItoStrLZ(yTile / tileStepY, 3) + "x" + ItoStrLZ(xTile / tileStepX, 3);

					FileName += FileNameExtension;

					imwrite(FileName, ImTile);
					ImTile.release();
				}

		}
		else
		{
			FileName = FolderName;
			FileName += FileNameBase;
			switch (shape)
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
			FileName += to_string(shapeSpacingY);
			FileName += "x";
			FileName += to_string(shapeSpacingX);

			if (averageBlur)
				FileName += "AvgBlurKernel" + ItoStrLZ(kernelSize, 2);

			if (rotateImage)
				FileName += "Angle" + ItoStrLZ(rotationAngle, 3);
			else
				FileName += "Angle" + ItoStrLZ(0, 3);

			FileName += FileNameExtension;

			imwrite(FileName, ImSave);
		}
		Im.release();
		ImSave.release();
	}

	return 0;

}

