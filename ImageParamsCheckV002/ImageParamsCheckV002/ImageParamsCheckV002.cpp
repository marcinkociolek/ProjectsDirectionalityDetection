// ImageParamsCheckV002.cpp : Defines the entry point for the console application.
//

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
//#include "opencv2\contrib\contrib.hpp"

#include <boost\filesystem.hpp>
#include <boost/regex.hpp>

//#include "math.h"
#include <iostream>
#include <fstream>
//#include <Windows.h>

#include "..\..\..\ProjectsLib\LibMarcin\Functions.h"
#include "..\..\..\ProjectsLib\LibMarcin\ParamFromXML.h"
#include "..\..\..\ProjectsLib\LibMarcin\DispLib.h"
#include "..\..\..\ProjectsLib\LibMarcin\NormalizationLib.h"
//#include "HaralickLib.h"
#include "..\..\..\ProjectsLib\LibMarcin\StringFcLib.h"

#include "..\..\..\ProjectsLib\tinyxml\tinyxml.h"
#include "..\..\..\ProjectsLib\tinyxml\tinystr.h"


#define PI 3.14159265

//using namespace cv;
using namespace cv;
using namespace std;
using namespace boost;
using namespace boost::filesystem;


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



	cout << "Input folder name" << "\t" << ProcOptions.InFolderName1 << "\n";
	
	cout << "Output folder name" << "\t" << ProcOptions.OutFolderName1 << "\n";
	
	path InPath;
	string OutString = ProcOptionsStr;
	OutString += "\n\n";
	
	// read  files
	//string OutStringLocal = "";
	OutString += "File Name\tMin Norm Global\tMin Norm Global\tmin\tmax\tmean\tstd\tperc 1\tperc99\tperc 0.1\tperc99.9\n";
	for (directory_entry& FileToProcess : directory_iterator(PathToProcess))
	{
		InPath = FileToProcess.path();

	

		if (!regex_match(InPath.filename().string().c_str(), FilePattern))
			continue;

		if (!exists(InPath))
		{
			cout << InPath.filename().string() << " File not exists" << "\n";
			continue;
		}



		Mat ImIn = imread(InPath.string(), CV_LOAD_IMAGE_ANYDEPTH);

		if (!ImIn.size)
		{
			cout << "this is not a valid image file";
			continue;
		}
		OutString += InPath.filename().string() + "\t";
		cout << "In file  - " << InPath.filename().string() << "\n";

		int maxX, maxY, maxXY;
		maxX = ImIn.cols;
		maxY = ImIn.rows;
		maxXY = maxX * maxY;

		// conversion to float
		Mat ImInF;						// ,ImInFTemp;
		ImIn.convertTo(ImInF, CV_32F);
	


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

		OutString +=  to_string(minNormGlobal) + "\t";
		OutString +=  to_string(maxNormGlobal) + "\t";

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
						rectangle(ImToShow, Point(x - roiMaxX / 2, y - roiMaxY / 2),
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


		imwrite(ProcOptions.OutFolderName1 + InPath.filename().stem().string() + ".bmp", ImToShow);

		float imMean, imStd;
		int* Hist;
		int imMin, imMax;
		Hist = MatFMeanAndStd(ImInF, &imMean, &imStd, &imMin, &imMax);

		OutString += to_string(imMin);
		OutString += "\t";

		OutString += to_string(imMax);
		OutString += "\t";

		OutString += to_string(imMean);
		OutString += "\t";

		OutString += to_string(imStd);
		OutString += "\t";

		int* HistBined;

		int perc1, perc99, perm1, perm999;
		HistBined = FistogramFeatures(Hist, 1, &perc1, &perc99, &perm1, &perm999);
		delete[] Hist;
		delete[] HistBined;

		OutString += to_string(perc1);
		OutString += "\t";

		OutString += to_string(perc99);
		OutString += "\t";

		OutString += to_string(perm1);
		OutString += "\t";

		OutString += to_string(perm999);

		OutString += "\n";

	}

	//handle files
	if (ProcOptions.textOut)
	{
		string TextFileName = ProcOptions.OutFolderName1 + InPath.filename().stem().string() + ".txt";
		std::ofstream out(TextFileName);
		out << OutString;
		out.close();
	}


	return 0;
}

