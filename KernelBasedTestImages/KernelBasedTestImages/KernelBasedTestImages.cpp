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
Mat ImOut;

static void onMouse(int event, int x, int y, int, void*)
{
	if (event != EVENT_LBUTTONDOWN)
		return;
	cout << "x = " << x << "\ty = " << y << "\tval = " << ImOut.at<unsigned short>(x,y) << "\n";
}


int main(int argc, char* argv[])
{
	string FileName, FolderName, FileNameBase, FileNameExtension;
	FolderName = "C:\\Data\\GroundTrueForTextFeat\\Circles\\";
	FileNameBase = "Circles";
	FileNameExtension = ".tif";

	bool saveResult = 1;
	bool displayResult = 1;
	bool addNoise = 0;
	bool gausBlur = 0;
	bool averageBlur = 0;

	int kernelShape = 1; //0 - rectangle; 1 - ellipse 

	//int barTickness = 16;
	//int barFrequency = 32;

	float intensityBright	= 65535.0 / 4.0 *3;
	float intensityDark		= 65535.0 / 4.0;
	float intensityRange	= intensityBright - intensityDark;

	int noiseMean = 0;
	int noiseSDTBase = 2000;

	bool rotateImage = 0;
	int rotationAngle = 41;
	bool cropImage = 0;
	int kernelRotationAngle = 0;

	int iterNr = 1;

	int maxX = 2048;
	int maxY = 2048;

	//int offset = 0;

	// dot matrix definitiom
	int spacing = 12;

	int stepX = spacing;
	int stepY = spacing;

	int offsetX = stepX/2;
	int offsetY = stepY/2;

	//donot change
	int noiseSTD = 0;
	int kernelSize = 0;

	int kernelMax = 41;
	int shapeSizeX =7;
	int shapeSizeY = 7;

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
	Mat rotationMatrix = getRotationMatrix2D(rotationCenter, kernelRotationAngle, 1);
	warpAffine(Kernel, Kernel, rotationMatrix, Kernel.size());
	
	imshow("Kernel", ShowImageF32PseudoColor(Kernel, 0.0, 1.0));

	namedWindow("imageOut");
	setMouseCallback("imageOut", onMouse, 0);

	// iteration from here
	for (int k = 0; k < iterNr; k++)
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
		filter2D(Im, Im, -1, Kernel);
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



		//Mat ImOut;
		Im.convertTo(ImOut, CV_16U);
		imshow("imageOut", ShowImage16PseudoColor(ImOut, 0, 65535));


		if (saveResult)
		{
			FileName = FolderName + FileNameBase;
			FileName += "Bars";
			FileName += "SizeY" + ItoStrLZ(shapeSizeY, 2);
			FileName += "eSizeX" + ItoStrLZ(shapeSizeX, 2);
			switch(kernelShape)
			{
			case 1:
				FileName += "Elipse";
				break;
			default:
				FileName += "Rectangle"; 
				break;
			}
			
			FileName += "KernelAngle" + ItoStrLZ(kernelRotationAngle, 3);

			FileName += "Spacing" + ItoStrLZ(spacing, 2);

			if (rotateImage)
				FileName += "Angle" + ItoStrLZ(rotationAngle, 3);

			if (addNoise)
				FileName += "NoiseSTD" + ItoStrLZ(noiseSTD, 5);

			if (gausBlur)
				FileName += "BlurKernel" + ItoStrLZ(kernelSize, 2);

			if (averageBlur)
				FileName += "BlurKernel" + ItoStrLZ(kernelSize, 2);

			FileName += FileNameExtension;

			imwrite(FileName, ImOut);
		}


		waitKey(2000);
	}
		/*
		Point start, stop;
		for (int x = offset; x < Im.rows; x += 1)
		{
			if (x%barFrequency < barTickness)
			{

				start.x = x;
				start.y = 0;

				stop.x = x;
				stop.y = maxY;
				line(Im, start, stop, (float)intensityBright, 1);
			}
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

	}
	*/
	return 0;
}

