// MultithreadExecution.cpp : Defines the entry point for the console application.
//

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"

#include <boost\filesystem.hpp>
#include <boost/regex.hpp>

#include <iostream>
#include <fstream>
#include <windows.h>


//#include "..\..\..\ProjectsLib\LibMarcin\ParamFromXML.h"
//#include "..\..\..\ProjectsLib\LibMarcin\DispLib.h"
//#include "..\..\..\ProjectsLib\LibMarcin\StringFcLib.h"

//#include <tinyxml.h>
//#include <tinystr.h>

#define PI 3.14159265

using namespace cv;
using namespace std;
using namespace boost;
using namespace boost::filesystem;


//const int stepNr = 180;


int main(int argc, char* argv[])
{
/*
	if (argc < 2)
	{
		cout << "\nTo few arguments.";
		return 0;
	}
*/
	//path ConfigFile(argv[1]);
	path ConfigFile("E:\\TestImages\\DirectionalityExtensiveTest\\BarsA0-90F16T08N08000POST.txt");

	if (!exists(ConfigFile))
	{
		cout << ConfigFile.filename().string() << " not exists " << '\n';
		return 0;
	}
	std::ifstream inFile1(ConfigFile.string());
	if (!inFile1.is_open())
	{
		cout << ConfigFile.stem() << "could not be openned " << "\n";
		return 0;
	}
	string Line;

	PROCESS_INFORMATION pi[8];
	STARTUPINFOA si[8];
	int processIndex = 0;

	while (inFile1.good())
	{
		getline(inFile1, Line);
		

		ZeroMemory(&pi[processIndex], sizeof(pi[processIndex]));
		ZeroMemory(&si[processIndex], sizeof(si[processIndex]));
		si[processIndex].cb = sizeof(si[processIndex]);
		LPSTR cmdline((char *)Line.c_str());
		//CreateProcessA
		if (!CreateProcessA(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si[processIndex], &pi[processIndex]))
			cout << "no process created";
		WaitForSingleObject(pi[processIndex].hProcess, INFINITE);

		DWORD exitcode;
		while (!GetExitCodeProcess(pi[processIndex].hProcess, &exitcode))
		{
			//processIndex++;
			//if (processIndex > 6)
			//	processIndex = 0;

		}
		CloseHandle(pi[processIndex].hProcess);
		CloseHandle(pi[processIndex].hThread);
		cout << "exitcode recived " << exitcode;

	}
	inFile1.close();
	string in;
	cin >> in;
	return 0;
}
