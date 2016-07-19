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
	path ConfigFile("D:\\Testimages\\execute.txt");

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
	while (inFile1.good())
	{
		getline(inFile1, Line);
		
		PROCESS_INFORMATION pi;
		STARTUPINFO si;

		ZeroMemory(&pi, sizeof(pi));
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		//LPWSTR cmdline(Line);
		//CreateProcessA
		if (!CreateProcessA(NULL, Line.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			cout << "no process created");
		WaitForSingleObject(pi.hProcess, INFINITE);

		DWORD exitcode;
		while (!GetExitCodeProcess(pi.hProcess, &exitcode))
		{
		}
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		cout << "exitcode recived " << exitcode;

	}
	string in;
	cin >> in;
	return 0;
}

