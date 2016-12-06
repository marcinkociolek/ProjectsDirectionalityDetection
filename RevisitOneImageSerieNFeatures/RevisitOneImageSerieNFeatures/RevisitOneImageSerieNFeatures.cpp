// RevisitOneImageSerieNFeatures.cpp : Defines the entry point for the console application.
// RevisitOneImageSerie4features.cpp : Defines the entry point for the console application.
// RevisitOneImageSerie.cpp : Defines the entry point for the console application.
//

#include <boost\filesystem.hpp>
#include <boost/regex.hpp>

#include <iostream>
#include <fstream>

#include <math.h>

#include <ParamFromXML.h>
#include <StringFcLib.h>

#include "StatClass.h"

#define PI 3.14159265

using namespace std;
using namespace boost;
using namespace boost::filesystem;

const int modalityCount = 9;
//#define hitCount 5
//#define fileCount = 181;

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

	path PathToProcess(ProcOptions.OutFolderName1);
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

	DirectionalityStatParams *DirStatPar = new DirectionalityStatParams[modalityCount];

	int *DirCount = new int[91];
	for (int i = 0; i < 91; i++)
	{
		DirCount[i] = 0;
	}

	string StringOut = "";
	string StringOutCommon = "";
	StringOut += ProcOptions.ShowParams();
	StringOut += "\n";

	int fileCount = 0;
	int globalCounter = 0;



	for (directory_entry& FileToProcess : directory_iterator(PathToProcess))
	{
		path InPath = FileToProcess.path();

		if (!exists(InPath))
		{
			cout << InPath.filename().string() << " File not exists" << "\n";
			break;
		}
		regex FilePattern(ProcOptions.InFilePattern2);
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
			regex LinePattern("Tile Y\tTile X\t.+");
			if (regex_match(Line1.c_str(), LinePattern))
				break;

			//			if (Line1 == "Tile Y\tTile X\t")
			//				break;
			StringOutCommon += Line1;
			//StringOutCommon += "\t\t\t\t\t\t\t\t";
			//StringOutCommon += "\n";

		}
		StringOutCommon += Line1;
		//getline(inFile1, Line1);
		//StringOutCommon += Line1;
		StringOutCommon += "\n";


		int x1, y1;
		bool dir1IsNAN;

		//float dirErrorCon, dirErrorEne, dirErrorHom, dirErrorCor;

		int localCounter = 0;
		//		for (int i = 0; i < 10000; i++)
		//		{
		//			LocalErrors[i] = 0.0;
		//		}

		for (int i = 0; i < modalityCount; i++)
		{
			DirStatPar[i].LocalInit();
		}
		
		while (inFile1.good())
		{

			//getline(inFile1, Line1);
			getline(inFile1, Line1, '\t');
			if (Line1 != "")
			{

				y1 = stoi(Line1);
				StringOutCommon += Line1;
				StringOutCommon += "\t";

				getline(inFile1, Line1, '\t');
				x1 = stoi(Line1);
				StringOutCommon += Line1;
				StringOutCommon += "\t";

				for (int i = 0; i < modalityCount; i++)
				{
					getline(inFile1, Line1, '\t');
					
					StringOutCommon += Line1;
					StringOutCommon += "\t";
					if (Line1 == "NAN")
					{
						DirStatPar[i].localDirIsNan = 1;
						DirStatPar[i].globalNaNCount++;
						DirStatPar[i].naNCount++;
						continue;

					}

					DirStatPar[i].localDirIsNan = 0;

					double dir = stod(Line1);
					//DirStatPar[i]. = dir;

					double dirError = dirFomFileName - dir;
					if (dirError > 90.0)
						dirError = dirFomFileName - dir - 180;
					if (dirError < -90.0)
						dirError = dirFomFileName - dir + 180;
					
					DirStatPar[i].localDirError = dirError;


					localCounter++;
					globalCounter++;
					DirStatPar[i].sumOfErrors += dirError;
					DirStatPar[i].sumOfAbsErrors += abs(dirError);
					if (DirStatPar[i].maxAbsError < abs(dirError))
						DirStatPar[i].maxAbsError = abs(dirError);
					DirStatPar[i].sumForStd += dirError * dirError;

					DirStatPar[i].globalSumOfErrors += dirError;
					DirStatPar[i].globalSumOfAbsErrors += abs(dirError);
					if (DirStatPar[i].globalMaxAbsError < abs(dirError))
						DirStatPar[i].globalMaxAbsError = abs(dirError);
					DirStatPar[i].globalSumForStd += dirError * dirError;

					if (dirError)
					{
						DirStatPar[i].globalDirErrorsCount++;
						DirStatPar[i].errorCounter++;
					}


				}
				getline(inFile1, Line1);
				StringOutCommon += Line1;
				StringOutCommon += "\t";

				for (int i = 0; i < modalityCount; i++)
				{
					if (!DirStatPar[i].localDirIsNan)
						StringOutCommon += to_string(DirStatPar[i].localDirError);
					else
						StringOutCommon += "";
					StringOutCommon += "\t";
				}
				StringOutCommon += "\n";

			}
		}

		StringOutCommon += "\n";
		//mean error estimation

		for (int i = 0; i < modalityCount; i++)
		{
			DirStatPar[i].avgAbsError = DirStatPar[i].sumOfAbsErrors / (double)localCounter;
			DirStatPar[i].avgError = DirStatPar[i].sumOfErrors / (double)localCounter;

			DirStatPar[i].DirErrorsMean[(int)dirFomFileName] = DirStatPar[i].avgError;
			DirStatPar[i].DirErrorsAbsMean[(int)dirFomFileName] = DirStatPar[i].avgAbsError;
			DirStatPar[i].DirErrorsStd[(int)dirFomFileName] = sqrt(DirStatPar[i].sumForStd / (double)localCounter);
			DirStatPar[i].DirErrorsMax[(int)dirFomFileName] = DirStatPar[i].maxAbsError;
			DirStatPar[i].DirErrorsCount[(int)dirFomFileName] = DirStatPar[i].errorCounter;

		}

		DirCount[(int)dirFomFileName] = localCounter;

		inFile1.close();
	}

	for (int i = 0; i < modalityCount; i++)
	{
		DirStatPar[i].globalDirErrorsMean = DirStatPar[i].globalSumOfErrors / (double)globalCounter;
		DirStatPar[i].globalDirErrorsAbsMean = DirStatPar[i].globalSumOfAbsErrors / (double)globalCounter;
		DirStatPar[i].globalDirErrorsStd = sqrt(DirStatPar[i].globalSumForStd / (double)globalCounter);

	}

	string CommonFullFileNameOut = ProcOptions.OutFolderName2 + ConfigFile.filename().string() + "_CommonNew" + ".txt";
	std::ofstream outFileCommon(CommonFullFileNameOut);//FileToProcess.path().filename().string());

	outFileCommon << StringOutCommon;
	outFileCommon.close();

	string StringDirStats = ProcOptions.ShowParams();
	StringDirStats += "\n\n";
	StringDirStats += "File\tfeature\tGlobal Error Mean\tGlobal Error ABS Mean\tGlobal Error Std\tGlobal Error Max\tGlobal Error Count\t Global Sample Count\t\tMin Offset\Max Offset";
	StringDirStats += "\n";
	StringDirStats += ConfigFile.string();
	StringDirStats += "\t";
	StringDirStats += "Contrast";
	StringDirStats += "\t";
	StringDirStats += to_string(globalConDirErrorsMean);
	StringDirStats += "\t";
	StringDirStats += to_string(globalConDirErrorsAbsMean);
	StringDirStats += "\t";
	StringDirStats += to_string(globalConDirErrorsStd);
	StringDirStats += "\t";
	StringDirStats += to_string(globalMaxAbsErrorCon);
	StringDirStats += "\t";
	StringDirStats += to_string(globalConDirErrorsCount);
	StringDirStats += "\t";
	StringDirStats += to_string(globalCounter);
	StringDirStats += "\t\t";
	StringDirStats += to_string(ProcOptions.minOfset);
	StringDirStats += "\t";
	StringDirStats += to_string(ProcOptions.maxOfset);

	StringDirStats += "\n";
	StringDirStats += ConfigFile.string();
	StringDirStats += "\t";
	StringDirStats += "Energy";
	StringDirStats += "\t";
	StringDirStats += to_string(globalEneDirErrorsMean);
	StringDirStats += "\t";
	StringDirStats += to_string(globalEneDirErrorsAbsMean);
	StringDirStats += "\t";
	StringDirStats += to_string(globalEneDirErrorsStd);
	StringDirStats += "\t";
	StringDirStats += to_string(globalMaxAbsErrorEne);
	StringDirStats += "\t";
	StringDirStats += to_string(globalEneDirErrorsCount);
	StringDirStats += "\t";
	StringDirStats += to_string(globalCounter);
	StringDirStats += "\t\t";
	StringDirStats += to_string(ProcOptions.minOfset);
	StringDirStats += "\t";
	StringDirStats += to_string(ProcOptions.maxOfset);

	StringDirStats += "\n";
	StringDirStats += ConfigFile.string();
	StringDirStats += "\t";
	StringDirStats += "Homogeneity";
	StringDirStats += "\t";
	StringDirStats += to_string(globalHomDirErrorsMean);
	StringDirStats += "\t";
	StringDirStats += to_string(globalHomDirErrorsAbsMean);
	StringDirStats += "\t";
	StringDirStats += to_string(globalHomDirErrorsStd);
	StringDirStats += "\t";
	StringDirStats += to_string(globalMaxAbsErrorHom);
	StringDirStats += "\t";
	StringDirStats += to_string(globalHomDirErrorsCount);
	StringDirStats += "\t";
	StringDirStats += to_string(globalCounter);
	StringDirStats += "\t\t";
	StringDirStats += to_string(ProcOptions.minOfset);
	StringDirStats += "\t";
	StringDirStats += to_string(ProcOptions.maxOfset);

	StringDirStats += "\n";
	StringDirStats += ConfigFile.string();
	StringDirStats += "\t";
	StringDirStats += "Correlation";
	StringDirStats += "\t";
	StringDirStats += to_string(globalCorDirErrorsMean);
	StringDirStats += "\t";
	StringDirStats += to_string(globalCorDirErrorsAbsMean);
	StringDirStats += "\t";
	StringDirStats += to_string(globalCorDirErrorsStd);
	StringDirStats += "\t";
	StringDirStats += to_string(globalMaxAbsErrorCor);
	StringDirStats += "\t";
	StringDirStats += to_string(globalCorDirErrorsCount);
	StringDirStats += "\t";
	StringDirStats += to_string(globalCounter);
	StringDirStats += "\t\t";
	StringDirStats += to_string(ProcOptions.minOfset);
	StringDirStats += "\t";
	StringDirStats += to_string(ProcOptions.maxOfset);



	StringDirStats += "\n\n";
	StringDirStats += "Contrast\n";
	StringDirStats += "direction\tError Mean\tError ABS Mean\tError Std\tError Max\tError Count \tSample Count\t\tMin Offset\Max Offset";
	StringDirStats += "\n";
	for (int i = 0; i < 91; i++)
	{
		StringDirStats += to_string(i);
		StringDirStats += "\t";
		StringDirStats += to_string(DirConErrorsMean[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirConErrorsAbsMean[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirConErrorsStd[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirConErrorsMax[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirConErrorsCount[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirCount[i]);

		StringDirStats += "\n";

	}

	StringDirStats += "\n\n";
	StringDirStats += "Energy\n";
	StringDirStats += "direction\tError Mean\tError ABS Mean\tError Std\tError Max\tError Count \tSample Count\t\tMin Offset\Max Offset";
	StringDirStats += "\n";
	for (int i = 0; i < 91; i++)
	{
		StringDirStats += to_string(i);
		StringDirStats += "\t";
		StringDirStats += to_string(DirEneErrorsMean[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirEneErrorsAbsMean[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirEneErrorsStd[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirEneErrorsMax[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirEneErrorsCount[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirCount[i]);

		StringDirStats += "\n";

	}

	StringDirStats += "\n\n";
	StringDirStats += "Homogeneity\n";
	StringDirStats += "direction\tError Mean\tError ABS Mean\tError Std\tError Max\tError Count \tSample Count\t\tMin Offset\Max Offset";
	StringDirStats += "\n";
	for (int i = 0; i < 91; i++)
	{
		StringDirStats += to_string(i);
		StringDirStats += "\t";
		StringDirStats += to_string(DirHomErrorsMean[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirHomErrorsAbsMean[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirHomErrorsStd[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirHomErrorsMax[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirHomErrorsCount[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirCount[i]);

		StringDirStats += "\n";

	}

	StringDirStats += "\n\n";
	StringDirStats += "Correlation\n";
	StringDirStats += "direction\tError Mean\tError ABS Mean\tError Std\tError Max\tError Count \tSample Count\t\tMin Offset\Max Offset";
	StringDirStats += "\n";
	for (int i = 0; i < 91; i++)
	{
		StringDirStats += to_string(i);
		StringDirStats += "\t";
		StringDirStats += to_string(DirCorErrorsMean[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirCorErrorsAbsMean[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirCorErrorsStd[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirCorErrorsMax[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirCorErrorsCount[i]);
		StringDirStats += "\t";
		StringDirStats += to_string(DirCount[i]);

		StringDirStats += "\n";

	}

	string DirStatsFileNameOut = ProcOptions.OutFolderName2 + ConfigFile.filename().string() + "_DirStats" + ".txt";
	std::ofstream dirStatsFileCommon(DirStatsFileNameOut);//FileToProcess.path().filename().string());

	dirStatsFileCommon << StringDirStats;
	dirStatsFileCommon.close();


	//string in;
	//std::cin >> in;
	return 0;
}



