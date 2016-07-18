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
	
	path ConfigFile("E:\\TestImages\\DirectionalityExtensiveTest02\\config.xml");
	
	string InputFolderName = "BarsA0-90F16T08N00000";
	string CommonName = InputFolderName + "OutF0C61Off";
	
	int iterStart = 2;
	int iterEnd = 4;

	
	path ExeFile("D:\\MSVSBuildDir\\HaralickDirectionalityBuildR\\x64\\Release\\HaralickDirectionality.exe");
	
	path BaseFolder = ConfigFile.parent_path();

	path Out2Folder = BaseFolder;
	Out2Folder /= path(InputFolderName + "DataAnalisis");
	
	path InFolder = BaseFolder;
	InFolder /= path(InputFolderName);

	path AnalisisTxtFile = BaseFolder;
	AnalisisTxtFile /= path(InputFolderName + ".txt");

	cout << ConfigFile.string();
	cout << "\n";
	cout << BaseFolder.string();
	cout << "\n";

	//return 0;

	string AnalisisTxt = "";
	
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
		TextToModyfy = ItoStrLZ(i, 2);
		pElem->Clear();
		pElem->LinkEndChild(new TiXmlText(TextToModyfy.c_str()));


		doc.SaveFile(NewFile.string().c_str());

		create_directory(NewFolder);
	}
	create_directory(Out2Folder);

	
	std::ofstream dirStatsFileCommon(AnalisisTxtFile.string());//FileToProcess.path().filename().string());

	dirStatsFileCommon << AnalisisTxt;
	dirStatsFileCommon.close();
	//string in;
	//cin >> in;
	return 0;
}
