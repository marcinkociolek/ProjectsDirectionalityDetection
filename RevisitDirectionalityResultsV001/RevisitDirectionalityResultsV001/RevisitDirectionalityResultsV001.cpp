// RevisitDirectionalityResultsV001.cpp : Defines the entry point for the console application.
//

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\contrib\contrib.hpp"
#include "math.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include "StringFcLib.h"

#include <boost\filesystem.hpp>

#include "Functions.h"
#include "RedundantWaveletLib.h"

#include <ParamFromXML.h>

using namespace cv;
using namespace std;
using namespace boost::filesystem;

#define PI 3.14159265


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
	cout << ProcOptions.ShowParams();

	path PathToProcess(ProcOptions.InFolderName1);
	if (!exists(PathToProcess))
	{
		cout << PathToProcess << " not exists " << '\n';
		return 0;
	}
	if (!is_directory(PathToProcess))
	{
		cout << PathToProcess << " This is not a directory path " << '\n';
		return 0;
	}
	
	string CommonString = "";
	int fileCounter = 1;
	double sumDirDiff = 0;

	int countDirDiff = 0;
	int countAll = 0;
	int countActinOveral = 0;
	int countCalceinOveral = 0;
	int countActinHiCalcein = 0;
	int countActinNoCalcein = 0;
	int countCalceinHiActin = 0;
	int countCalceinNoActin = 0;



	int HistogramDirDiff[91];
	int HistogramDirActinOveral[181];
	int HistogramDirCalceinOveral[181];
	int HistogramDirActinHiCalcein[181];
	int HistogramDirActinNoCalcein[181];
	int HistogramDirCalceinHiActin[181];
	int HistogramDirCalceinNoActin[181];


	for (int i = 0; i <= 90; i++)
		HistogramDirDiff[i] = 0;

	for (int i = 0; i <= 180; i++)
	{
		HistogramDirActinOveral[i] = 0;
		HistogramDirCalceinOveral[i] = 0;
		HistogramDirActinHiCalcein[i] = 0;
		HistogramDirActinNoCalcein[i] = 0;
		HistogramDirCalceinHiActin[i] = 0;
		HistogramDirCalceinNoActin[i] = 0;
	}

	for (directory_entry& FileToProcess : directory_iterator(PathToProcess))
	{
		if (FileToProcess.path().extension() != ProcOptions.InFileExtension1)
			continue;
	
		path Path1 = FileToProcess.path();
		if (!exists(Path1))
		{
			cout << Path1.filename().string() << " File not exists" << "\n";
			break;
		}
		//string LocalFullFileName1, LocalFileName1, LocalFullFileName2, LocalFileName2;

		//LocalFullFileName1 = FileToProcess.path().string();
		//LocalFileName1 = FileToProcess.path().filename().string();
	
		int patternPosition = Path1.stem().string().find(ProcOptions.InFilePattern1);
		if (patternPosition<0)
			continue;
		string FileNameCore = Path1.stem().string().substr(0,patternPosition);


		path Path2(ProcOptions.InFolderName2 + FileNameCore + ProcOptions.InFilePattern2 + ProcOptions.InFileExtension2);
		if (!exists(Path2))
		{
			cout << Path2.filename().string() << " File not exists" << "\n";
			break;
		}

		path Path3(ProcOptions.InFolderName3 + FileNameCore + ProcOptions.InFilePattern3 + ProcOptions.InFileExtension3);
		if (!exists(Path3))
		{	 
			cout << Path3.filename().string() << " File not exists" << "\n";
			break;
		}

		path Path4(ProcOptions.InFolderName4 + FileNameCore + ProcOptions.InFilePattern4 + ProcOptions.InFileExtension4);
		if (!exists(Path4))
		{	  
			cout << Path4.filename().string() << " File not exists" << "\n";
			break;
		}

		path Path5(ProcOptions.InFolderName5 + FileNameCore + ProcOptions.InFilePattern5 + ProcOptions.InFileExtension5);
		if (!exists(Path5))
		{
			cout << Path5.filename().string() << " File not exists" << "\n";
			break;
		}

		path Path6(ProcOptions.InFolderName6 + FileNameCore + ProcOptions.InFilePattern6 + ProcOptions.InFileExtension6);
		if (!exists(Path6))
		{
			cout << Path6.filename().string() << " File not exists" << "\n";
			break;
		}

		string Line1,Line2, StringOut;

		std::ifstream inFile1(Path1.string());//FileToProcess.path().filename().string());
		
		if (!inFile1.is_open())
		{
			cout << Path1.stem() << "could not be openned " << "\n";
			break;
		}

		std::ifstream inFile2(Path2.string());//FileToProcess.path().filename().string());

		if (!inFile2.is_open())
		{
			cout << Path2.stem() << "could not be openned " << "\n";
			break;
		}

		Mat ImIn1 = imread(Path3.string(), CV_LOAD_IMAGE_ANYDEPTH);
		if (!ImIn1.size)
		{
			cout << Path3.stem() << " Image could not be openned " << "\n";
			break;
		}

		Mat ImIn2 = imread(Path4.string(), CV_LOAD_IMAGE_ANYDEPTH);
		if (!ImIn2.size)
		{
			cout << Path4.stem() << " Image could not be openned " << "\n";
			break;
		}

		Mat ImIn3 = imread(Path5.string(), CV_LOAD_IMAGE_ANYDEPTH);
		if (!ImIn3.size)
		{
			cout << Path5.stem() << " Image could not be openned " << "\n";
			break;
		}

		Mat ImIn4 = imread(Path6.string(), CV_LOAD_IMAGE_ANYDEPTH);
		if (!ImIn4.size)
		{
			cout << Path6.stem() << " Image could not be openned " << "\n";
			break;
		}

		int maxX, maxY, maxXY;
		maxX = ImIn1.cols;
		maxY = ImIn1.rows;
		maxXY = maxX * maxY;

		Mat ImIn1F, ImIn2F, ImIn3F, ImIn4F, ImInFTemp;
		ImIn1.convertTo(ImIn1F, CV_32F);
		ImIn2.convertTo(ImIn2F, CV_32F);
		ImIn3.convertTo(ImIn3F, CV_32F);
		ImIn4.convertTo(ImIn4F, CV_32F);



		Mat Im1Temp, Im1ShowGlobal, ImScale;
		Mat Im2Temp, Im2ShowGlobal;
		Mat Im3Temp, Im3ShowGlobal;
		Mat Im4Temp, Im4ShowGlobal;

		float gainCoef1 = 255.0 / (ProcOptions.displayMax - ProcOptions.displayMin);
		float offsetCoef1 = gainCoef1 * ProcOptions.displayMin;
		ImInFTemp = ImIn1F * gainCoef1 - offsetCoef1;
		ImInFTemp.convertTo(Im1Temp, CV_8U);
		applyColorMap(Im1Temp, Im1ShowGlobal, COLORMAP_JET);

		float gainCoef2 = 255.0 / (ProcOptions.displayMax2 - ProcOptions.displayMin2);
		float offsetCoef2 = gainCoef2 * ProcOptions.displayMin2;
		ImInFTemp = ImIn2F * gainCoef2 - offsetCoef2;
		ImInFTemp.convertTo(Im2Temp, CV_8U);
		applyColorMap(Im2Temp, Im2ShowGlobal, COLORMAP_JET);

		float gainCoef3 = 255.0 / (ProcOptions.displayMax3 - ProcOptions.displayMin3);
		float offsetCoef3 = gainCoef3 * ProcOptions.displayMin3;
		ImInFTemp = ImIn3F * gainCoef3 - offsetCoef3;
		ImInFTemp.convertTo(Im3Temp, CV_8U);
		applyColorMap(Im3Temp, Im3ShowGlobal, COLORMAP_JET);

		float gainCoef4 = 255.0 / (ProcOptions.displayMax4 - ProcOptions.displayMin4);
		float offsetCoef4 = gainCoef4 * ProcOptions.displayMin4;
		ImInFTemp = ImIn4F * gainCoef4 - offsetCoef4;
		ImInFTemp.convertTo(Im4Temp, CV_8U);
		applyColorMap(Im4Temp, Im4ShowGlobal, COLORMAP_JET);

		if (ProcOptions.showTiles)
		{
			for (int x = ProcOptions.maxTileX; x < maxX; x += ProcOptions.maxTileX)
			{
				line(Im1ShowGlobal, Point(x, 0), Point(x, maxY - 1), Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
			}
			for (int y = ProcOptions.maxTileY; y < maxY; y += ProcOptions.maxTileY)
			{
				line(Im1ShowGlobal, Point(0, y), Point(maxX - 1, y), Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
			}

			for (int x = ProcOptions.maxTileX; x < maxX; x += ProcOptions.maxTileX)
			{
				line(Im2ShowGlobal, Point(x, 0), Point(x, maxY - 1), Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
			}
			for (int y = ProcOptions.maxTileY; y < maxY; y += ProcOptions.maxTileY)
			{
				line(Im2ShowGlobal, Point(0, y), Point(maxX - 1, y), Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
			}

			for (int x = ProcOptions.maxTileX; x < maxX; x += ProcOptions.maxTileX)
			{
				line(Im3ShowGlobal, Point(x, 0), Point(x, maxY - 1), Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
			}
			for (int y = ProcOptions.maxTileY; y < maxY; y += ProcOptions.maxTileY)
			{
				line(Im3ShowGlobal, Point(0, y), Point(maxX - 1, y), Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
			}

			for (int x = ProcOptions.maxTileX; x < maxX; x += ProcOptions.maxTileX)
			{
				line(Im4ShowGlobal, Point(x, 0), Point(x, maxY - 1), Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
			}
			for (int y = ProcOptions.maxTileY; y < maxY; y += ProcOptions.maxTileY)
			{
				line(Im4ShowGlobal, Point(0, y), Point(maxX - 1, y), Scalar(0.0, 0.0, 0.0, 0.0), ProcOptions.tileLineThickness);
			}

		}

		StringOut = "";

		while ((inFile1.good())&(inFile2.good()))
		{
			getline(inFile1, Line1);
			getline(inFile2, Line2);
			if (Line1 == "yRoiNr\txRoiNr\tdirection\tDDC\tmean Tile\tstd Tile\tmean tile 2\tstd Tile 2")
				break;
			StringOut += Line1;
			StringOut += "\t\t\t\t\t\t\t\t";
			StringOut += Line2;
			StringOut += "\n";
		}
		StringOut += Line1;
		StringOut += "\t\t";
		StringOut += Line2;
		StringOut += "\n";

		int x1, y1;
		float  dir1, dDC1, mean1, std1;
		int x2, y2;
		float dir2, dDC2, mean2, std2;
		bool dir1IsNAN, dir2IsNAN;
		while ((inFile1.good())&(inFile2.good()))
		{
			//getline(inFile1, Line1);
			getline(inFile1, Line1, '\t');
			y1 = atoi(Line1.c_str());
			StringOut += Line1;
			StringOut += "\t";

			getline(inFile1, Line1, '\t');
			x1 = atoi(Line1.c_str());
			StringOut += Line1;
			StringOut += "\t";

			getline(inFile1, Line1, '\t');
			dir1 = atof(Line1.c_str());
			StringOut += Line1;
			StringOut += "\t";

			if (Line1 == "NAN")
				dir1IsNAN = 1;
			else
				dir1IsNAN = 0;

			getline(inFile1, Line1, '\t');
			dDC1 = atof(Line1.c_str());
			StringOut += Line1;
			StringOut += "\t";

			getline(inFile1, Line1, '\t');
			mean1 = atof(Line1.c_str());
			StringOut += Line1;
			StringOut += "\t";

			getline(inFile1, Line1, '\n');
			std1 = atof(Line1.c_str());
			StringOut += Line1;
			StringOut += "\t";

			
			StringOut += "\t\t\t";

			//getline(inFile2, Line2);
			//StringOut += Line1;
			
			//StringOut += Line2;
			getline(inFile2, Line2, '\t');
			y2 = atoi(Line2.c_str());
			StringOut += Line2;
			StringOut += "\t";

			getline(inFile2, Line2, '\t');
			x2 = atoi(Line2.c_str());
			StringOut += Line2;
			StringOut += "\t";

			getline(inFile2, Line2, '\t');
			dir2 = atof(Line2.c_str());
			StringOut += Line2;
			StringOut += "\t";

			if (Line2 == "NAN")
				dir2IsNAN = 1;
			else
				dir2IsNAN = 0;

			getline(inFile2, Line2, '\t');
			dDC2 = atof(Line2.c_str());
			StringOut += Line2;
			StringOut += "\t";

			getline(inFile2, Line2, '\t');
			mean2 = atof(Line2.c_str());
			StringOut += Line2;
			StringOut += "\t";

			getline(inFile2, Line2, '\n');
			std2 = atof(Line2.c_str());
			StringOut += Line2;
			StringOut += "\t";
			
			StringOut += "\t";
			// colocalization
			if (!dir1IsNAN & !dir2IsNAN & (mean1 >= ProcOptions.treshold1) & (mean2 >= ProcOptions.treshold2))
			{
				float dirDiff;
				dirDiff = abs(dir1 - dir2);
				if (dirDiff > 90.0)
					dirDiff = 180 - abs(dir1 - dir2);

				//dirDiff = 0;

				StringOut += to_string(dirDiff);
				sumDirDiff += (double)(dirDiff);
				countDirDiff++;
				HistogramDirDiff[int(dirDiff)]++;






			}
			StringOut += "\t";

			//Actin directions if enough signal
			if (!dir1IsNAN & (mean1 >= ProcOptions.treshold1))
			{
				StringOut += to_string(dir1);

				countActinOveral++;
//				int histPos = (int(dir1) + 2) / 5;
//				if (histPos > 35)
//					histPos = 0;
//				HistogramDirActinOveral[histPos]++;
				HistogramDirActinOveral[int(dir1)]++;

				// show 1 line on image
				int barCenterX = ProcOptions.maxTileX / 2 + (x1 * ProcOptions.maxTileX);
				int barCenterY = ProcOptions.maxTileY / 2 + (y1 * ProcOptions.maxTileY);
				double lineLength = (double)(ProcOptions.lineHalfLength);
				int lineOffsetX = (int)round(lineLength * sin((double)(dir1)* PI / 180.0));
				int lineOffsetY = (int)round(lineLength * cos((double)(dir1)* PI / 180.0));

				line(Im1ShowGlobal, Point(barCenterX - lineOffsetX, barCenterY - lineOffsetY), Point(barCenterX + lineOffsetX, barCenterY + lineOffsetY), Scalar(0, 0.0, 0.0, 0.0), 3);
				line(Im2ShowGlobal, Point(barCenterX - lineOffsetX, barCenterY - lineOffsetY), Point(barCenterX + lineOffsetX, barCenterY + lineOffsetY), Scalar(0, 0.0, 0.0, 0.0), 3);
				line(Im3ShowGlobal, Point(barCenterX - lineOffsetX, barCenterY - lineOffsetY), Point(barCenterX + lineOffsetX, barCenterY + lineOffsetY), Scalar(0, 0.0, 0.0, 0.0), 3);
				line(Im4ShowGlobal, Point(barCenterX - lineOffsetX, barCenterY - lineOffsetY), Point(barCenterX + lineOffsetX, barCenterY + lineOffsetY), Scalar(0, 0.0, 0.0, 0.0), 3);

			}
			StringOut += "\t";

			//Calcein directions if enough signal
			if (!dir2IsNAN & (mean2 >= ProcOptions.treshold2))
			{
				StringOut += to_string(dir2);

				countCalceinOveral++;
				HistogramDirCalceinOveral[int(dir2)]++;

				// show 1 line on image
				int barCenterX = ProcOptions.maxTileX / 2 + (x1 * ProcOptions.maxTileX) + 3;
				int barCenterY = ProcOptions.maxTileY / 2 + (y1 * ProcOptions.maxTileY) + 3;
				double lineLength = (double)(ProcOptions.lineHalfLength);
				int lineOffsetX = (int)round(lineLength * sin((double)(dir1)* PI / 180.0));
				int lineOffsetY = (int)round(lineLength * cos((double)(dir1)* PI / 180.0));

				lineOffsetX = (int)round(lineLength * sin((double)(dir2)* PI / 180.0));
				lineOffsetY = (int)round(lineLength * cos((double)(dir2)* PI / 180.0));
				line(Im1ShowGlobal, Point(barCenterX - lineOffsetX, barCenterY - lineOffsetY), Point(barCenterX + lineOffsetX, barCenterY + lineOffsetY), Scalar(100.0, 100.0, 100.0, 100.0), 3);
				line(Im2ShowGlobal, Point(barCenterX - lineOffsetX, barCenterY - lineOffsetY), Point(barCenterX + lineOffsetX, barCenterY + lineOffsetY), Scalar(100.0, 100.0, 100.0, 100.0), 3);
				line(Im3ShowGlobal, Point(barCenterX - lineOffsetX, barCenterY - lineOffsetY), Point(barCenterX + lineOffsetX, barCenterY + lineOffsetY), Scalar(100.0, 100.0, 100.0, 100.0), 3);
				line(Im4ShowGlobal, Point(barCenterX - lineOffsetX, barCenterY - lineOffsetY), Point(barCenterX + lineOffsetX, barCenterY + lineOffsetY), Scalar(100.0, 100.0, 100.0, 100.0), 3);


			}
			StringOut += "\t";

			//Actin directions if enough signal and calcein is present
			if (!dir1IsNAN & (mean1 >= ProcOptions.treshold1) & (mean2 >= ProcOptions.treshold2))
			{
				StringOut += to_string(1);

				countActinHiCalcein++;
				HistogramDirActinHiCalcein[int(dir1)]++;
			}

			StringOut += "\t";
			//Actin directions if enough signal and calcein is absent
			if (!dir1IsNAN & (mean1 >= ProcOptions.treshold1) & (mean2 < ProcOptions.treshold2))
			{
				StringOut += to_string(1);

				countActinNoCalcein++;
				HistogramDirActinNoCalcein[int(dir1)]++;
			}

			StringOut += "\t";
			//Calcein directions if enough signal and actin is present
			if (!dir2IsNAN & (mean2 >= ProcOptions.treshold2) & (mean1 >= ProcOptions.treshold1))
			{
				StringOut += to_string(1);

				countCalceinHiActin++;
				HistogramDirCalceinHiActin[int(dir2)]++;
			}

			StringOut += "\t";
			//Calcein directions if enough signal and actin is absent
			if (!dir2IsNAN & (mean2 >= ProcOptions.treshold2) & (mean1 < ProcOptions.treshold1))
			{
				StringOut += to_string(1);

				countCalceinNoActin++;
				HistogramDirCalceinNoActin[int(dir2)]++;
			}

			countAll++;
			StringOut += "\n";
			
		}
		inFile1.close();
		inFile2.close();
		//string LocalFullFileNameOut = ProcOptions.OutFolderName1 + FileNameCore + ".txt";
		//ofstream outFile(LocalFullFileNameOut);//FileToProcess.path().filename().string());
		//outFile << StringOut;
		//outFile.close();
		
		cout << ItoStrLZ(fileCounter, 3) << "\n";

		//cout << File1;
		CommonString += "File count \t ";
		CommonString += ItoStrLZ(fileCounter, 3);
		CommonString += "\n ";
		CommonString += StringOut;
		CommonString += "\n";
		fileCounter++;
		

		Mat BigImage(Size(maxX * 2, maxY * 2), Im1ShowGlobal.type());
		Im1ShowGlobal.copyTo(BigImage(Rect(0, 0, maxX, maxY)));
		Im2ShowGlobal.copyTo(BigImage(Rect(maxX, 0, maxX, maxY)));
		Im3ShowGlobal.copyTo(BigImage(Rect(0, maxY, maxX, maxY)));
		Im4ShowGlobal.copyTo(BigImage(Rect(maxX, maxY, maxX, maxY)));


		string LocalFullFileNameOut = ProcOptions.OutFolderName1 + Path3.stem().string() + ".bmp";
		imwrite(LocalFullFileNameOut, BigImage);

		LocalFullFileNameOut = ProcOptions.OutFolderName2 + Path4.stem().string() + ".bmp";
		imwrite(LocalFullFileNameOut, Im3ShowGlobal);
		//break;
	}
	string additions = "";
	additions += "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	additions += "dir\t";
	for (int i = 0; i <= 90; i++)
	{
		additions += to_string(i);
		additions += "\t";
	}
	additions += "\n";
	additions += "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	additions += "mean diff =\t";
	additions += to_string(sumDirDiff/(double(countDirDiff)));
	additions += "\t";
	additions += "count diff =\t";
	additions += to_string(countDirDiff);
	additions += "\t";
	additions += "count all =\t";
	additions += to_string(countAll);
	additions += "\t";

	additions += "Hist Dir Dif\t";
	for (int i = 0; i <= 90; i++)
	{
		additions += to_string(HistogramDirDiff[i]);
		additions += "\t";
	}
	additions += "\n";

	// rest of histograms
	additions += "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	additions += "dir\t";
	for (int i = 180; i > -180; i-= 30)
	{
		additions += to_string(i);
		additions += "\t\t\t\t\t";
		additions += "\t\t\t\t\t";
		additions += "\t\t\t\t\t";
		additions += "\t\t\t\t\t";
		additions += "\t\t\t\t\t";
		additions += "\t\t\t\t\t";
	}
	additions += "\n";

	//actin overal
	additions += "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	additions += "Actin dir  count =\t";
	additions += to_string(countActinOveral);
	additions += "\t";
	additions += "Actin dir\t";
	additions += to_string(HistogramDirActinOveral[0]);
	additions += "\t";
	for (int i = 179; i > 0; i--)
	{
		additions += to_string(HistogramDirActinOveral[i]);
		additions += "\t";
	}
	additions += to_string(HistogramDirActinOveral[0]);
	additions += "\t";
	for (int i = 179; i > 0; i--)
	{
		additions += to_string(HistogramDirActinOveral[i]);
		additions += "\t";
	}
	additions += "\n";

	//calcein overal
	additions += "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	additions += "Calcein dir  count =\t";
	additions += to_string(countCalceinOveral);
	additions += "\t";
	additions += "Calcein dir\t";
	
	additions += to_string(HistogramDirCalceinOveral[0]);
	additions += "\t";
	for (int i = 179; i > 0; i--)
	{
		additions += to_string(HistogramDirCalceinOveral[i]);
		additions += "\t";
	}
	additions += to_string(HistogramDirCalceinOveral[0]);
	additions += "\t";
	for (int i = 179; i > 0; i--)
	{
		additions += to_string(HistogramDirCalceinOveral[i]);
		additions += "\t";
	}
	
	additions += "\n";

	//actin calcein present
	additions += "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	additions += "Actin with calcelin dir count =\t";
	additions += to_string(countActinHiCalcein);
	additions += "\t";
	additions += "Actin with calcelin dir\t";

	additions += to_string(HistogramDirActinHiCalcein[0]);
	additions += "\t";
	for (int i = 179; i > 0; i--)
	{
		additions += to_string(HistogramDirActinHiCalcein[i]);
		additions += "\t";
	}
	additions += to_string(HistogramDirActinHiCalcein[0]);
	additions += "\t";
	for (int i = 179; i > 0; i--)
	{
		additions += to_string(HistogramDirActinHiCalcein[i]);
		additions += "\t";
	}

	additions += "\n";

	//actin no calcein present
	additions += "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	additions += "Actin without calcelin dir count =\t";
	additions += to_string(countActinNoCalcein);
	additions += "\t";
	additions += "Actin without calcelin dir\t";

	additions += to_string(HistogramDirActinNoCalcein[0]);
	additions += "\t";
	for (int i = 179; i > 0; i--)
	{
		additions += to_string(HistogramDirActinNoCalcein[i]);
		additions += "\t";
	}
	additions += to_string(HistogramDirActinNoCalcein[0]);
	additions += "\t";
	for (int i = 179; i > 0; i--)
	{
		additions += to_string(HistogramDirActinNoCalcein[i]);
		additions += "\t";
	}
	additions += "\n";

	// calcein actin present
	additions += "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	additions += "Calcelin with actin dir count =\t";
	additions += to_string(countCalceinHiActin);
	additions += "\t";
	additions += "Calcelin with actin dir\t";

	additions += to_string(HistogramDirCalceinHiActin[0]);
	additions += "\t";
	for (int i = 179; i > 0; i--)
	{
		additions += to_string(HistogramDirCalceinHiActin[i]);
		additions += "\t";
	}
	additions += to_string(HistogramDirCalceinHiActin[0]);
	additions += "\t";
	for (int i = 179; i > 0; i--)
	{
		additions += to_string(HistogramDirCalceinHiActin[i]);
		additions += "\t";
	}

	additions += "\n";

	// calcein no actin present
	additions += "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	additions += "Calcelin without actin dir count =\t";
	additions += to_string(countCalceinNoActin);
	additions += "\t";
	additions += "Calcelin without actin dir\t";

	additions += to_string(HistogramDirCalceinNoActin[0]);
	additions += "\t";
	for (int i = 179; i > 0; i--)
	{
		additions += to_string(HistogramDirCalceinNoActin[i]);
		additions += "\t";
	}
	additions += to_string(HistogramDirCalceinNoActin[0]);
	additions += "\t";
	for (int i = 179; i > 0; i--)
	{
		additions += to_string(HistogramDirCalceinNoActin[i]);
		additions += "\t";
	}
	additions += "\n";

	//CommonString = additions + CommonString;
	
	string CommonFullFileNameOut = ConfigFile.string()+ "Common_" + ".txt";
	std::ofstream outFile(CommonFullFileNameOut);//FileToProcess.path().filename().string());
	
	outFile <<additions << CommonString;
	outFile.close();
	cout << "\n\n";

	string InputStr;
	cin >> InputStr;
	return 0;
}

