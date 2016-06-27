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
	bool useSecondFile = true;
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
	path PathToProcess2(ProcOptions.InFolderName2);
	if (useSecondFile)
	{
		
		if (!exists(PathToProcess2))
		{
			cout << PathToProcess2 << " not exists " << '\n';
			return 0;
		}
		if (!is_directory(PathToProcess2))
		{
			cout << PathToProcess2 << " This is not a directory path " << '\n';
			return 0;
		}
	}

	int *DirHistA  = new int[360];
	float *LengthHistA = new float[360];
	for (int i = 0; i < 360; i++)
	{
		DirHistA[i] = 0;
		LengthHistA[i] = 0;
	}
	float *SamplesA = new float[65536];
	for (int i = 0; i < 65536; i++)
	{
		SamplesA[i] = 0.0;
	}

	int *DirHistApC = new int[360];
	float *LengthHistApC = new float[360];
	for (int i = 0; i < 360; i++)
	{
		DirHistApC[i] = 0;
		LengthHistApC[i] = 0;
	}
	float *SamplesApC = new float[65536];
	for (int i = 0; i < 65536; i++)
	{
		SamplesApC[i] = 0.0;
	}

	int *DirHistAmC = new int[360];
	float *LengthHistAmC = new float[360];
	for (int i = 0; i < 360; i++)
	{
		DirHistAmC[i] = 0;
		LengthHistAmC[i] = 0;
	}
	float *SamplesAmC = new float[65536];
	for (int i = 0; i < 65536; i++)
	{
		SamplesAmC[i] = 0.0;
	}


	int *DirHistC = new int[360];
	float *LengthHistC = new float[360];
	for (int i = 0; i < 360; i++)
	{
		DirHistC[i] = 0;
		LengthHistC[i] = 0;
	}
	float *SamplesC = new float[65536];
	for (int i = 0; i < 65536; i++)
	{
		SamplesC[i] = 0.0;
	}

	int *DirHistCpA = new int[360];
	float *LengthHistCpA = new float[360];
	for (int i = 0; i < 360; i++)
	{
		DirHistCpA[i] = 0;
		LengthHistCpA[i] = 0;
	}
	float *SamplesCpA = new float[65536];
	for (int i = 0; i < 65536; i++)
	{
		SamplesCpA[i] = 0.0;
	}

	int *DirHistCmA = new int[360];
	float *LengthHistCmA = new float[360];
	for (int i = 0; i < 360; i++)
	{
		DirHistCmA[i] = 0;
		LengthHistCmA[i] = 0;
	}
	float *SamplesCmA = new float[65536];
	for (int i = 0; i < 65536; i++)
	{
		SamplesCmA[i] = 0.0;
	}


	string StringOut = "";
	string StringOutCommon = "";
	StringOut += ProcOptions.ShowParams();
	StringOut += "\n";

	int histCountA = 0;
	int histCountApC = 0;
	int histCountAmC = 0;
	int histCountC = 0;
	int histCountCpA = 0;
	int histCountCmA = 0;

	int fileCount = 0;
	for (directory_entry& FileToProcess : directory_iterator(PathToProcess))
	{
		path InPath1 = FileToProcess.path();

		
		if (FileToProcess.path().extension() != ProcOptions.InFileExtension1)
			continue;

		//path Path1 = FileToProcess.path();
		if (!exists(InPath1))
		{
			cout << InPath1.filename().string() << " File not exists" << "\n";
			break;
		}
		regex FilePattern(ProcOptions.InFilePattern1);
		if (!regex_match(InPath1.filename().string().c_str(), FilePattern))
			continue;
		// looking for z number
		string::const_iterator start, end;
		string FileName1 = InPath1.filename().string();
		start = FileName1.begin();
		end = FileName1.end();
		match_results<std::string::const_iterator> ZNumberStringIterator;
		match_flag_type flags = boost::match_default;
		regex ZNumberPattern("z[[:digit:]]{3}");
		if (!regex_search(start, end, ZNumberStringIterator, ZNumberPattern, flags))
			continue;
	    
		string ZNumberString1 = ZNumberStringIterator[0];
		//cout << ZNumberString1 << "\n";
		//string inT;
		//std::cin >> inT;
		bool  secondFileNotFound = true;
		path InPath2;
		if (useSecondFile)
		{
			string ZNumberString2;
			for (directory_entry& FileToProcess2 : directory_iterator(PathToProcess2))
			{
				InPath2 = FileToProcess2.path();
				
				if (FileToProcess2.path().extension() != ProcOptions.InFileExtension2)
					continue;

				// Path2 = FileToProcess2.path();
				if (!exists(InPath2))
				{
					cout << InPath2.filename().string() << " File not exists" << "\n";
					break;
				}
				regex FilePattern2(ProcOptions.InFilePattern2);
				if (!regex_match(InPath2.filename().string().c_str(), FilePattern2))
					continue;
				// looking for z number
				string FileName2 = InPath2.filename().string();
				start = FileName2.begin();
				end = FileName2.end();
				match_results<std::string::const_iterator> ZNumberStringIterator2;
				if (!regex_search(start, end, ZNumberStringIterator2, ZNumberPattern, flags))
					continue;

				ZNumberString2 = ZNumberStringIterator2[0];
				if (ZNumberString2 != ZNumberString1)
					continue;
				else
				{
					secondFileNotFound = false;
					break;
				}
			}


			if (secondFileNotFound)
				continue;
		}
		fileCount++;

		StringOut +=  ItoStrLZ(fileCount,3) + "\t";
		StringOut += InPath1.filename().string();
		if (useSecondFile)
		{
			StringOut += "\t";
			StringOut += InPath2.filename().string();
		}
		
		StringOut += "\n";

		string Line1, Line2;

		std::ifstream inFile1(InPath1.string());//FileToProcess.path().filename().string());

		if (!inFile1.is_open())
		{
			cout << InPath1.stem() << "could not be openned " << "\n";
			break;
		}

		std::ifstream inFile2(InPath2.string());//FileToProcess.path().filename().string());

		if (useSecondFile & !inFile2.is_open())
		{
			cout << InPath2.stem() << "could not be openned " << "\n";
			break;
		}



		while (inFile1.good())
		{
			getline(inFile1, Line1);
			if (useSecondFile & inFile2.good())
			{
				getline(inFile2, Line2);
			}
			if (Line1 == "yRoiNr\txRoiNr\tdirection\tDDC\tmean Tile\tstd Tile\tmean tile 2\tstd Tile 2")
				break;
			StringOutCommon += Line1;
			StringOutCommon += "\t\t\t\t\t\t\t\t";
			if (useSecondFile & inFile2.good())
			{
				StringOutCommon += Line2;
			}
			StringOutCommon += "\n";

		}
		StringOutCommon += Line1;
		StringOutCommon += "\t\t";
		StringOutCommon += Line2;
		StringOutCommon += "\n";


		int x1, y1;
		float  dir1, dDC1, mean1, std1;
		bool dir1IsNAN;
		int x2, y2;
		float dir2, dDC2, mean2, std2;
		bool dir2IsNAN;




		while (inFile1.good())
		{
			//getline(inFile1, Line1);
			getline(inFile1, Line1, '\t');
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
			StringOutCommon += "\t";

			if (useSecondFile & inFile2.good())
			{
				getline(inFile2, Line2, '\t');
				y2 = atoi(Line2.c_str());
				StringOutCommon += Line2;
				StringOutCommon += "\t";

				getline(inFile2, Line2, '\t');
				x2 = atoi(Line2.c_str());
				StringOutCommon += Line2;
				StringOutCommon += "\t";

				getline(inFile2, Line2, '\t');
				dir2 = atof(Line2.c_str());
				StringOutCommon += Line2;
				StringOutCommon += "\t";

				if (Line2 == "NAN")
					dir2IsNAN = 1;
				else
					dir2IsNAN = 0;

				getline(inFile2, Line2, '\t');
				dDC2 = atof(Line2.c_str());
				StringOutCommon += Line2;
				StringOutCommon += "\t";

				getline(inFile2, Line2, '\t');
				mean2 = atof(Line2.c_str());
				StringOutCommon += Line2;
				StringOutCommon += "\t";

				getline(inFile2, Line2, '\n');
				std2 = atof(Line2.c_str());
				StringOutCommon += Line2;
				//StringOutCommon+= "\t"; 

			}


			//StringOutCommon += Line1;
			//StringOutCommon += "\t";


			//StringOutCommon += "\t\t\t";
			StringOutCommon += "\n";


			// Actin dir hist estimation
			if (!dir1IsNAN& (mean1 >= ProcOptions.treshold1))
			{
				DirHistA[(int)dir1]++;
				DirHistA[(int)dir1 + 180]++;
				if (histCountA < 65000)
					SamplesA[histCountA] = dir1;
				histCountA++;
			}

			if (useSecondFile)
			{
				if (!dir2IsNAN & (mean2 >= ProcOptions.treshold2))
				{
					DirHistC[(int)dir2]++;
					DirHistC[(int)dir2 + 180]++;
					if (histCountC < 65000)
						SamplesC[histCountC] = dir2;
					histCountC++;
				}

				if (!dir1IsNAN & (mean1 >= ProcOptions.treshold1) & (mean2 >= ProcOptions.treshold2))
				{
					DirHistApC[(int)dir1]++;
					DirHistApC[(int)dir1 + 180]++;
					if (histCountApC < 65000)
						SamplesApC[histCountApC] = dir1;
					histCountApC++;
				}

				if (!dir1IsNAN & (mean1 >= ProcOptions.treshold1) & (mean2 < ProcOptions.treshold2))
				{
					DirHistAmC[(int)dir1]++;
					DirHistAmC[(int)dir1 + 180]++;
					if (histCountAmC < 65000)
						SamplesAmC[histCountAmC] = dir1;
					histCountAmC++;
				}

				if (!dir2IsNAN & (mean2 >= ProcOptions.treshold2) & (mean1 >= ProcOptions.treshold1))
				{
					DirHistCpA[(int)dir2]++;
					DirHistCpA[(int)dir2 + 180]++;
					if (histCountCpA < 65000)
						SamplesCpA[histCountCpA] = dir2;
					histCountCpA++;
				}
				if (!dir2IsNAN & (mean2 >= ProcOptions.treshold2) & (mean1 < ProcOptions.treshold1))
				{
					DirHistCmA[(int)dir2]++;
					DirHistCmA[(int)dir2 + 180]++;
					if (histCountCmA < 65000)
						SamplesCmA[histCountCmA] = dir2;
					histCountCmA++;
				}
			}
		}
 
	}
	// Actin resulting direction estimation
	float maxLengthA = 0;
	float dirForMaxLengthA = 0;
	float maxLengthC = 0;
	float dirForMaxLengthC = 0;
	float maxLengthApC = 0;
	float dirForMaxLengthApC = 0;
	float maxLengthAmC = 0;
	float dirForMaxLengthAmC = 0;
	float maxLengthCpA = 0;
	float dirForMaxLengthCpA = 0;
	float maxLengthCmA = 0;
	float dirForMaxLengthCmA = 0;
	for (int i = 0; i < 180; i++)
	{
		float a = 0;
		float b = 0;
		for (int k = 0; k < 180; k++)
		{
			int dir = k + i;
			float val = (float)DirHistA[dir];

			float dirF = (float)dir / 180.0 * PI;
			a += val*cos(dirF);
			b += val*sin(dirF);
		}
		float length = sqrt(a*a + b*b);
		if (maxLengthA < length)
		{
			maxLengthA = length;
			dirForMaxLengthA = atan2(b, a)/PI * 180.0;
		}
		LengthHistA[i] = length;
		LengthHistA[i+180] = length;
	}
	if (dirForMaxLengthA < 0)
		dirForMaxLengthA = 180 + dirForMaxLengthA;

	if (useSecondFile)
	{
		// Calcein resulting direction estimation
		
		for (int i = 0; i < 180; i++)
		{
			float a = 0;
			float b = 0;
			for (int k = 0; k < 180; k++)
			{
				int dir = k + i;
				float val = (float)DirHistC[dir];

				float dirF = (float)dir / 180.0 * PI;
				a += val*cos(dirF);
				b += val*sin(dirF);
			}
			float length = sqrt(a*a + b*b);
			if (maxLengthC < length)
			{
				maxLengthC = length;
				dirForMaxLengthC = atan2(b, a) / PI * 180.0;
			}
			LengthHistC[i] = length;
			LengthHistC[i + 180] = length;
		}
		if (dirForMaxLengthC < 0)
			dirForMaxLengthC = 180 + dirForMaxLengthC;

		// Actin p C resulting direction estimation
		
		for (int i = 0; i < 180; i++)
		{
			float a = 0;
			float b = 0;
			for (int k = 0; k < 180; k++)
			{
				int dir = k + i;
				float val = (float)DirHistApC[dir];

				float dirF = (float)dir / 180.0 * PI;
				a += val*cos(dirF);
				b += val*sin(dirF);
			}
			float length = sqrt(a*a + b*b);
			if (maxLengthApC < length)
			{
				maxLengthApC = length;
				dirForMaxLengthApC = atan2(b, a) / PI * 180.0;
			}
			LengthHistApC[i] = length;
			LengthHistApC[i + 180] = length;
		}
		if (dirForMaxLengthApC < 0)
			dirForMaxLengthApC = 180 + dirForMaxLengthApC;

		// Actin m C resulting direction estimation
		
		for (int i = 0; i < 180; i++)
		{
			float a = 0;
			float b = 0;
			for (int k = 0; k < 180; k++)
			{
				int dir = k + i;
				float val = (float)DirHistAmC[dir];

				float dirF = (float)dir / 180.0 * PI;
				a += val*cos(dirF);
				b += val*sin(dirF);
			}
			float length = sqrt(a*a + b*b);
			if (maxLengthAmC < length)
			{
				maxLengthAmC = length;
				dirForMaxLengthAmC = atan2(b, a) / PI * 180.0;
			}
			LengthHistAmC[i] = length;
			LengthHistAmC[i + 180] = length;
		}
		if (dirForMaxLengthAmC < 0)
			dirForMaxLengthAmC = 180 + dirForMaxLengthAmC;

		// Calcein p A resulting direction estimation

		for (int i = 0; i < 180; i++)
		{
			float a = 0;
			float b = 0;
			for (int k = 0; k < 180; k++)
			{
				int dir = k + i;
				float val = (float)DirHistCpA[dir];

				float dirF = (float)dir / 180.0 * PI;
				a += val*cos(dirF);
				b += val*sin(dirF);
			}
			float length = sqrt(a*a + b*b);
			if (maxLengthCpA < length)
			{
				maxLengthCpA = length;
				dirForMaxLengthCpA = atan2(b, a) / PI * 180.0;
			}
			LengthHistCpA[i] = length;
			LengthHistCpA[i + 180] = length;
		}
		if (dirForMaxLengthCpA < 0)
			dirForMaxLengthCpA = 180 + dirForMaxLengthCpA;

		// Calcein m A resulting direction estimation

		for (int i = 0; i < 180; i++)
		{
			float a = 0;
			float b = 0;
			for (int k = 0; k < 180; k++)
			{
				int dir = k + i;
				float val = (float)DirHistCmA[dir];

				float dirF = (float)dir / 180.0 * PI;
				a += val*cos(dirF);
				b += val*sin(dirF);
			}
			float length = sqrt(a*a + b*b);
			if (maxLengthCmA < length)
			{
				maxLengthCmA = length;
				dirForMaxLengthCmA = atan2(b, a) / PI * 180.0;
			}
			LengthHistCmA[i] = length;
			LengthHistCmA[i + 180] = length;
		}
		if (dirForMaxLengthCmA < 0)
			dirForMaxLengthCmA = 180 + dirForMaxLengthCmA;

	}

	StringOut += "\n";
	StringOut += "max Lenght\t";
	StringOut += to_string(maxLengthA);
	StringOut += "\n";
	StringOut += "Angle\t";
	StringOut += to_string(dirForMaxLengthA);
	StringOut += "\n";

	StringOut += "Hist Count\t";
	StringOut += to_string(histCountA);
	StringOut += "\n";


	StringOut += "\n\n";
	StringOut += "Direction";
	StringOut += "hist A";
	if (useSecondFile)
	{
		StringOut += "hist C";
		StringOut += "hist ApC";
		StringOut += "hist AmC";
		StringOut += "hist CpA";
		StringOut += "hist CmA";
	}
	StringOut += "\n";

	for (int i = 0; i < 360; i++)
	{
		StringOut += to_string(180 - i);
		StringOut += "\t";
		StringOut += to_string(DirHistA[359 - i]);
		//StringOut += "\t";
		//StringOut += to_string(LengthHist[359 - i]);
		//StringOut += "\t";
		//StringOut += to_string(DirHist[359 - i] - dirForMaxLength);
		if (useSecondFile)
		{
			StringOut += "\t";
			StringOut += to_string(DirHistC[359 - i]);

			StringOut += "\t";
			StringOut += to_string(DirHistApC[359 - i]);

			StringOut += "\t";
			StringOut += to_string(DirHistAmC[359 - i]);
			
			StringOut += "\t";
			StringOut += to_string(DirHistCpA[359 - i]);

			StringOut += "\t";
			StringOut += to_string(DirHistCmA[359 - i]);

		}
		
		
		StringOut += "\n";

	}
	string CommonFullFileNameOut = ConfigFile.string() + "_CommonNew" + ".txt";
	std::ofstream outFileCommon(CommonFullFileNameOut);//FileToProcess.path().filename().string());

	outFileCommon << StringOutCommon;
	outFileCommon.close();


	string HistFullFileNameOut = ConfigFile.string() + "_DirHist" + ".txt";
	std::ofstream outFile(HistFullFileNameOut);//FileToProcess.path().filename().string());

	outFile << StringOut;
	outFile.close();

	string StringOutA = "";
	for (int i = 0; i < 65000; i++)
	{
		if (i >= histCountA )
			break;
		StringOutA += to_string(SamplesA[i] - dirForMaxLengthA);
		StringOutA += "\n";

	}

	string CommonFullFileNameOutA = ConfigFile.string() + "_DirAForTTest" + ".txt";
	std::ofstream outFileA(CommonFullFileNameOutA);//FileToProcess.path().filename().string());

	outFileA << StringOutA;
	outFileA.close();

	if (useSecondFile)
	{
		string StringOutC = "";
		for (int i = 0; i < 65000; i++)
		{
			if (i >= histCountC)
				break;
			StringOutC += to_string(SamplesC[i] - dirForMaxLengthC);
			StringOutC += "\n";

		}

		string CommonFullFileNameOutC = ConfigFile.string() + "_DirCForTTest" + ".txt";
		std::ofstream outFileC(CommonFullFileNameOutC);//FileToProcess.path().filename().string());

		outFileC << StringOutC;
		outFileC.close();


		string StringOutApC = "";
		for (int i = 0; i < 65000; i++)
		{
			if (i >= histCountApC)
				break;
			StringOutApC += to_string(SamplesApC[i] - dirForMaxLengthApC);
			StringOutApC += "\n";

		}

		string CommonFullFileNameOutApC = ConfigFile.string() + "_DirApCForTTest" + ".txt";
		std::ofstream outFileApC(CommonFullFileNameOutApC);//FileToProcess.path().filename().string());

		outFileApC << StringOutApC;
		outFileApC.close();


		string StringOutAmC = "";
		for (int i = 0; i < 65000; i++)
		{
			if (i >= histCountAmC)
				break;
			StringOutAmC += to_string(SamplesAmC[i] - dirForMaxLengthAmC);
			StringOutAmC += "\n";

		}

		string CommonFullFileNameOutAmC = ConfigFile.string() + "_DirAmCForTTest" + ".txt";
		std::ofstream outFileAmC(CommonFullFileNameOutAmC);//FileToProcess.path().filename().string());

		outFileAmC << StringOutAmC;
		outFileAmC.close();


		string StringOutCpA = "";
		for (int i = 0; i < 65000; i++)
		{
			if (i >= histCountCpA)
				break;
			StringOutCpA += to_string(SamplesCpA[i] - dirForMaxLengthCpA);
			StringOutCpA += "\n";

		}

		string CommonFullFileNameOutCpA = ConfigFile.string() + "_DirCpAForTTest" + ".txt";
		std::ofstream outFileCpA(CommonFullFileNameOutCpA);//FileToProcess.path().filename().string());

		outFileCpA << StringOutCpA;
		outFileCpA.close();


		string StringOutCmA = "";
		for (int i = 0; i < 65000; i++)
		{
			if (i >= histCountCmA)
				break;
			StringOutCmA += to_string(SamplesCmA[i] - dirForMaxLengthCmA);
			StringOutCmA += "\n";

		}

		string CommonFullFileNameOutCmA = ConfigFile.string() + "_DirCmAForTTest" + ".txt";
		std::ofstream outFileCmA(CommonFullFileNameOutCmA);//FileToProcess.path().filename().string());

		outFileCmA << StringOutCmA;
		outFileCmA.close();
	}
	string in;
	std::cin >> in;
	return 0;
}

