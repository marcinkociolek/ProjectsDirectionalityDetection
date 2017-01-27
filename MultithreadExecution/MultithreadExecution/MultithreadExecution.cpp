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

	if (argc < 2)
	{
		cout << "\nTo few arguments.";
		return 0;
	}
	//string arg1;// = "E:\\TestImages\\DirectionalityExtensiveTest01\\BarsA0-90F16T08N10000POST.txt";

	//path ConfigFile(argv[1]);
	path ConfigFile(argv[1]);

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


	list<string> Commands;
	while (inFile1.good())
	{
		getline(inFile1, Line);
		if (Line == "")
			continue;
		Commands.push_back(Line);
	}
	inFile1.close();

	path CommandLineFile2 = ConfigFile.parent_path();;
	CommandLineFile2 /= path(ConfigFile.filename().stem().string() + "x.txt");
//	cout << CommandLineFile2.string() << "\n";

	while (!Commands.empty())
	{
		std::ofstream CommandLineFileStream(CommandLineFile2.string());//FileToProcess.path().filename().string());

//		CommandLineFileStream << CommandLine;
//		CommandLineFileStream << "\n";
//		CommandLineFileStream << CommandLineCommon;
//		CommandLineFileStream.close();
		for (list<string>::iterator iterCommands = Commands.begin(); iterCommands != Commands.end(); iterCommands++)
		{
			CommandLineFileStream << *iterCommands << "\n";
		}
		CommandLineFileStream.close();

		string Command = *Commands.begin();
		ZeroMemory(&pi[processIndex], sizeof(pi[processIndex]));
		ZeroMemory(&si[processIndex], sizeof(si[processIndex]));
		si[processIndex].cb = sizeof(si[processIndex]);
		LPSTR cmdline((char *)Command.c_str());
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

		Commands.pop_front();
	}

	/*
	for (list<string>::iterator iterCommands = Commands.begin(); iterCommands != Commands.end(); iterCommands++)
	{
		cout << *iterCommands << "\n";
		Commands.pop_front();
	}
	*/
	/*
	while (inFile1.good())
	{
		getline(inFile1, Line);
		if (Line == "")
			continue;

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

	*/
	string in;
	cin >> in;
	return 0;
}

