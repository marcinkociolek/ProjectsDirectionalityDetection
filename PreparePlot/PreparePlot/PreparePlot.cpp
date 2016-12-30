// PreparePlot.cpp : Defines the entry point for the console application.
//

#include <boost\filesystem.hpp>
#include <boost/regex.hpp>

#include <iostream>
#include <fstream>

#include <math.h>

#include "..\..\..\ProjectsLib\LibMarcin\ParamFromXML.h"

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
	int modalityCount = 9;
	
	string arg1 = "E:\\TestImages\\DirectionalityExtensiveTest\\DataAnalisisBarsA0-90F16T08N00000\\";

	string arg2 = "DataAnalisisBarsA0-90F16T08N2000";

	string arg3;

	if (argc > 1)
		arg1 = argv[1];

	if (argc > 2)
		arg2 = argv[2];

	if (argc > 3)
		arg3 = argv[3];
	else
		arg3 = arg1;

	path PathToProcess(arg1);
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
	
	
	
	int fileCount = 0;
	string StringOut = PathToProcess.string();
	StringOut += "\n\n";
	StringOut += "\n";


	string *StringOutModality = new string[modalityCount];
	for (int i = 0; i < modalityCount; i++)
	{
		StringOutModality[i] = "File\tFeature\tGlobal Error Mean\tGlobal Error ABS Mean\tGlobal Error Std\tGlobal Error Max\tGlobal Error Count\t Global Sample Count\tGlobal NaN count\t\tMin Offset\Max Offset";
		StringOutModality[i] += "\n";
	}

	for (directory_entry& FileToProcess : directory_iterator(PathToProcess))
	{
		path InPath = FileToProcess.path();

		if (!exists(InPath))
		{
			cout << InPath.filename().string() << " File not exists" << "\n";
			break;
		}
		regex FilePattern(".+DirStats.txt");
		if (!regex_match(InPath.filename().string().c_str(), FilePattern))
			continue;
		string FileName = InPath.filename().string();
		fileCount++;

		string Line1, Line2;

		std::ifstream inFile1(InPath.string());//FileToProcess.path().filename().string());

		if (!inFile1.is_open())
		{
			cout << InPath.stem() << "could not be openned " << "\n";
			break;
		}

		while (inFile1.good())
		{
			std::getline(inFile1, Line1);
			regex LinePattern("File	feature.+");
			if (regex_match(Line1.c_str(), LinePattern))
				break;
		}
		//getline(inFile1, Line1);
		//StringOut += Line1;
		//StringOut += "\n";
		for (int i = 0; i < modalityCount; i++)
		{
			getline(inFile1, Line1);
			StringOutModality[i] += Line1;
			StringOutModality[i] += "\n";
		}
		inFile1.close();
	}

	for (int i = 0; i < modalityCount; i++)
	{
		StringOut += StringOutModality[i];
		StringOut += "\n\n";
	}

	string DirStatsFileNameOut = PathToProcess.string() + "\\" + arg2;// "_Plot" + ".txt";
	std::ofstream dirStatsFileCommon(DirStatsFileNameOut);//FileToProcess.path().filename().string());

	dirStatsFileCommon << StringOut;
	dirStatsFileCommon.close();

	return 0;
}
