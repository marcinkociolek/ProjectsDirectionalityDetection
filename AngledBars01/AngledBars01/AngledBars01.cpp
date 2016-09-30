// AngledBars01.cpp : Defines the entry point for the console application.
//

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"

#include "math.h"
#include <iostream>
#include <fstream>
//#include "Functions.h"
#include "..\..\..\ProjectsLib\LibMarcin\StringFcLib.h"
#include "..\..\..\ProjectsLib\LibMarcin\DispLib.h"

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	std::string FileName, FolderName, FileNameBase, FileNameExtension;
	FolderName = "C:\\Data\\ExtensiveDirAvgTest\\InputData\\BarsA0-90F16T08B21\\"; // folder where to save outpute test images (use double slashes, even at the end)
	FileNameBase = "Bars"; // beginning of test file name
	FileNameExtension = ".tif"; // test file name extension

	bool saveResult = 1; // 1 --> files saved
	bool displayResult = 1; // 1 --> files only displayed
	// only one of the following three options should be chosen, or none (type of noise)
	bool addNoise = 0;
	bool gausBlur = 1; // blur kernel if going to have a Gaussian shape (opencv function below, parameter fixed for now)
	bool averageBlur = 0; // blur kernel if going to have constant value (1/number of pixels in the kernel)
	
	int barTickness = 8;
	int barFrequency = 16; // distance between corresponding bar location (i.e., barFrequency - barTickness = distance between bars)

	// other parameters
	int intensityBright = 65535.0/4.0*3.0;
	int intensityDark = 65535.0 / 4.0;

	int noiseMean = 0; // mean of the Gaussian noise function 
	int noiseSDTBase = 2000; // increasing step for Gaussian noise STD starting from noiseSTD below

	bool rotateImage = 1; // Image rotation: 1-->yes; 0-->no
	float rotationAngleStart = 0;
	float rotationAngleStop = 90;
	float rotationAngleStep = 1;

	bool cropImage = 1; // Crop image after rotation: 1-->yes; 0-->no

	int iterNr = 10;//10; // number of modifed images (blur or noise): 0 means not distortion applied
	int iterStart = 10;

	// the resulting image size if always half of these values, if cropping is on (see above)
	int maxX = 1024;
	int maxY = 1024;

	int offset = 0;

	//donot change
	int noiseSTD = 0; // Initial value for Gaussian noise STD
	int kernelSize = 0; // notv needed for now, defined below

// iteration from here
	for (float rotationAngle = rotationAngleStart; rotationAngle <=  rotationAngleStop; rotationAngle += rotationAngleStep)
		for (int k = iterStart; k <= iterNr; k++)
	{

		Mat Im;
		//unsigned char *wIm;

		Im = Mat::ones(maxX, maxY, CV_32F)*((float)intensityDark);
		//wIm = Im.data;

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

		//Rotation of image
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


		if (saveResult)
		{
			Mat ImSave;
			Im.convertTo(ImSave, CV_16U);
			FileName = FolderName + FileNameBase + "Thickness" + ItoStrLZ(barTickness, 2);
			FileName += "Spacing" + ItoStrLZ(barFrequency - barTickness, 2);
			if (rotateImage)
				FileName += "Angle" + ItoStrLZ(rotationAngle,3);
			else
				FileName += "Angle" + ItoStrLZ(0,3);
			
			if (addNoise)
				FileName += "NoiseSTD" + ItoStrLZ(noiseSTD, 5);

			if (gausBlur)
				FileName += "BlurKernel" + ItoStrLZ(kernelSize,2);

			if (averageBlur)
				FileName += "BlurKernel" + ItoStrLZ(kernelSize, 2);

			FileName += FileNameExtension;

			imwrite(FileName, ImSave);
		}
		// display result
		if (displayResult)
		{
			namedWindow("Image", WINDOW_AUTOSIZE);
		}
		if (displayResult)
		{
			Mat ImTemp;
			Im.convertTo(ImTemp, CV_32F);

			Mat ImShow = ShowImageF32PseudoColor(ImTemp, 0.0, 65535.0);
			//Mat ImShow = ShowImage16PseudoColor(Im, 0.0, 65535.0);
		//	int normCoef = 65535 / 256;
		//	ImTemp = Im / normCoef;

		//	ImTemp.convertTo(ImTemp, CV_8U);
		//	applyColorMap(ImTemp, ImShow, COLORMAP_JET);
			imshow("Image", ImShow);
			waitKey(100);

		}

	}
	return 0;
}


