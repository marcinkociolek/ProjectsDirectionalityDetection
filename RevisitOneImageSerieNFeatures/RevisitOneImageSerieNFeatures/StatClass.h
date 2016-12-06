#ifndef StatClass
#define StatClass
#include <stdio.h>

#include <iostream>

using namespace std;

class DirectionalityStatParams
{
public:
	//int size;
	string name;
	double	globalDirErrorsMean;
	double	globalDirErrorsAbsMean;
	double	globalDirErrorsStd;
	double	globalDirErrorsMax;
	int		globalDirErrorsCount;
	int		globalNaNCount;

	double	globalSumOfAbsErrors;
	double	globalSumOfErrors;
	double	globalMaxAbsError;
	double	globalSumForStd;

	double	sumOfAbsErrors;
	double	sumOfErrors;
	double	maxAbsError;
	double	sumForStd;

	double	avgAbsError;
	double	avgError;
	double	StdError;


	int		dirErrorsCount;
	int		naNCount;

	int errorCounter = 0;


	double	*DirErrorsMean;
	double	*DirErrorsAbsMean;
	double	*DirErrorsStd;
	double	*DirErrorsMax;
	int		*DirErrorsCount;

	double localDirError;
	bool localDirIsNan;

	DirectionalityStatParams()
	{
		//size = newSize;
		globalDirErrorsMax = 0.0;
		globalDirErrorsCount = 0;

		DirErrorsMean = new double[181];
		DirErrorsAbsMean = new double[181];
		DirErrorsStd = new double[181];
		DirErrorsMax = new double[181];
		DirErrorsCount = new int[181];

		globalSumOfAbsErrors = 0;
		globalSumOfErrors = 0;
		globalMaxAbsError = 0;
		globalSumForStd = 0;

		sumOfAbsErrors = 0;
		sumOfErrors = 0;
		maxAbsError = 0;
		sumForStd = 0;

		dirErrorsCount = 0;
		naNCount = 0;

		for (int i = 0; i < 181; i++)
		{
			DirErrorsMean[i] = 0.0;
			DirErrorsAbsMean[i] = 0.0;
			DirErrorsStd[i] = 0.0;
			DirErrorsMax[i] = 0.0;
			DirErrorsCount[i] = 0;
		}
	}
	~DirectionalityStatParams()
	{
		delete[] DirErrorsMean;
		delete[] DirErrorsAbsMean;
		delete[] DirErrorsStd;
		delete[] DirErrorsMax;
		delete[] DirErrorsCount;

		DirErrorsMean = 0;
		DirErrorsAbsMean = 0;
		DirErrorsStd = 0;
		DirErrorsMax = 0;
		DirErrorsCount = 0;
	}
	void LocalInit()
	{
		sumOfAbsErrors = 0;
		sumOfErrors = 0;
		maxAbsError = 0;
		sumForStd = 0;
		
		dirErrorsCount = 0;
		naNCount = 0;
		
	}
};

#endif