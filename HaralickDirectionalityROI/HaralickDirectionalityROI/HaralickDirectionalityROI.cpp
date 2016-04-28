// HaralickDirectionalityROI.cpp : Defines the entry point for the console application.
//

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\contrib\contrib.hpp"

#include <boost\filesystem.hpp>
#include <boost/regex.hpp>


//#include "math.h"
#include <iostream>
#include <fstream>
#//include <Windows.h>

#include "Functions.h"
//#include "RedundantWaveletLib.h"
#include "NormalizationLib.h"
#include "HaralickLib.h"
#include "RegionU16Lib.h"

#include "ParamFromXML.h"
#include "DispLib.h"
#include "StringFcLib.h"

#include <tinyxml.h>
#include <tinystr.h>

#define PI 3.14159265

using namespace cv;
using namespace std;
using namespace boost;
using namespace boost::filesystem;


//const int stepNr = 180;


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "\nTo few arguments.";
		return 0;
	}

	path ConfigFile(argv[1]);
	if (!exists(ConfigFile))
	{
		cout << ConfigFile.filename().string() << " not exists " << '\n';
		return 0;
	}
	ProcessOptions ProcOptions;
	ProcOptions.LoadParams(ConfigFile.string());
	string ProcOptionsStr = ProcOptions.ShowParams();
	cout << ProcOptionsStr;

	path PathToProcess(ProcOptions.InFolderName1);
	if (!exists(PathToProcess))
	{
		cout << PathToProcess << " not exists " << '\n';
		//return 0;
	}
	if (!is_directory(PathToProcess))
	{
		cout << PathToProcess << " This is not a directory path " << '\n';
		//return 0;
	}
	
	if (ProcOptions.displayResult)
	{
		namedWindow("Image", WINDOW_AUTOSIZE);
	}

	if (ProcOptions.displaySmallImage)
	{
		namedWindow("ImageSmall", WINDOW_AUTOSIZE);
	}

	regex FilePattern(ProcOptions.InFilePattern1);

	Mat RoiIm;

	path PathRoi(ProcOptions.InFolderName2);
	if (!exists(PathRoi))
	{
		cout << PathRoi << " not exists " << '\n';
		//return 0;
	}
	if (!is_directory(PathRoi))
	{
		cout << PathRoi << " This is not a directory path " << '\n';
		//return 0;
	}
	path RoiFilePath;
	for (directory_entry& FileToProcess : directory_iterator(PathRoi))
	{
		RoiFilePath = FileToProcess.path();
		regex RoiFilePattern(ProcOptions.InFilePattern2);
		if (!regex_match(RoiFilePath.filename().string().c_str(), RoiFilePattern))
			continue;

		if (!exists(RoiFilePath))
		{
			continue;
		}

		//OutString += "Roi file - " + RoiFilePath.filename().string() + "\n";
		RoiIm = imread(RoiFilePath.string(), CV_LOAD_IMAGE_ANYDEPTH);

		if (!RoiIm.size)
		{
			continue;
		}
		else
			break;
	}
	if (!RoiIm.cols || !RoiIm.cols)
	{
		cout << "No vailid Roi file";
		return 0;
	}
	else
		cout << "Roi file  - " << RoiFilePath.filename().string() << "\n";


	int *maxRoiX = new int[65536];
	int *minRoiX = new int[65536];
	int *maxRoiY = new int[65536];
	int *minRoiY = new int[65536];
	
	int maxRoiNr = 0;
	
	for (int i = 0; i < 65536; i++)
	{
		maxRoiX[i] = 0;
		minRoiX[i] = 65536;
		maxRoiY[i] = 0;
		minRoiY[i] = 65536;
	}

	if (RoiIm.depth() != CV_16U)
		RoiIm.convertTo(RoiIm, CV_16U);
	unsigned short *wRoiIm = (unsigned short *)RoiIm.data;
	for (int y = 0; y < RoiIm.rows; y++)
	{
		for (int x = 0; x < RoiIm.cols; x++)
		{
			if (*wRoiIm)
			{
				if (maxRoiX[*wRoiIm] < x)
					maxRoiX[*wRoiIm] = x;
				if (minRoiX[*wRoiIm] > x)
					minRoiX[*wRoiIm] = x;
				if (maxRoiY[*wRoiIm] < y)
					maxRoiY[*wRoiIm] = y;
				if (minRoiY[*wRoiIm] > y)
					minRoiY[*wRoiIm] = y;

				if (maxRoiNr < *wRoiIm)
					maxRoiNr = *wRoiIm;
			}
			wRoiIm++;
		}
	}

	int stepNr = (int)(180.0 / ProcOptions.angleStep);

	float *Energy = new float[stepNr];
	float *Contrast = new float[stepNr];
	float *Correlation = new float[stepNr];
	float *Homogenity = new float[stepNr];

	int *Angles = new int[stepNr]; // vector for best angles histogtam

	float featCount = 0;
	if (ProcOptions.useContrast)
		featCount++;
	if (ProcOptions.useEnergy)
		featCount++;
	if (ProcOptions.useHomogeneity)
		featCount++;
	if (ProcOptions.useCorrelation)
		featCount++;

	for (directory_entry& FileToProcess : directory_iterator(PathToProcess))
	{
		path InPath = FileToProcess.path();

		string OutString = ProcOptionsStr;

		if (!regex_match(InPath.filename().string().c_str(), FilePattern))
			continue;

		if (!exists(InPath))
		{
			cout << InPath.filename().string() << " File not exists" << "\n";
			continue;
		}

		OutString += "In file - " + InPath.filename().string() + "\n";
		cout << "In file  - " << InPath.filename().string() << "\n";

		Mat ImIn = imread(InPath.string(), CV_LOAD_IMAGE_ANYDEPTH);

		if (!ImIn.size)
		{
			cout << "this is not a valid image file";
			continue;
		}

		int maxX, maxY, maxXY;
		maxX = ImIn.cols;
		maxY = ImIn.rows;
		maxXY = maxX * maxY;

		// conversion to float
		Mat ImInF;						// ,ImInFTemp;
		ImIn.convertTo(ImInF, CV_32F);
		// Filtering

		//
		switch (ProcOptions.preprocessType)
		{
		case 1:
			blur(ImInF, ImInF, Size(3, 3));
			break;
		case 2:
			medianBlur(ImInF, ImInF, 3);
			break;
		default:
			break;
		}


		float maxNormGlobal = ProcOptions.maxNormGlobal;
		float minNormGlobal = ProcOptions.minNormGlobal;

		switch (ProcOptions.normalisation)
		{
		case 1:
			NormParamsMinMax(ImInF, &maxNormGlobal, &minNormGlobal);
			break;
		case 2:
			NormParamsMinMax(ImInF, &maxNormGlobal, &minNormGlobal);
			break;
		case 3:
			NormParamsMeanP3Std(ImInF, &maxNormGlobal, &minNormGlobal);
			break;
		case 4:
			NormParamsMeanP3Std(ImInF, &maxNormGlobal, &minNormGlobal);
			break;
		case 5:
			NormParams1to99perc(ImInF, &maxNormGlobal, &minNormGlobal);
			break;
		case 6:
			NormParams1to99perc(ImInF, &maxNormGlobal, &minNormGlobal);
			break;
		default:
			break;
		}

		OutString += "Min Norm Global: \t" + to_string(minNormGlobal) + "'n";
		OutString += "Max Norm Global: \t" + to_string(maxNormGlobal) + "'n";

		float displayMax = ProcOptions.displayMax;
		float displayMin = ProcOptions.displayMin;

		if (!ProcOptions.useFixtDispNorm)
		{
			displayMax = maxNormGlobal;
			displayMin = minNormGlobal;
		}

		Mat ImToShow;

		ImToShow = ShowImageF32PseudoColor(ImInF, displayMin, displayMax);
		/*
		if (ProcOptions.showTiles)
		{
			for (int i = 1; i <= maxRoiNr; i++)
			{
				rectangle(ImToShow, Point(minRoiX[i], minRoiY[i]), Point(maxRoiX[i], maxRoiY[i]), Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
			}
		}
		*/
		Mat Contour = GetContour5(RoiIm);
		RegionDilation5(Contour);
		ImToShow = ShowSolidRegionOnImageInBlack(Contour, ImToShow);
		if (ProcOptions.displayResult)
		{
			imshow("Image", ImToShow);
			waitKey(50);
		}

		

		OutString += "Tile Y\tTile X\tAngle\tBest Angle Count\tMean Intensity\tTile min norm\tTile max norm\n";
		
		for (unsigned short roiNr = 1; roiNr <= maxRoiNr; roiNr++)
		{
			Mat SmallIm;
			Mat Roi;
			ImInF(Rect(minRoiX[roiNr], minRoiY[roiNr], maxRoiX[roiNr] - minRoiX[roiNr], maxRoiY[roiNr] - minRoiY[roiNr])).copyTo(SmallIm);
			RoiIm(Rect(minRoiX[roiNr], minRoiY[roiNr], maxRoiX[roiNr] - minRoiX[roiNr], maxRoiY[roiNr] - minRoiY[roiNr])).copyTo(Roi);
			float meanSmallIm = -10.0;
			//if (ProcOptions.useMinMean)
			meanSmallIm = MatFMeanRoi(SmallIm, Roi, roiNr);

			float maxNorm, minNorm;

			switch (ProcOptions.normalisation)
			{
			case 1:
				NormParamsMinMax(SmallIm, Roi, roiNr, &maxNorm, &minNorm);
				break;
			case 2:
				maxNorm = maxNormGlobal;
				minNorm = minNormGlobal;
				break;
			case 3:
				NormParamsMeanP3Std(SmallIm, Roi, roiNr, &maxNorm, &minNorm);
				break;
			case 4:
				maxNorm = maxNormGlobal;
				minNorm = minNormGlobal;
				break;
			case 5:
				NormParams1to99perc(SmallIm, Roi, roiNr, &maxNorm, &minNorm);
				break;
			case 6:
				maxNorm = maxNormGlobal;
				minNorm = minNormGlobal;
				break;
			default:
				maxNorm = 65535.0;
				minNorm = 0.0;
				break;
			}

			for (int i = 0; i < stepNr; i++)
			{
				Angles[i] = 0;
			}

			// ofset loop
			for (int offset = ProcOptions.minOfset; offset <= ProcOptions.maxOfset; offset += 1)
			{
				for (int angleIndex = 0; angleIndex < stepNr; angleIndex++)
				{
					float angle = ProcOptions.angleStep * angleIndex;
					Mat COM;

					COM = COMCardoneRoi(SmallIm, Roi, offset, angle, ProcOptions.binCount, maxNorm, minNorm, ProcOptions.interpolation, roiNr);
					COMParams(COM, &Contrast[angleIndex], &Energy[angleIndex], &Homogenity[angleIndex], &Correlation[angleIndex]);
				}
				// best angle for ofset
				int bestAngleContrast, bestAngleEnergy, bestAngleHomogenity, bestAngleCorrelation;
				if (ProcOptions.useContrast)
					bestAngleContrast = FindBestAngleMin(Contrast, stepNr);
				if (ProcOptions.useEnergy)
					bestAngleEnergy = FindBestAngleMax(Energy, stepNr);
				if (ProcOptions.useHomogeneity)
					bestAngleHomogenity = FindBestAngleMax(Homogenity, stepNr);
				if (ProcOptions.useCorrelation)
					bestAngleCorrelation = FindBestAngleMax(Correlation, stepNr);

				if (ProcOptions.useContrast)
					Angles[bestAngleContrast]++;
				if (ProcOptions.useEnergy)
					Angles[bestAngleEnergy]++;
				if (ProcOptions.useHomogeneity)
					Angles[bestAngleHomogenity]++;
				if (ProcOptions.useCorrelation)
					Angles[bestAngleCorrelation]++;
			}
			int bestAngle = 0;
			int maxAngle = Angles[0];
			for (int i = 1; i < stepNr; i++)
			{
				if (maxAngle < Angles[i])
				{
					maxAngle = Angles[i];
					bestAngle = i;
				}
			}

			// show line on image
			//int regCenterX, regCenterY;
			int barCenterX = (maxRoiX[roiNr] - minRoiX[roiNr]) / 2 + minRoiX[roiNr];
			int barCenterY = (maxRoiY[roiNr] - minRoiY[roiNr]) / 2 + minRoiY[roiNr];
			CenterOfMas(RoiIm, &barCenterX, &barCenterY, roiNr);

			double lineLength;
			if (ProcOptions.lineLengthPropToConfidence)
				lineLength = (double)(ProcOptions.lineHalfLength) / (ProcOptions.maxOfset - ProcOptions.minOfset + 1) / featCount * maxAngle;
			else
				lineLength = (double)(ProcOptions.lineHalfLength);
			int lineOffsetX = (int)round(lineLength *  sin((double)(bestAngle)*ProcOptions.angleStep* PI / 180.0));
			int lineOffsetY = (int)round(lineLength * cos((double)(bestAngle)*ProcOptions.angleStep* PI / 180.0));

			//
			bool meanCondition = 0;
			if (ProcOptions.useMinMean)
			{
				if (meanSmallIm >= ProcOptions.minMean)
					meanCondition = true;
				else
					meanCondition = false;
			}
			else
				meanCondition = true;

			if ((maxAngle >= ProcOptions.minHit) && meanCondition)
			{
				line(ImToShow, Point(barCenterX - lineOffsetX, barCenterY - lineOffsetY), Point(barCenterX + lineOffsetX, barCenterY + lineOffsetY), Scalar(0, 0.0, 0.0, 0.0), ProcOptions.imposedLineThickness);
			}
			imshow("Image", ImToShow);

			cout << roiNr << "\t"  "\t" << "A = " << to_string(bestAngle*ProcOptions.angleStep);
			cout << "  c = " << ItoStrLS(maxAngle, 2) << " mean =  " << to_string(meanSmallIm);
			cout << " min norm = " << to_string(minNorm) << " max norm = " << to_string(maxNorm) << "\n";
			OutString += ItoStrLS(roiNr, 2) + "\t";
			if ((maxAngle >= ProcOptions.minHit) && meanCondition)
			{
				OutString += to_string((float)bestAngle * ProcOptions.angleStep) + "\t";
				OutString += to_string((float)(maxAngle) / (float)(ProcOptions.maxOfset - ProcOptions.minOfset + 1) / (float)featCount) + "\t";
			}
			else
			{
				OutString += "NAN\t";
				OutString += "NAN\t";
			}
			OutString += to_string(meanSmallIm) + "\t" + to_string(minNorm) + "\t" + to_string(maxNorm) + "\n";

			if (ProcOptions.displaySmallImage)
			{
				Mat SmallImToShow = ShowImageF32PseudoColor(SmallIm, minNorm, maxNorm);
				
				Mat Contour = GetContour5(Roi);
				RegionDilation5(Contour);
				SmallImToShow = ShowSolidRegionOnImage(Contour, SmallImToShow);
	
				line(SmallImToShow, Point(SmallImToShow.cols / 2 - lineOffsetX, SmallImToShow.rows / 2 - lineOffsetY), Point(SmallImToShow.cols / 2 + lineOffsetX, SmallImToShow.rows / 2 + lineOffsetY), Scalar(0, 0.0, 0.0, 0.0), ProcOptions.imposedLineThickness);
				destroyWindow("ImageSmall");
				imshow("ImageSmall", SmallImToShow);
			}
			if (ProcOptions.goThru)
				waitKey(50);
			else
				waitKey(0);

		}
			
		imwrite(ProcOptions.OutFolderName1 + InPath.filename().stem().string() + ".bmp", ImToShow);

		if (ProcOptions.textOut)
		{
			string TextFileName = ProcOptions.OutFolderName1 + InPath.filename().stem().string() + ".txt";
			std::ofstream out(TextFileName);
			out << OutString;
			out.close();
		}

	}
	//end
	string TempStr;
	cin >> TempStr;
	return 0;
}

