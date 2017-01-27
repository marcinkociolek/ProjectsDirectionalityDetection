// CreateTheTest.cpp : Defines the entry point for the console application.
//

#include <boost\filesystem.hpp>
#include <boost/regex.hpp>

#include "..\..\..\ProjectsLib\tinyxml\tinyxml.h"
#include "..\..\..\ProjectsLib\tinyxml\tinystr.h"

#include "..\..\..\ProjectsLib\LibMarcin\StringFcLib.h"

#include <iostream>
#include <fstream>

#include <math.h>



#define PI 3.14159265

//using namespace cv;
using namespace std;
using namespace boost;
using namespace boost::filesystem;


//const int stepNr = 180;

int main(int argc, char* argv[])
{
	
	path ConfigFile("C:\\Data\\ExtensiveDirectionalityTest\\F12T03\\F12T03OutMap4Offsets\\config.xml");
	
	bool blur = false; // if false Noise test will be prepared

	int iterStart = 2;
	int iterEnd = 40;
	int ofsetRange = 4;

	int commonAnalisisCount = 6;

	bool createFolders = true;
	bool createPlotFolders = true;
	bool postAnalisis = true;

	string InputFolderNameCommon = "BarsA0-90F12T03";


	string InputFolderNamesMod[11];
	if (blur)
	{
		InputFolderNamesMod[0] = "B01";
		InputFolderNamesMod[1] = "B03";
		InputFolderNamesMod[2] = "B05";
		InputFolderNamesMod[3] = "B07";
		InputFolderNamesMod[4] = "B09";
		InputFolderNamesMod[5] = "B11";
		InputFolderNamesMod[6] = "B13";
		InputFolderNamesMod[7] = "B15";
		InputFolderNamesMod[8] = "B17";
		InputFolderNamesMod[9] = "B19";
		InputFolderNamesMod[10] = "B21";
	}
	else
	{
		InputFolderNamesMod[0] = "N00000";
		InputFolderNamesMod[1] = "N02000";
		InputFolderNamesMod[2] = "N04000";
		InputFolderNamesMod[3] = "N06000";
		InputFolderNamesMod[4] = "N08000";
		InputFolderNamesMod[5] = "N10000";
		InputFolderNamesMod[6] = "N12000";
		InputFolderNamesMod[7] = "N14000";
		InputFolderNamesMod[8] = "N16000";
		InputFolderNamesMod[9] = "N18000";
		InputFolderNamesMod[10] = "N20000";
	}

	//path ExeFile("C:\\Data\\MSVSBuildDir\\HaralickDirectionalityAvgBuildR\\x64\\Release\\HaralickDirectionalityAvg.exe");
	//path ExeFile("C:\\Data\\MSVSBuildDir\\HaralickBasedDirectionalityMapBuild\\x64\\Release\\HaralickBasedDirectionalityMap.exe");
	//path ExeFile("D:\\MSVSBuildDir\\HaralickDirectionalityAvgBuildR\\x64\\Release\\HaralickDirectionalityAvg.exe");
	//path ExeFile("D:\\MSVSBuildDir\\HaralickBasedDirectionalityMapBuild\\x64\\Release\\HaralickBasedDirectionalityMap.exe");
	//path ExeFile2("D:\\Data\\MSVSBuildDir\\RevisitOneImageSerie4featuresBuild\\x64\\Release\\RevisitOneImageSerie9features.exe");
	//path ExeFile3("D:\\Data\\MSVSBuildDir\\PreparePlotBuild\\x64\\Debug\\PreparePlot.exe");
	//path ExeFile("C:\\SoftProjects\\MSVSBuildDir\\HaralickBasedDirectionalityMapBuild\\x64\\Release\\HaralickBasedDirectionalityMap.exe");
	//path ExeFile2("C:\\SoftProjects\\MSVSBuildDir\\RevisitOneImageSerieNFeaturesBuild\\x64\\Release\\RevisitOneImageSerieNFeatures.exe");
	//path ExeFile3("C:\\SoftProjects\\MSVSBuildDir\\PreparePlotBuild\\x64\\Debug\\PreparePlot.exe");
	//path ExeFile4("C:\\SoftProjects\\MSVSBuildDir\\MultithreadExecutionBuild\\x64\\Release\\MultithreadExecution.exe");
	path ExeFile("C:\\MKApp\\HaralickBasedDirectionalityMap.exe");
	path ExeFile2("C:\\MKApp\\RevisitOneImageSerieNFeatures.exe");
	path ExeFile3("C:\\MKApp\\PreparePlot.exe");
	path ExeFile4("C:\\MKApp\\MultithreadExecution.exe");
	

	string CommonAnalisisTxt = "";
	path CommonAnalisisTxtFile;

	string CommandLine = "";
	string CommandLineCommon = "";

	path CommandLineFile;
	for (int k = 0; k < 11; k++)
	{
		path InFolder("C:\\Data\\ExtensiveDirectionalityTest\\InData");

		string InputFolderName = InputFolderNameCommon + InputFolderNamesMod[k];
		string CommonName = InputFolderName + "OutF0C61Off" + ItoStrLZ(ofsetRange, 2) + "Min";

		InFolder /= path(InputFolderName);

		path BaseFolder = ConfigFile.parent_path();

		path Out2Folder = BaseFolder;
		Out2Folder /= path(InputFolderName + "DataAnlOffRng" + ItoStrLZ(ofsetRange, 2));

		path AnalisisTxtFile = BaseFolder;
		AnalisisTxtFile /= path(InputFolderName + ".txt");

		path PostAnalisisTxtFile = BaseFolder;
		PostAnalisisTxtFile /= path(InputFolderName + "POST.txt");
		
		cout << ConfigFile.string();
		cout << "\n";
		cout << BaseFolder.string();
		cout << "\n";

		//return 0;

		string AnalisisTxt = "";
		string PostAnalisisTxt = "";

		if (!exists(ConfigFile))
		{
			cout << ConfigFile.filename().string() << " not exists " << '\n';
			return 0;
		}

		TiXmlDocument doc(ConfigFile.string().c_str());
		doc.LoadFile();

		TiXmlHandle hDoc(&doc);
		TiXmlElement* pElem;
		TiXmlHandle hRoot(0);
		TiXmlHandle hParameters(0);
		TiXmlHandle hPreprocess(0);

		pElem = hDoc.FirstChildElement().Element();

		hRoot = TiXmlHandle(pElem);

		for (int i = iterStart; i <= iterEnd; i++)
		{

			path NewFolder = BaseFolder;
			NewFolder /= path(CommonName + ItoStrLZ(i, 2));

			path NewFile = BaseFolder;
			NewFile /= path(CommonName + ItoStrLZ(i, 2) + ".xml");


			AnalisisTxt += ExeFile.string() + " " + NewFile.string() + "\n";
			PostAnalisisTxt += ExeFile2.string() + " " + NewFile.string() + "\n";

			string TextToModyfy = "";
			// InFolder name

			pElem = hRoot.FirstChild("input").FirstChild("directory").Element();
			if (!pElem)
			{
				cout << "No entry: input directory";
				cout << "\n";
			}
			else if (!pElem->GetText())
			{
				cout << "Empty entry: input directory";
				cout << "\n";
			}
			else
				TextToModyfy = pElem->GetText();

			TextToModyfy = InFolder.string();
			TextToModyfy += "\\";

			pElem->Clear();
			pElem->LinkEndChild(new TiXmlText(TextToModyfy.c_str()));

			// OutFolder name

			pElem = hRoot.FirstChild("output").FirstChild("directory").Element();
			if (!pElem)
			{
				cout << "No entry: input directory";
				cout << "\n";
			}
			else if (!pElem->GetText())
			{
				cout << "Empty entry: input directory";
				cout << "\n";
			}
			else
				TextToModyfy = pElem->GetText();

			TextToModyfy = NewFolder.string();
			TextToModyfy += "\\";

			pElem->Clear();
			pElem->LinkEndChild(new TiXmlText(TextToModyfy.c_str()));


			pElem = hRoot.FirstChild("output2").FirstChild("directory").Element();
			if (!pElem)
			{
				cout << "No entry: input directory";
				cout << "\n";
			}
			else if (!pElem->GetText())
			{
				cout << "Empty entry: input directory";
				cout << "\n";
			}
			else
				TextToModyfy = pElem->GetText();

			TextToModyfy = Out2Folder.string();
			TextToModyfy += "\\";

			pElem->Clear();
			pElem->LinkEndChild(new TiXmlText(TextToModyfy.c_str()));

			pElem = hRoot.FirstChild("parameters").FirstChild("minOfset").Element();
			if (!pElem)
			{
				cout << "No entry: minOfset";
				cout << "\n";
			}
			else if (!pElem->GetText())
			{
				cout << "Empty entry: minOfset";
				cout << "\n";
			}
			else
			{
				TextToModyfy = pElem->GetText();
			}
			TextToModyfy = ItoStrLZ(i, 2);
			pElem->Clear();
			pElem->LinkEndChild(new TiXmlText(TextToModyfy.c_str()));

			pElem = hRoot.FirstChild("parameters").FirstChild("maxOfset").Element();
			if (!pElem)
			{
				cout << "No entry: maxOfset";
				cout << "\n";
			}
			else if (!pElem->GetText())
			{
				cout << "Empty entry: maxOfset";
				cout << "\n";
			}
			else
			{
				TextToModyfy = pElem->GetText();
			}
			//!!!!!!!!!!!!!!! 1 offset
			//TextToModyfy = ItoStrLZ(i, 2);
			//!!!!!!!!!!!!!!! 3 ofsets
			TextToModyfy = ItoStrLZ(i + ofsetRange - 1, 2);
			pElem->Clear();
			pElem->LinkEndChild(new TiXmlText(TextToModyfy.c_str()));


			doc.SaveFile(NewFile.string().c_str());
			if(createFolders)
				create_directory(NewFolder);
		}
		if (createPlotFolders)
			create_directory(Out2Folder);

		PostAnalisisTxt += ExeFile3.string() + " " + Out2Folder.string() + "\\ " + InputFolderName + "plot" + to_string(ofsetRange) + "Ofs.txt";

		std::ofstream dirStatsFileCommon(AnalisisTxtFile.string());//FileToProcess.path().filename().string());

		dirStatsFileCommon << AnalisisTxt;
		dirStatsFileCommon << PostAnalisisTxt;
		dirStatsFileCommon.close();




		std::ofstream dirStatsFile2Common(PostAnalisisTxtFile.string());//FileToProcess.path().filename().string());

		//dirStatsFile2Common << AnalisisTxt;
		dirStatsFile2Common << PostAnalisisTxt;
		dirStatsFile2Common.close();
		//string in;
		//cin >> in;

		// cteate commonanalisis files
		if (!(k%commonAnalisisCount))
		{

			CommonAnalisisTxtFile = BaseFolder;
			CommonAnalisisTxtFile /= path(InputFolderName + "c.txt");

		}
		CommonAnalisisTxt += AnalisisTxt;
		CommonAnalisisTxt += PostAnalisisTxt;
		if ((!((k+1)%commonAnalisisCount) && k) || k == 10)
		{
			std::ofstream dirStatsFile3Common(CommonAnalisisTxtFile.string());//FileToProcess.path().filename().string());

			dirStatsFile3Common << CommonAnalisisTxt;
			dirStatsFile3Common.close();
			CommonAnalisisTxt = "";
			CommandLineCommon += ExeFile4.string() + " " + CommonAnalisisTxtFile.string() + "\n";
		}
		else
		{
			CommonAnalisisTxt += "\n";
		}
		// end of cteate commonanalisis files

		CommandLine += ExeFile4.string() + " " + AnalisisTxtFile.string() + "\n";
	}
	CommandLineFile = ConfigFile.parent_path();;
	CommandLineFile /= path("CommandLine.txt");
	std::ofstream CommandLineFileStream(CommandLineFile.string());//FileToProcess.path().filename().string());

	CommandLineFileStream << CommandLine;
	CommandLineFileStream << "\n";
	CommandLineFileStream << CommandLineCommon;
	CommandLineFileStream.close();
	return 0;
}

