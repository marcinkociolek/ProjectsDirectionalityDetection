// RevisitOneImageSerie.cpp : Defines the entry point for the console application.
//

#include <boost\filesystem.hpp>
#include <boost/regex.hpp>

#include <iostream>
#include <fstream>

#include <math.h>

#include "..\..\..\ProjectsLib\LibMarcin\ParamFromXML.h"
#include "..\..\..\ProjectsLib\LibMarcin\DispLib.h"
#include "..\..\..\ProjectsLib\LibMarcin\StringFcLib.h"

//#include <tinyxml.h>
//#include <tinystr.h>

#define PI 3.14159265

//using namespace cv;
using namespace std;
using namespace boost;
using namespace boost::filesystem;


//const int stepNr = 180;


int main(int argc, char* argv[])
{
	bool useSecondFile = 0;
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

	int *DirHist = new int[360];
	float *LengthHist = new float[360];
	for (int i = 0; i < 360; i++)
	{
		DirHist[i] = 0;
		LengthHist[i] = 0;
	}
	float *Samples = new float[65536];
	for (int i = 0; i < 65536; i++)
	{
		Samples[i] = 0.0;
	}

	float *LocalErrors = new float[10000];

	float *DirErrorsMean = new float[91];
	float *DirErrorsAbsMean = new float[91];
	float *DirErrorsStd = new float[91];
	float *DirErrorsMax = new float[91];
	for (int i = 0; i < 91; i++)
	{
		DirErrorsMean[i] = 0.0;
		DirErrorsAbsMean[i] = 0.0;
		DirErrorsStd[i] = 0.0;
		DirErrorsMax[i] = 0.0;
	}


	string StringOut = "";
	string StringOutCommon = "";
	StringOut += ProcOptions.ShowParams();
	StringOut += "\n";

	int histCount = 0;

	int fileCount = 0;
	for (directory_entry& FileToProcess : directory_iterator(PathToProcess))
	{
		path InPath = FileToProcess.path();

		if (!exists(InPath))
		{
			cout << InPath.filename().string() << " File not exists" << "\n";
			break;
		}
		regex FilePattern(ProcOptions.InFilePattern1);
		if (!regex_match(InPath.filename().string().c_str(), FilePattern))
			continue;
		// looking for direction in the file name
		string::const_iterator start, end;
		string FileName = InPath.filename().string();
		start = FileName.begin();
		end = FileName.end();
		match_results<std::string::const_iterator> AngleStringIterator;
		match_flag_type flags = boost::match_default;
		regex AngleStringPattern("Angle[[:digit:]]{3}");
		if (!regex_search(start, end, AngleStringIterator, AngleStringPattern, flags))
			continue;

		string AngleString = AngleStringIterator[0];
		start = AngleString.begin();
		end = AngleString.end();
		match_results<std::string::const_iterator> AngleValStringIterator;

		regex AngleValStringPattern("[[:digit:]]{3}");
		if (!regex_search(start, end, AngleValStringIterator, AngleValStringPattern, flags))
			continue;

		string AngleValString = AngleValStringIterator[0];
		float dirFomFileName = stof(AngleValString);

		fileCount++;

		StringOut += ItoStrLZ(fileCount, 3) + "\t";
		StringOut += InPath.filename().string();

		StringOut += "\n";

		string Line1, Line2;

		std::ifstream inFile1(InPath.string());//FileToProcess.path().filename().string());

		if (!inFile1.is_open())
		{
			cout << InPath.stem() << "could not be openned " << "\n";
			break;
		}

		while (inFile1.good())
		{
			getline(inFile1, Line1);
			if (Line1 == "Tile Y\tTile X\tAngle\tBest Angle Count\tMean Intensity\tTile min norm\tTile max norm")//"yRoiNr\txRoiNr\tdirection\tDDC\tmean Tile\tstd Tile\tmean tile 2\tstd Tile 2")
				break;
			StringOutCommon += Line1;
			//StringOutCommon += "\t\t\t\t\t\t\t\t";
			StringOutCommon += "\n";

		}
		StringOutCommon += Line1;
		StringOutCommon += "\n";


		int x1, y1;
		float  dir1, dDC1, mean1, std1;
		bool dir1IsNAN;
		int x2, y2;
		float dir2, dDC2, mean2, std2;
		bool dir2IsNAN;

		float dirError;

		int localCounter = 0;
		for (int i = 0; i < 10000; i++)
		{
			LocalErrors[i] = 0.0;
		}


		while (inFile1.good())
		{

				//getline(inFile1, Line1);
			getline(inFile1, Line1, '\t');
			if (Line1 != "")
			{

				y1 = atoi(Line1.c_str());
				StringOutCommon += Line1;
				StringOutCommon += "\t";

				getline(inFile1, Line1, '\t');
				x1 = atoi(Line1.c_str());
				StringOutCommon += Line1;
				StringOutCommon += "\t";

				getline(inFile1, Line1, '\t');
				dir1 = atof(Line1.c_str());
				StringOutCommon += Line1;
				StringOutCommon += "\t";

				if (Line1 == "NAN")
					dir1IsNAN = 1;
				else
					dir1IsNAN = 0;

				getline(inFile1, Line1, '\t');
				dDC1 = atof(Line1.c_str());
				StringOutCommon += Line1;
				StringOutCommon += "\t";

				getline(inFile1, Line1, '\t');
				mean1 = atof(Line1.c_str());
				StringOutCommon += Line1;
				StringOutCommon += "\t";

				getline(inFile1, Line1, '\n');
				std1 = atof(Line1.c_str());
				StringOutCommon += Line1;
				StringOutCommon += "\t\t";

				dirError = dirFomFileName - dir1;
				StringOutCommon += to_string(dirError);
				//StringOutCommon += Line1;
				//StringOutCommon += "\t";

				//StringOutCommon += "\t\t\t";
				StringOutCommon += "\n";


				if (localCounter < 10000)
					LocalErrors[localCounter] = dirError;
				localCounter++;
				// Actin dir hist estimation
				if (!dir1IsNAN& (mean1 >= ProcOptions.treshold1))
				{
					DirHist[(int)dir1]++;
					DirHist[(int)dir1 + 180]++;
					if (histCount < 65000)
						Samples[histCount] = dir1;
					histCount++;
				}
			}
		}
		StringOutCommon += "\n";
		//mean error estimation
		double sumOfAbsErrors = 0;
		double sumOfErrors = 0;
		float maxAbsError = 0;
		for (int i = 0; i < localCounter; i++)
		{
			sumOfErrors += (double)(LocalErrors[i]);
			sumOfAbsErrors += abs((double)(LocalErrors[i]));
			if (maxAbsError < abs(LocalErrors[i]))
				maxAbsError = abs(LocalErrors[i]);
		}

		float errorAbsMean = (float)(sumOfAbsErrors / (double)localCounter);
		float errorMean = (float)(sumOfErrors / (double)localCounter);

		double sumForStd = 0;
		for (int i = 0; i < localCounter; i++)
		{
			sumForStd += ((double)LocalErrors[i]) * ((double)LocalErrors[i]);
		}

		DirErrorsMean[(int)dirFomFileName] = errorMean;
		DirErrorsAbsMean[(int)dirFomFileName] = errorAbsMean;
		DirErrorsStd[(int)dirFomFileName] = (float)sqrt(sumForStd / (double)localCounter);
		DirErrorsMax[(int)dirFomFileName] = maxAbsError;


	}
/*
	//resulting direction estimation
	float maxLength = 0;
	float dirForMaxLength = 0;
	for (int i = 0; i < 180; i++)
	{
		float a = 0;
		float b = 0;
		for (int k = 0; k < 180; k++)
		{
			int dir = k + i;
			float val = (float)DirHist[dir];

			float dirF = (float)dir / 180.0 * PI;
			a += val*cos(dirF);
			b += val*sin(dirF);
		}
		float length = sqrt(a*a + b*b);
		if (maxLength < length)
		{
			maxLength = length;
			dirForMaxLength = atan2(b, a) / PI * 180.0;
		}
		LengthHist[i] = length;
		LengthHist[i + 180] = length;
	}
	if (dirForMaxLength < 0)
		dirForMaxLength = 180 + dirForMaxLength;

	StringOut += "\n";
	StringOut += "max Lenght\t";
	StringOut += to_string(maxLength);
	StringOut += "\n";

	StringOut += "Angle\t";
	StringOut += to_string(dirForMaxLength);
	StringOut += "\n";

	StringOut += "Hist Count\t";
	StringOut += to_string(histCount);
	StringOut += "\n";


	StringOut += "\n\n";
	StringOut += "Direction\t";
	StringOut += "hist A\t";
	StringOut += "\n";

	for (int i = 0; i < 360; i++)
	{
		StringOut += to_string(180 - i);
		StringOut += "\t";
		StringOut += to_string(DirHist[359 - i]);
		//StringOut += "\t";
		//StringOut += to_string(LengthHist[359 - i]);
		//StringOut += "\t";
		//StringOut += to_string(DirHist[359 - i] - dirForMaxLength);
		StringOut += "\n";
	}



	string HistFullFileNameOut = ConfigFile.string() + "_DirHist" + ".txt";
	std::ofstream outFile(HistFullFileNameOut);//FileToProcess.path().filename().string());

	outFile << StringOut;
	outFile.close();

	string StringOutA = "";
	for (int i = 0; i < 65000; i++)
	{
		if (i >= histCount)
			break;
		StringOutA += to_string(Samples[i] - dirForMaxLength);
		StringOutA += "\n";

	}

	CommonFullFileNameOut = ConfigFile.string() + "_DirForTTest" + ".txt";
	std::ofstream outFileA(CommonFullFileNameOut);//FileToProcess.path().filename().string());

	outFileA << StringOutA;
	outFileA.close();
*/

/*
	string ErrorPlotsFileNameOut = ConfigFile.string() + "_DirHist" + ".txt";
	std::ofstream outFile(ErrorPlotsFileNameOut);//FileToProcess.path().filename().string());

	outFile << StringOut;
	outFile.close();

	string StringOutA = "";
	for (int i = 0; i < 65000; i++)
	{
		if (i >= histCount)
			break;
		StringOutA += to_string(Samples[i] - dirForMaxLength);
		StringOutA += "\n";

	}

	CommonFullFileNameOut = ConfigFile.string() + "_DirForTTest" + ".txt";
	std::ofstream outFileA(CommonFullFileNameOut);//FileToProcess.path().filename().string());

	outFileA << StringOutA;
	outFileA.close();
*/

	string CommonFullFileNameOut = ConfigFile.string() + "_CommonNew" + ".txt";
	std::ofstream outFileCommon(CommonFullFileNameOut);//FileToProcess.path().filename().string());

	outFileCommon << StringOutCommon;
	outFileCommon.close();

	string StringDirStats = ProcOptions.ShowParams();
	StringDirStats += "\n\n";
	StringDirStats += "direction\tError Mean\tError ABS Mean\tError Std\tError Max";
	StringDirStats += "\n";
	for(int i = 0; i < 91; i++)
	{
		StringDirStats += to_string(i);
		StringDirStats += "\t";
		StringDirStats += to_string(DirErrorsMean[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirErrorsAbsMean[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirErrorsStd[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirErrorsMax[i]);
		StringDirStats += "\n";

	}

	string DirStatsFileNameOut = ConfigFile.string() + "_DirStats" + ".txt";
	std::ofstream dirStatsFileCommon(DirStatsFileNameOut);//FileToProcess.path().filename().string());

	dirStatsFileCommon << StringDirStats;
	dirStatsFileCommon.close();


	string in;
	//std::cin >> in;
	return 0;
}

