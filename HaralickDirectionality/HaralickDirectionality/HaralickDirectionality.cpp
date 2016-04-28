// HaralickDirectionality.cpp : Defines the entry point for the console application.
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
#include "ParamFromXML.h"
#include "DispLib.h"
#include "StringFcLib.h"
#include "RegionU16Lib.h"

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
		return 0;
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

	Mat Roi;
	int roiMaxX, roiMaxY;
	switch (ProcOptions.tileShape)
	{
	case 1:
		roiMaxX = ProcOptions.maxTileX;
		roiMaxY = ProcOptions.maxTileY;
		Roi = Mat::ones(roiMaxY, roiMaxX, CV_16U);
		break;
	case 2:
		roiMaxX = ProcOptions.maxTileX;
		roiMaxY = ProcOptions.maxTileY;
		Roi = Mat::zeros(roiMaxY, roiMaxX, CV_16U);
		ellipse(Roi, Point(roiMaxX / 2, roiMaxY / 2),
					Size(roiMaxX / 2, roiMaxY / 2), 0.0, 0.0, 360.0,
					1, -1);
		break;
	case 3:
		{
			  int edgeLength = ProcOptions.maxTileX;
			  roiMaxX = edgeLength * 2;
			  roiMaxY = (int)((float)edgeLength * 0.8660254 * 2.0);
			  Roi = Mat::zeros(roiMaxY, roiMaxX, CV_16U);

			  Point vertice0(edgeLength / 2, 0);
			  Point vertice1(edgeLength / 2 + edgeLength - 1, 0);
			  Point vertice2(roiMaxX - 1, roiMaxY / 2);
			  Point vertice3(edgeLength / 2 + edgeLength - 1, roiMaxY - 1);
			  Point vertice4(edgeLength / 2, roiMaxY - 1);
			  Point vertice5(0, roiMaxY / 2);

			  line(Roi, vertice0, vertice1, 1, 1);
			  line(Roi, vertice1, vertice2, 1, 1);
			  line(Roi, vertice2, vertice3, 1, 1);
			  line(Roi, vertice3, vertice4, 1, 1);
			  line(Roi, vertice4, vertice5, 1, 1);
			  line(Roi, vertice5, vertice0, 1, 1);
			  unsigned short *wRoi;
			  
			  for (int y = 1; y < roiMaxY - 1; y++)
			  {
				  wRoi = (unsigned short *)Roi.data + roiMaxX * y;
				  int x = 0;
				  for (x; x < roiMaxX; x++)
				  {
					  if (*wRoi)
						  break;
					  wRoi++;
				  }
				  x++;
				  wRoi++;
				  for (x; x < roiMaxX; x++)
				  {
					  if (*wRoi)
						  break;
					  *wRoi = 1;
					  wRoi++;
				  }
			  }
			  
		}
		break;
	default:
		break;
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

		OutString += "Min Norm Global: \t" + to_string(minNormGlobal) + "\n";
		OutString += "Max Norm Global: \t" + to_string(maxNormGlobal) + "\n";

		float displayMax = ProcOptions.displayMax;
		float displayMin = ProcOptions.displayMin;
		
		if (!ProcOptions.useFixtDispNorm)
		{
			displayMax = maxNormGlobal;
			displayMin = minNormGlobal;
		}

		Mat ImToShow;

		ImToShow = ShowImageF32PseudoColor(ImInF, displayMin, displayMax);

		if (ProcOptions.showTiles)
		{
			switch (ProcOptions.tileShape)
			{
			case 1:
				for (int y = ProcOptions.offsetTileY; y <= (maxY - ProcOptions.offsetTileY); y += ProcOptions.shiftTileY)
				{
					for (int x = ProcOptions.offsetTileX; x <= (maxX - ProcOptions.offsetTileX); x += ProcOptions.shiftTileX)
					{
						rectangle(ImToShow, Point(x - roiMaxX / 2, y -roiMaxY / 2),
							Point(x - roiMaxX / 2 + roiMaxX - 1, y - roiMaxY / 2 + roiMaxY - 1),
							Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
					}
				}
				break;
			case 2:
				for (int y = ProcOptions.offsetTileY; y <= (maxY - ProcOptions.offsetTileY); y += ProcOptions.shiftTileY)
				{
					for (int x = ProcOptions.offsetTileX; x <= (maxX - ProcOptions.offsetTileX); x += ProcOptions.shiftTileX)
					{
						ellipse(ImToShow, Point(x, y),
							Size(roiMaxX / 2, roiMaxY / 2), 0.0, 0.0, 360.0,
							Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
					}
				}
				break;
			case 3:
				for (int y = ProcOptions.offsetTileY; y <= (maxY - ProcOptions.offsetTileY); y += ProcOptions.shiftTileY)
				{
					for (int x = ProcOptions.offsetTileX; x <= (maxX - ProcOptions.offsetTileX); x += ProcOptions.shiftTileX)
					{
						int edgeLength = ProcOptions.maxTileX;
						Point vertice0(x - edgeLength / 2, y - (int)((float)edgeLength * 0.8660254));
						Point vertice1(x + edgeLength - edgeLength / 2, y - (int)((float)edgeLength * 0.8660254));
						Point vertice2(x + edgeLength, y);
						Point vertice3(x + edgeLength - edgeLength / 2, y + (int)((float)edgeLength * 0.8660254));
						Point vertice4(x - edgeLength / 2, y + (int)((float)edgeLength * 0.8660254));
						Point vertice5(x - edgeLength, y);

						line(ImToShow, vertice0, vertice1, Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
						line(ImToShow, vertice1, vertice2, Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
						line(ImToShow, vertice2, vertice3, Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
						line(ImToShow, vertice3, vertice4, Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
						line(ImToShow, vertice4, vertice5, Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
						line(ImToShow, vertice5, vertice0, Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);

					}
				}
				break;
			default:
				break;
			}
		}

		if (ProcOptions.displayResult)
		{
			imshow("Image", ImToShow);
			waitKey(50);
		}

		Mat SmallIm;
		int xTileNr = 0;
		int yTileNr = 0;

		OutString += "Tile Y\tTile X\tAngle\tBest Angle Count\tMean Intensity\tTile min norm\tTile max norm\n";
		for (int y = ProcOptions.offsetTileY; y <= (maxY - ProcOptions.offsetTileY); y += ProcOptions.shiftTileY)
		{
			for (int x = ProcOptions.offsetTileX; x <= (maxX - ProcOptions.offsetTileX); x += ProcOptions.shiftTileX)
			{
				ImInF(Rect(x - roiMaxX/2, y - roiMaxY / 2, roiMaxX, roiMaxY)).copyTo(SmallIm);
				float meanSmallIm = -10.0;
				//if (ProcOptions.useMinMean)
				meanSmallIm = MatFMeanRoi( SmallIm, Roi,1);

				float maxNorm, minNorm;
				
				switch (ProcOptions.normalisation)
				{
				case 1:
					NormParamsMinMax(SmallIm, Roi, 1, &maxNorm, &minNorm);
					break;
				case 2:
					maxNorm = maxNormGlobal;
					minNorm = minNormGlobal;
					break;
				case 3:
					NormParamsMeanP3Std(SmallIm, Roi, 1, &maxNorm, &minNorm);
					break;
				case 4:
					maxNorm = maxNormGlobal;
					minNorm = minNormGlobal;
					break;
				case 5:
					NormParams1to99perc(SmallIm, Roi, 1, &maxNorm, &minNorm);
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
						if (ProcOptions.tileShape < 2)
							COM = COMCardone4(SmallIm, offset, angle, ProcOptions.binCount, maxNorm, minNorm, ProcOptions.interpolation);
						else
							COM = COMCardoneRoi(SmallIm,Roi, offset, angle, ProcOptions.binCount, maxNorm, minNorm, ProcOptions.interpolation, 1);
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
				//int barCenterX = ProcOptions.maxTileX / 2 + x;
				//int barCenterY = ProcOptions.maxTileY / 2 + y;
				
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
					//line(ImToShow, Point(barCenterX - lineOffsetX, barCenterY - lineOffsetY), Point(barCenterX + lineOffsetX, barCenterY + lineOffsetY), Scalar(0, 0.0, 0.0, 0.0), ProcOptions.imposedLineThickness);
					line(ImToShow, Point(x - lineOffsetX, y - lineOffsetY), Point(x + lineOffsetX, y + lineOffsetY), Scalar(0, 0.0, 0.0, 0.0), ProcOptions.imposedLineThickness);
				}
				imshow("Image", ImToShow);

				cout << yTileNr << "\t" << xTileNr << "\t" << "A = " << to_string(bestAngle*ProcOptions.angleStep);
				cout << "  c = " << ItoStrLS(maxAngle, 2) << " mean =  " << to_string(meanSmallIm);
				cout << " min norm = " << to_string(minNorm) << " max norm = " << to_string(maxNorm) << "\n";
				OutString += ItoStrLS(yTileNr, 2) + "\t" + ItoStrLS(yTileNr, 2) + "\t";
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
				
					line(SmallImToShow, Point(SmallImToShow.cols / 2 - lineOffsetX, SmallImToShow.rows / 2 - lineOffsetY), Point(SmallImToShow.cols / 2 + lineOffsetX, SmallImToShow.rows / 2 + lineOffsetY), Scalar(0, 0.0, 0.0, 0.0), ProcOptions.imposedLineThickness);
					imshow("ImageSmall", ShowSolidRegionOnImage(GetContour5(Roi), SmallImToShow));
				}
				if (ProcOptions.goThru)
					waitKey(50);
				else
					waitKey(0);
			
				xTileNr++;
			}
			yTileNr++;
			xTileNr = 0;
		}
		imwrite(ProcOptions.OutFolderName1 + InPath.filename().stem().string() + ".bmp", ImToShow);

		if (ProcOptions.textOut)
		{
			string TextFileName = ProcOptions.OutFolderName1 + InPath.filename().stem().string() +".txt";
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

