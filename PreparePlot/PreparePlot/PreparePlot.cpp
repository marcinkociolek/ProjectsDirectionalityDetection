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
	path PathToProcess("E:\\TestImages\\DirectionalityExtensiveTest\\DataAnalisisBarsA0-90F16T08N2000\\");
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
	StringOut += "File\tGlobal Error Mean\tGlobal Error ABS Mean\tGlobal Error Std\tGlobal Error Max\tGlobal Error Count\t Global Sample Count\t\tMin Offset\Max Offset";
	StringOut += "\n";
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
			getline(inFile1, Line1);
			if (Line1 == "File\tGlobal Error Mean\tGlobal Error ABS Mean\tGlobal Error Std\tGlobal Error Max\tGlobal Error Count\t Global Sample Count\t\tMin Offset\Max Offset")
				break;
		}
		getline(inFile1, Line1);
		StringOut += Line1;
		StringOut += "\n";
		inFile1.close();
	}

	string DirStatsFileNameOut = PathToProcess.string() + "_Plot" + ".txt";
	std::ofstream dirStatsFileCommon(DirStatsFileNameOut);//FileToProcess.path().filename().string());

	dirStatsFileCommon << StringOut;
	dirStatsFileCommon.close();


	//string in;
	//std::cin >> in;
	return 0;
}
