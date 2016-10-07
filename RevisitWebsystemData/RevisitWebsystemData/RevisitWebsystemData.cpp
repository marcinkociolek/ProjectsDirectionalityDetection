// RevisitWebsystemData.cpp : Defines the entry point for the console application.
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
	int roiCount = 100;
	int imageCount = 91;
	int featCount = 
	path InputFile("C:\\Data\\GroundTrueForTextFeat\\OutData\\Test002.csv");

	regex NumeralPattern("([0-9])+[.]?([0-9])*");
	regex IntegerPattern("([0-9])");
	regex FileNamePattern(".+[.]ome[.].+");
	if (!exists(InputFile))
	{
		cout << InputFile.filename().string() << " not exists " << '\n';
		return 0;
	}
	string Line;

	std::ifstream inFile(InputFile.string());//FileToProcess.path().filename().string());

	if (!inFile.is_open())
	{
		cout << InputFile.stem() << "could not be openned " << "\n";
		return 0;
	}

	int featureCount = 0;

	string OutStr = "";
	string InStr;

	// chech if begining of fthe file is propperly formated
	if (inFile.good())
	{
		getline(inFile, Line, ',');
		if (Line != "imageName")
		{
			cout << Line << " wrong file format " << "\n";
			cin >> InStr;
			return 0;
		}
		OutStr += Line;
		OutStr += ",";
	}
	else
	{
		cout << Line << " unexpected  end of file " << "\n";
		cin >> InStr;
		return 0;
	}

	if (inFile.good())
	{
		getline(inFile, Line, ',');
		if (Line != "PartitionId")
		{
			cout << Line << " wrong file format " << "\n";
			cin >> InStr;
			return 0;
		}
		OutStr += Line;
		OutStr += ",";
	}
	else
	{
		cout << Line << " unexpected  end of file " << "\n";
		cin >> InStr;
		return 0;
	}
	if (inFile.good())
	{
		getline(inFile, Line, ',');
		if (Line != "GroupId")
		{
			cout << Line << " wrong file format " << "\n";
			cin >> InStr;
			return 0;
		}
		OutStr += Line;
		//OutStr += ",";
	}
	else
	{
		cout << Line << " unexpected  end of file " << "\n";
		cin >> InStr;
		return 0;
	}

	// getinfg feature count
	while(inFile.good())
	{
		getline(inFile, Line, ',');
		if (regex_match(Line.c_str(), FileNamePattern))
			break;

		featureCount++;
		OutStr += ",";
		OutStr += Line;
	}

	// creating feature matrix and vector
	int featMartixSize = featureCount * roiCount;

	double *Features = new double[featMartixSize];
	
	bool *nanFeature = new bool[featureCount];
	double *FeaturesSum = new double[featureCount];
	double *FeaturesDevSum = new double[featureCount];

	double *FeaturesMax = new double[featureCount];
	double *FeaturesMin = new double[featureCount];

	int partitionID, groupID;
	
	for (int i = 0; i < featMartixSize; i++)
	{
		Features[i] = 0;
	}

	for (int i = 0; i < featureCount;  i++)
	{
		FeaturesSum[i] = 0;
		FeaturesDevSum[i] = 0;
		//FeaturesMax[i] = 0;
	}

	// itrating though images
	for (int imageNr = 0; imageNr < imageCount; imageNr++)
	{
		OutStr += "\n";
		OutStr += Line;
		//iterating through ROI/partitions
		for (int roiNr = 1; roiNr <= roiCount; roiNr++)
		{
			
			//check if first column is file name
			if (!regex_match(Line.c_str(), FileNamePattern))
			{
				cout << Line << " Here should be filename " << "\n";
				cin >> InStr;

				delete[] Features;
				delete[] nanFeature;
				delete[] FeaturesSum;
				delete[] FeaturesDevSum;
				delete[] FeaturesMax;
				delete[] FeaturesMin;

				return 0;
			}

			if (inFile.good())
			{
				getline(inFile, Line, ',');
				//check if line is numeral
				if (!regex_match(Line.c_str(), IntegerPattern))
				{
					cout << Line << " wrong ROI id format " << "\n";
					cin >> InStr;

					delete[] Features;
					delete[] nanFeature;
					delete[] FeaturesSum;
					delete[] FeaturesDevSum;
					delete[] FeaturesMax;
					delete[] FeaturesMin;

					return 0;
				}
				//check if roiNr is correct
				if (roiNr != stoi(Line))
				{
					cout << Line << " wrong ROI id number != " << roiNr<< "\n";
					cin >> InStr;

					delete[] Features;
					delete[] nanFeature;
					delete[] FeaturesSum;
					delete[] FeaturesDevSum;
					delete[] FeaturesMax;
					delete[] FeaturesMin;

					return 0;
				}
			}
			else
			{
				cout << Line << " unexpected  end of file " << "\n";
				cin >> InStr;
				return 0;
			}
			if (inFile.good())
			{
				getline(inFile, Line, ',');
				//check if line is numeral
				if (!regex_match(Line.c_str(), IntegerPattern))
				{
					cout << Line << " wrong Group Id format " << "\n";
					cin >> InStr;

					delete[] Features;
					delete[] nanFeature;
					delete[] FeaturesSum;
					delete[] FeaturesDevSum;
					delete[] FeaturesMax;
					delete[] FeaturesMin;

					return 0;
				}
			}
			else
			{
				cout << Line << " unexpected  end of file " << "\n";
				cin >> InStr;

				delete[] Features;
				delete[] nanFeature;
				delete[] FeaturesSum;
				delete[] FeaturesDevSum;
				delete[] FeaturesMax;
				delete[] FeaturesMin;

				return 0;
			}

			//iterating thru features
			for (int featuteNr = 0; featuteNr < featureCount; featuteNr++)
			{
				bool featureisNumeral;
				if (inFile.good())
				{
					getline(inFile, Line, ',');
					//check if line is NaN
					
					featureisNumeral = regex_match(Line.c_str(), IntegerPattern);
					//check if line is numeral Or NAN
					if (!featureisNumeral && Line != "NaN")
					{
						cout << Line << " Feature value is non nomereal or NaN " << "\n";
						cin >> InStr;

						delete[] Features;
						delete[] nanFeature;
						delete[] FeaturesSum;
						delete[] FeaturesDevSum;
						delete[] FeaturesMax;
						delete[] FeaturesMin;

						return 0;
					}
					//check if roiNr is correct
				}

				double featureVal;
				if (featureisNumeral)
					featureVal = stod(Line);
				else
					featureVal = -1111111;

				Features[featuteNr] = featureVal;

				if (roiNr = 1)
				{
					if (featureisNumeral)
						nanFeature[featuteNr] = false;
					else
						nanFeature[featuteNr] = true;

					FeaturesSum[featuteNr] = featureVal;
					FeaturesMax[featuteNr] = featureVal;
					FeaturesMin[featuteNr] = featureVal;

				}
				else
				{
					if (!featureisNumeral)
						nanFeature[featuteNr] = true;

					FeaturesSum[featuteNr] += featureVal;

					if (FeaturesMax[featuteNr] < featureVal)
						FeaturesMax[featuteNr] = featureVal;

					if (FeaturesMin[featuteNr] > featureVal)
						FeaturesMin[featuteNr] = featureVal;
				}
			}
		}
		getline(inFile, Line, ',');
	}

	cout << OutStr;
	cout << "\n\n" << featureCount << "\n";
	cin >> InStr;
	
	delete[] Features;
	delete[] nanFeature;
	delete[] FeaturesSum;
	delete[] FeaturesDevSum;
	delete[] FeaturesMax;
	delete[] FeaturesMin;

	return 0;
}

