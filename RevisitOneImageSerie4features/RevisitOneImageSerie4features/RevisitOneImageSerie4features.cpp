// RevisitOneImageSerie4features.cpp : Defines the entry point for the console application.
// RevisitOneImageSerie.cpp : Defines the entry point for the console application.
//

#include <boost\filesystem.hpp>
#include <boost/regex.hpp>

#include <iostream>
#include <fstream>

#include <math.h>

#include "..\..\..\ProjectsLib\LibMarcin\ParamFromXML.h"
#include "..\..\..\ProjectsLib\LibMarcin\StringFcLib.h"

#define PI 3.14159265

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

	float globalConDirErrorsMean;
	float globalConDirErrorsAbsMean;
	float globalConDirErrorsStd;
	float globalConDirErrorsMax = 0.0;
	int   globalConDirErrorsCount = 0;

	float globalEneDirErrorsMean;
	float globalEneDirErrorsAbsMean;
	float globalEneDirErrorsStd;
	float globalEneDirErrorsMax = 0.0;
	int   globalEneDirErrorsCount = 0;

	float globalHomDirErrorsMean;
	float globalHomDirErrorsAbsMean;
	float globalHomDirErrorsStd;
	float globalHomDirErrorsMax = 0.0;
	int   globalHomDirErrorsCount = 0;

	float globalCorDirErrorsMean;
	float globalCorDirErrorsAbsMean;
	float globalCorDirErrorsStd;
	float globalCorDirErrorsMax = 0.0;
	int   globalCorDirErrorsCount = 0;


	float *DirConErrorsMean = new float[91];
	float *DirConErrorsAbsMean = new float[91];
	float *DirConErrorsStd = new float[91];
	float *DirConErrorsMax = new float[91];
	int   *DirConErrorsCount = new int[91];

	float *DirEneErrorsMean = new float[91];
	float *DirEneErrorsAbsMean = new float[91];
	float *DirEneErrorsStd = new float[91];
	float *DirEneErrorsMax = new float[91];
	int   *DirEneErrorsCount = new int[91];

	float *DirHomErrorsMean = new float[91];
	float *DirHomErrorsAbsMean = new float[91];
	float *DirHomErrorsStd = new float[91];
	float *DirHomErrorsMax = new float[91];
	int   *DirHomErrorsCount = new int[91];

	float *DirCorErrorsMean = new float[91];
	float *DirCorErrorsAbsMean = new float[91];
	float *DirCorErrorsStd = new float[91];
	float *DirCorErrorsMax = new float[91];
	int   *DirCorErrorsCount = new int[91];


	int *DirCount = new int[91];
	for (int i = 0; i < 91; i++)
	{
		DirConErrorsMean[i] = 0.0;
		DirConErrorsAbsMean[i] = 0.0;
		DirConErrorsStd[i] = 0.0;
		DirConErrorsMax[i] = 0.0;
		DirConErrorsCount[i] = 0;

		DirEneErrorsMean[i] = 0.0;
		DirEneErrorsAbsMean[i] = 0.0;
		DirEneErrorsStd[i] = 0.0;
		DirEneErrorsMax[i] = 0.0;
		DirEneErrorsCount[i] = 0;

		DirHomErrorsMean[i] = 0.0;
		DirHomErrorsAbsMean[i] = 0.0;
		DirHomErrorsStd[i] = 0.0;
		DirHomErrorsMax[i] = 0.0;
		DirHomErrorsCount[i] = 0;

		DirCorErrorsMean[i] = 0.0;
		DirCorErrorsAbsMean[i] = 0.0;
		DirCorErrorsStd[i] = 0.0;
		DirCorErrorsMax[i] = 0.0;
		DirCorErrorsCount[i] = 0;


		DirCount[i] = 0;
	}


	string StringOut = "";
	string StringOutCommon = "";
	StringOut += ProcOptions.ShowParams();
	StringOut += "\n";

	int fileCount = 0;
	int globalCounter = 0;
	double globalSumOfAbsErrorsCon = 0;
	double globalSumOfErrorsCon = 0;
	float globalMaxAbsErrorCon = 0;
	double globalSumForStdCon = 0;

	double globalSumOfAbsErrorsEne = 0;
	double globalSumOfErrorsEne = 0;
	float globalMaxAbsErrorEne = 0;
	double globalSumForStdEne = 0;

	double globalSumOfAbsErrorsHom = 0;
	double globalSumOfErrorsHom = 0;
	float globalMaxAbsErrorHom = 0;
	double globalSumForStdHom = 0;

	double globalSumOfAbsErrorsCor = 0;
	double globalSumOfErrorsCor = 0;
	float globalMaxAbsErrorCor = 0;
	double globalSumForStdCor = 0;


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
			if (Line1 == "Tile Y\tTile X\t")
				break;
			StringOutCommon += Line1;
			//StringOutCommon += "\t\t\t\t\t\t\t\t";
			StringOutCommon += "\n";

		}
		StringOutCommon += Line1;
		getline(inFile1, Line1);
		StringOutCommon += Line1;
		StringOutCommon += "\n";


		int x1, y1;
		float  dirCon, dirEne, dirHom, dirCor, mean1, std1;
		bool dir1IsNAN;
		int x2, y2;
		float dir2, dDC2, mean2, std2;
		bool dir2IsNAN;

		float dirErrorCon, dirErrorEne, dirErrorHom, dirErrorCor;

		int localCounter = 0;
		//		for (int i = 0; i < 10000; i++)
		//		{
		//			LocalErrors[i] = 0.0;
		//		}

		double sumOfAbsErrorsCon = 0;
		double sumOfErrorsCon = 0;
		float maxAbsErrorCon = 0;
		double sumForStdCon = 0;

		int errorCounterCon = 0;

		double sumOfAbsErrorsEne = 0;
		double sumOfErrorsEne = 0;
		float maxAbsErrorEne = 0;
		double sumForStdEne = 0;

		int errorCounterEne = 0;

		double sumOfAbsErrorsHom = 0;
		double sumOfErrorsHom = 0;
		float maxAbsErrorHom = 0;
		double sumForStdHom = 0;

		int errorCounterHom = 0;

		double sumOfAbsErrorsCor = 0;
		double sumOfErrorsCor = 0;
		float maxAbsErrorCor = 0;
		double sumForStdCor = 0;

		int errorCounterCor = 0;

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
				dirCon = atof(Line1.c_str());
				StringOutCommon += Line1;
				StringOutCommon += "\t";

				if (Line1 == "NAN")
					dir1IsNAN = 1;
				else
					dir1IsNAN = 0;

				getline(inFile1, Line1, '\t');
				dirEne = atof(Line1.c_str());
				StringOutCommon += Line1;
				StringOutCommon += "\t";

				getline(inFile1, Line1, '\t');
				dirHom = atof(Line1.c_str());
				StringOutCommon += Line1;
				StringOutCommon += "\t";

				getline(inFile1, Line1, '\t');
				dirCor = atof(Line1.c_str());
				StringOutCommon += Line1;
				StringOutCommon += "\t";

				getline(inFile1, Line1);//getline(inFile1, Line1, '\t');
				//mean1 = atof(Line1.c_str());
				StringOutCommon += Line1;
				StringOutCommon += "\t";

				//getline(inFile1, Line1, '\n');
				//std1 = atof(Line1.c_str());
				//StringOutCommon += Line1;
				//StringOutCommon += "\t\t";

				dirErrorCon = dirFomFileName - dirCon;
				if (dirErrorCon > 90.0)
					dirErrorCon = dirFomFileName - dirCon - 180;
				if (dirErrorCon < -90.0)
					dirErrorCon = dirFomFileName - dirCon + 180;
				StringOutCommon += to_string(dirErrorCon);

				dirErrorEne = dirFomFileName - dirEne;
				if (dirErrorEne > 90.0)
					dirErrorEne = dirFomFileName - dirEne - 180;
				if (dirErrorEne < -90.0)
					dirErrorEne = dirFomFileName - dirEne + 180;
				StringOutCommon += to_string(dirErrorEne);

				dirErrorHom = dirFomFileName - dirHom;
				if (dirErrorHom > 90.0)
					dirErrorHom = dirFomFileName - dirHom - 180;
				if (dirErrorHom < -90.0)
					dirErrorHom = dirFomFileName - dirHom + 180;
				StringOutCommon += to_string(dirErrorHom);

				dirErrorCor = dirFomFileName - dirCor;
				if (dirErrorCor > 90.0)
					dirErrorCor = dirFomFileName - dirCor - 180;
				if (dirErrorCor < -90.0)
					dirErrorCor = dirFomFileName - dirCor + 180;
				StringOutCommon += to_string(dirErrorCor);


				StringOutCommon += "\n";

				if (!dir1IsNAN)//(& mean1 >= ProcOptions.treshold1))
				{
					localCounter++;
					globalCounter++;
					sumOfErrorsCon += (double)(dirErrorCon);
					sumOfAbsErrorsCon += abs((double)(dirErrorCon));
					if (maxAbsErrorCon < abs(dirErrorCon))
						maxAbsErrorCon = abs(dirErrorCon);
					sumForStdCon += ((double)dirErrorCon) * ((double)dirErrorCon);

					sumOfErrorsEne += (double)(dirErrorEne);
					sumOfAbsErrorsEne += abs((double)(dirErrorEne));
					if (maxAbsErrorEne < abs(dirErrorEne))
						maxAbsErrorEne = abs(dirErrorEne);
					sumForStdEne += ((double)dirErrorEne) * ((double)dirErrorEne);

					sumOfErrorsHom += (double)(dirErrorHom);
					sumOfAbsErrorsHom += abs((double)(dirErrorHom));
					if (maxAbsErrorHom < abs(dirErrorHom))
						maxAbsErrorHom = abs(dirErrorHom);
					sumForStdHom += ((double)dirErrorHom) * ((double)dirErrorHom);

					sumOfErrorsCor += (double)(dirErrorCor);
					sumOfAbsErrorsCor += abs((double)(dirErrorCor));
					if (maxAbsErrorCor < abs(dirErrorCor))
						maxAbsErrorCor = abs(dirErrorCor);
					sumForStdCor += ((double)dirErrorCor) * ((double)dirErrorCor);


					globalSumOfErrorsCon += (double)(dirErrorCon);
					globalSumOfAbsErrorsCon += abs((double)(dirErrorCon));
					if (globalMaxAbsErrorCon < abs(dirErrorCon))
						globalMaxAbsErrorCon = abs(dirErrorCon);
					globalSumForStdCon += ((double)dirErrorCon) * ((double)dirErrorCon);

					if (dirErrorCon)
					{
						globalConDirErrorsCount++;
						errorCounterCon++;
					}

					globalSumOfErrorsEne += (double)(dirErrorEne);
					globalSumOfAbsErrorsEne += abs((double)(dirErrorEne));
					if (globalMaxAbsErrorEne < abs(dirErrorEne))
						globalMaxAbsErrorEne = abs(dirErrorEne);
					globalSumForStdEne += ((double)dirErrorEne) * ((double)dirErrorEne);

					if (dirErrorEne)
					{
						globalEneDirErrorsCount++;
						errorCounterEne++;
					}

					globalSumOfErrorsHom += (double)(dirErrorHom);
					globalSumOfAbsErrorsHom += abs((double)(dirErrorHom));
					if (globalMaxAbsErrorHom < abs(dirErrorHom))
						globalMaxAbsErrorHom = abs(dirErrorHom);
					globalSumForStdHom += ((double)dirErrorHom) * ((double)dirErrorHom);

					if (dirErrorHom)
					{
						globalHomDirErrorsCount++;
						errorCounterHom++;
					}

					globalSumOfErrorsCor += (double)(dirErrorCor);
					globalSumOfAbsErrorsCor += abs((double)(dirErrorCor));
					if (globalMaxAbsErrorCor < abs(dirErrorCor))
						globalMaxAbsErrorCor = abs(dirErrorCor);
					globalSumForStdCor += ((double)dirErrorCor) * ((double)dirErrorCor);

					if (dirErrorCor)
					{
						globalCorDirErrorsCount++;
						errorCounterCor++;
					}
				}
			}
		}

		StringOutCommon += "\n";
		//mean error estimation

		float errorAbsMeanCon = (float)(sumOfAbsErrorsCon / (double)localCounter);
		float errorMeanCon = (float)(sumOfErrorsCon / (double)localCounter);

		DirConErrorsMean[(int)dirFomFileName] = errorMeanCon;
		DirConErrorsAbsMean[(int)dirFomFileName] = errorAbsMeanCon;
		DirConErrorsStd[(int)dirFomFileName] = (float)sqrt(sumForStdCon / (double)localCounter);
		DirConErrorsMax[(int)dirFomFileName] = maxAbsErrorCon;
		DirConErrorsCount[(int)dirFomFileName] = errorCounterCon;

		float errorAbsMeanEne = (float)(sumOfAbsErrorsEne / (double)localCounter);
		float errorMeanEne = (float)(sumOfErrorsEne / (double)localCounter);

		DirEneErrorsMean[(int)dirFomFileName] = errorMeanEne;
		DirEneErrorsAbsMean[(int)dirFomFileName] = errorAbsMeanEne;
		DirEneErrorsStd[(int)dirFomFileName] = (float)sqrt(sumForStdEne / (double)localCounter);
		DirEneErrorsMax[(int)dirFomFileName] = maxAbsErrorEne;
		DirEneErrorsCount[(int)dirFomFileName] = errorCounterEne;

		float errorAbsMeanHom = (float)(sumOfAbsErrorsHom / (double)localCounter);
		float errorMeanHom = (float)(sumOfErrorsHom / (double)localCounter);

		DirHomErrorsMean[(int)dirFomFileName] = errorMeanHom;
		DirHomErrorsAbsMean[(int)dirFomFileName] = errorAbsMeanHom;
		DirHomErrorsStd[(int)dirFomFileName] = (float)sqrt(sumForStdHom / (double)localCounter);
		DirHomErrorsMax[(int)dirFomFileName] = maxAbsErrorHom;
		DirHomErrorsCount[(int)dirFomFileName] = errorCounterHom;

		float errorAbsMeanCor = (float)(sumOfAbsErrorsCor / (double)localCounter);
		float errorMeanCor = (float)(sumOfErrorsCor / (double)localCounter);

		DirCorErrorsMean[(int)dirFomFileName] = errorMeanCor;
		DirCorErrorsAbsMean[(int)dirFomFileName] = errorAbsMeanCor;
		DirCorErrorsStd[(int)dirFomFileName] = (float)sqrt(sumForStdCor / (double)localCounter);
		DirCorErrorsMax[(int)dirFomFileName] = maxAbsErrorCor;
		DirCorErrorsCount[(int)dirFomFileName] = errorCounterCor;


		DirCount[(int)dirFomFileName] = localCounter;


		inFile1.close();
	}
	globalConDirErrorsMean = (float)(globalSumOfErrorsCon / (double)globalCounter);
	globalConDirErrorsAbsMean = (float)(globalSumOfAbsErrorsCon / (double)globalCounter);
	globalConDirErrorsStd = (float)sqrt(globalSumForStdCon / (double)globalCounter);

	globalEneDirErrorsMean = (float)(globalSumOfErrorsEne / (double)globalCounter);
	globalEneDirErrorsAbsMean = (float)(globalSumOfAbsErrorsEne / (double)globalCounter);
	globalEneDirErrorsStd = (float)sqrt(globalSumForStdEne / (double)globalCounter);

	globalHomDirErrorsMean = (float)(globalSumOfErrorsHom / (double)globalCounter);
	globalHomDirErrorsAbsMean = (float)(globalSumOfAbsErrorsHom / (double)globalCounter);
	globalHomDirErrorsStd = (float)sqrt(globalSumForStdHom / (double)globalCounter);

	globalCorDirErrorsMean = (float)(globalSumOfErrorsCor / (double)globalCounter);
	globalCorDirErrorsAbsMean = (float)(globalSumOfAbsErrorsCor / (double)globalCounter);
	globalCorDirErrorsStd = (float)sqrt(globalSumForStdCor / (double)globalCounter);

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



