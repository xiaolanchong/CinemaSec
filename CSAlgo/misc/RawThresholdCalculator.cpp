#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

RawThresholdCalculator::RawThresholdCalculator()
{
}


RawThresholdCalculator::~RawThresholdCalculator()
{

}

void RawThresholdCalculator::LoadRawData(std::vector <float> arr1, std::vector <float> arr2)
{
	ASSERT( arr1.size() > 0 && arr2.size() > 0);
	int i = 0;

	MeanVarAccum < double > accum1;
	for (i = 0; i < (int) arr1.size(); i++ )
	{
		accum1 += (double)arr1[i];
	}
	
	MeanVarAccum < double > accum2;
	for (i = 0; i < (int) arr2.size(); i++ )
	{
		accum2 += (double)arr2[i];
	}
	
	double mean1, dev1;
	accum1.statistics(&mean1, 0, &dev1);

	double mean2, dev2;
	accum2.statistics(&mean2, 0, &dev2);

	if (mean1 < mean2)
	{
		leftArr = arr1;
		rightArr = arr2;
		leftBound = mean1;
		rightBound = mean2;
	}
	else
	{
		leftArr = arr2;
		rightArr = arr1;
		leftBound = mean2;
		rightBound = mean1;
	}

	step = std::min < double >  (dev1, dev2) / 10.0;
}

float RawThresholdCalculator::GetThreshold()
{
	ASSERT( leftBound <= rightBound );
    
	double candidate = leftBound;

	for (int iter = 0; iter < 3; iter++)
	{
		float x = leftBound;
		int err = CountErr( leftBound );
		
		while ( x <= rightBound )
		{
			int newErr = CountErr( x );
			if ( newErr < err )
			{
				err = newErr;
				candidate = x;            
			}
			x += step;	
		}

		leftBound = candidate - 2.0 * step;
		rightBound = candidate + 2.0 * step;

        step = step / 10.0;
	}

	return candidate;
}

int RawThresholdCalculator::CountErr( double threshold )
{
	int i = 0;

	int err = 0;
	
	for (i = 0; i < (int) rightArr.size(); i++ )
	{
		if ( rightArr[i] < threshold )
		{
			err += 1;
		}
	}

	for (i = 0; i < (int) leftArr.size(); i++ )
	{
		if ( leftArr[i] > threshold )
		{
			err += 1;
		}
	}

	return err;
}