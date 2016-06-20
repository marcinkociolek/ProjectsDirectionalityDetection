// RivisitResultsForDirStat.cpp : Defines the entry point for the console application.
//
#include <boost\filesystem.hpp>
#include <boost/regex.hpp>

#include <iostream>
#include <fstream>

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


	int DirHist[360];
	for (int i = 0; i < 360; i++)
	{
		DirHist[i] = 0;
	}
	string StringOut = "";
	StringOut += ProcOptions.ShowParams();
	StringOut += "\n";

	int histCount = 0;
	int fileCount = 0;
	for (directory_entry& FileToProcess : directory_iterator(PathToProcess))
	{
		path InPath = FileToProcess.path();

		
		if (FileToProcess.path().extension() != ProcOptions.InFileExtension1)
			continue;

		path Path1 = FileToProcess.path();
		if (!exists(Path1))
		{
			cout << Path1.filename().string() << " File not exists" << "\n";
			break;
		}
		regex FilePattern(ProcOptions.InFilePattern1);
		if (!regex_match(InPath.filename().string().c_str(), FilePattern))
			continue;
		
		fileCount++;

		StringOut +=  ItoStrLZ(fileCount,3) + "\t";
		StringOut += InPath.filename().string();
		StringOut += "\n";

		string Line1;// , Line2;

		std::ifstream inFile1(Path1.string());//FileToProcess.path().filename().string());

		if (!inFile1.is_open())
		{
			cout << Path1.stem() << "could not be openned " << "\n";
			break;
		}





		while (inFile1.good())
		{
			getline(inFile1, Line1);
			if (Line1 == "yRoiNr\txRoiNr\tdirection\tDDC\tmean Tile\tstd Tile\tmean tile 2\tstd Tile 2")
				break;
			//StringOut += Line1;
			//StringOut += "\t\t\t\t\t\t\t\t";
		}
		//StringOut += Line1;
		//StringOut += "\t\t";

		int x1, y1;
		float  dir1, dDC1, mean1, std1;
		bool dir1IsNAN;



		while (inFile1.good())
		{
			//getline(inFile1, Line1);
			getline(inFile1, Line1, '\t');
			y1 = atoi(Line1.c_str());
			//StringOut += Line1;
			//StringOut += "\t";

			getline(inFile1, Line1, '\t');
			x1 = atoi(Line1.c_str());
			//StringOut += Line1;
			//StringOut += "\t";

			getline(inFile1, Line1, '\t');
			dir1 = atof(Line1.c_str());
			//StringOut += Line1;
			//StringOut += "\t";

			if (Line1 == "NAN")
				dir1IsNAN = 1;
			else
				dir1IsNAN = 0;

			getline(inFile1, Line1, '\t');
			dDC1 = atof(Line1.c_str());
			//StringOut += Line1;
			//StringOut += "\t";

			getline(inFile1, Line1, '\t');
			mean1 = atof(Line1.c_str());
			//StringOut += Line1;
			//StringOut += "\t";

			getline(inFile1, Line1, '\n');
			std1 = atof(Line1.c_str());
			//StringOut += Line1;
			//StringOut += "\t";


			//StringOut += "\t\t\t";
			
			// dir hist estimation
			if (!dir1IsNAN& (mean1 >= ProcOptions.treshold1))
			{
				DirHist[(int)dir1]++;
				DirHist[(int)dir1 + 180]++;
				histCount++;
			}
		}
 
	}
	// resulting direction estimation
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
			dirForMaxLength = atan2(b, a)/PI * 180.0;
		}
	}
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
	StringOut += "hist";
	StringOut += "\n";
	for (int i = 0; i < 360; i++)
	{
		StringOut += to_string(i);
		StringOut += "\t";
		StringOut += to_string(DirHist[i]);
		StringOut += "\n";

	}
	string CommonFullFileNameOut = ConfigFile.string() + "_DirHist" + ".txt";
	std::ofstream outFile(CommonFullFileNameOut);//FileToProcess.path().filename().string());

	outFile << StringOut;
	outFile.close();
	string in;
	cin >> in;
	return 0;
}

