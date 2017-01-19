// KernelBasedTestImages.cpp : Defines the entry point for the console application.
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
	string FileName, FolderName, BaseFolderName, FileNameBase, FileNameExtension;
	BaseFolderName = "C:\\Data\\ExtensiveDirectionalityTest\\InData\\Test\\";

	FileNameExtension = ".tif";

	bool saveResult = 1;
	bool displayResult = 0;

	bool addNoise = 1;
	bool averageBlur = 0;

	bool gausBlur = 0;

	int kernelShape = 0; //0 - rectangle; 1 - ellipse 
	int kernelMax = 41;
	int shapeSizeX = 8;
	int shapeSizeY = 16;


	float intensityBright	= 65535.0 / 4.0 * 3.0;
	float intensityDark		= 65535.0 / 4.0;
	float intensityRange	= intensityBright - intensityDark;

	int noiseMean = 0;
	int noiseSDTBase = 2000;

	bool rotateImage = 0; // Image rotation: 1-->yes; 0-->no
	
	float rotationAngleStart = 0;
	float rotationAngleStop = 90;
	float rotationAngleStep = 1;


	bool cropImage = 1;
	int kernelRotationAngle = 0;

	int iterNr = 10;

	int maxX = 1024;
	int maxY = 1024;

	//int offset = 0;

	// dot matrix definitiom
	int spacing = 20;

	int stepX = spacing;
	int stepY = spacing;

	int offsetX = stepX/2;
	int offsetY = stepY/2;

	// section below for tiled images
	bool makeTiles = 0;
	int tileSizeX = 61;
	int tileSizeY = 61;

	int tileOffsetX = 31;
	int tileOffsetY = 31;

	int tileStepX = 45;
	int tileStepY = 45;
	// end of tile section

	//donot change
	int noiseSTD = 0;
	int kernelSize = 0;

	Mat Kernel = Mat::zeros(kernelMax, kernelMax, CV_32F);

	switch (kernelShape)
	{
	case 1:
		ellipse(Kernel, Point(kernelMax / 2, kernelMax / 2), Size(shapeSizeX / 2, shapeSizeY / 2), 0.0, 0.0, 360.0, 1, -1);
		break;
	default:
		rectangle(Kernel, Rect(kernelMax / 2 - shapeSizeX / 2, kernelMax / 2 - shapeSizeY / 2, shapeSizeX, shapeSizeY), 1.0, -1);
		break;
	}
	Point rotationCenter = Point(kernelMax / 2, kernelMax / 2);

	
	imshow("Kernel", ShowImageF32PseudoColor(Kernel, 0.0, 1.0));
	
	if (displayResult)
		namedWindow("imageOut");
	//setMouseCallback("imageOut", onMouse, 0);

	//Mat ImOut;
	// iteration from here
	for (int k = 0; k <= iterNr; k++)
	{
		FolderName = BaseFolderName;
		if (makeTiles)
			FolderName += "Tiled";
		
		switch (kernelShape)
		{
		case 1:
			FolderName += "Elip";
			break;
		default:
			FolderName += "Rect";
			break;
		}
		
		FolderName += "A" + to_string(int(rotationAngleStart)) + "-" + to_string(int(rotationAngleStop));

		FolderName += "S" + ItoStrLZ(spacing, 2) + "T" + ItoStrLZ(shapeSizeX, 2) + "L" + ItoStrLZ(shapeSizeY, 2);
		if (addNoise)
			FolderName += "N" + ItoStrLZ(k * noiseSDTBase, 5);
		if (averageBlur)
			FolderName += "B" + ItoStrLZ(k * 2 + 1, 2);
		FolderName += "\\";
		create_directory(path(FolderName));

		for (float rotationAngle = rotationAngleStart; rotationAngle <= rotationAngleStop; rotationAngle += rotationAngleStep)

		{
			Mat Im;
			Im = Mat::zeros(maxX, maxY, CV_32F);

			for (int y = offsetY; y < maxY - offsetY; y += stepY)
			{
				for (int x = offsetX; x < maxX - offsetX; x += stepX)
				{
					Im.at<float>(y, x) = (float)intensityRange;
				}
			}
			if (!rotateImage)
			{
				kernelRotationAngle = rotationAngle;
			}
			Mat rotationMatrix = getRotationMatrix2D(rotationCenter, kernelRotationAngle, 1);
			Mat KernelRotated;
			warpAffine(Kernel, KernelRotated, rotationMatrix, Kernel.size());

			filter2D(Im, Im, -1, KernelRotated);
			Im += intensityDark;

			if (rotateImage)
			{
				Point rotationCenter = Point(Im.cols / 2, Im.rows / 2);
				Mat rotationMatrix = getRotationMatrix2D(rotationCenter, rotationAngle, 1);
				warpAffine(Im, Im, rotationMatrix, Im.size());
			}

			if (addNoise && k)
			{
				noiseSTD = k * noiseSDTBase;
				Mat ImNoise = Mat::zeros(Im.rows, Im.cols, CV_32F);
				randn(ImNoise, noiseMean, noiseSTD);
				Im += ImNoise;
				//Im.convertTo(Im, CV_16U);
			}

			if (gausBlur && k)
			{
				Mat ImOut;
				kernelSize = k * 2 + 1;
				GaussianBlur(Im, ImOut, Size(kernelSize, kernelSize), 10000, 10000);
				Im = ImOut;
			}
			if (averageBlur && k)
			{
				Mat ImOut;
				kernelSize = k * 2 + 1;
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

			Mat ImSave;
			Im.convertTo(ImSave, CV_16U);
			if (displayResult)
			{
				imshow("imageOut", ShowImage16PseudoColor(ImSave, 0, 65535));
				waitKey(200);
			}
			if (saveResult)
			{
				FileName = FolderName;
				FileName += "";
				switch (kernelShape)
				{
				case 1:
					FileName += "Elipse";
					break;
				default:
					FileName += "Rect";
					break;
				}
				FileName += "L" + ItoStrLZ(shapeSizeY, 2);
				FileName += "T" + ItoStrLZ(shapeSizeX, 2);

				FileName += "A" + ItoStrLZ(rotationAngle, 3);
				FileName += "S" + ItoStrLZ(spacing, 2);

				//FileName += "A" + ItoStrLZ(rotationAngle, 3);

				if (addNoise)
					FileName += "NoiseSTD" + ItoStrLZ(noiseSTD, 5);

				if (gausBlur)
					FileName += "BlurKrnl" + ItoStrLZ(kernelSize, 2);

				if (averageBlur)
					FileName += "BlurKrnl" + ItoStrLZ(kernelSize, 2);

				if (makeTiles)
				{
					for (int yTile = tileOffsetY; yTile < maxY / 2 - tileSizeY / 2; yTile += tileStepY)
						for (int xTile = tileOffsetX; xTile < maxX / 2 - tileSizeX / 2; xTile += tileStepX)
						{
							Point PointStart(xTile - tileSizeX / 2, yTile - tileSizeY / 2);
							Point PointStop(xTile - tileSizeX / 2 + tileSizeX, yTile - tileSizeY / 2 + tileSizeY);
							Mat ImTile;
							ImSave(Rect(PointStart, PointStop)).copyTo(ImTile);

							string TileFileName = FileName;
							TileFileName += "Tile" + ItoStrLZ(yTile / tileStepY, 3) + "x" + ItoStrLZ(xTile / tileStepX, 3);

							TileFileName += FileNameExtension;

							imwrite(TileFileName, ImTile);
							ImTile.release();
						}
				}
				else
				{
					FileName += FileNameExtension;
					imwrite(FileName, ImSave);
				}
			}
		}
	}
	return 0;
}

