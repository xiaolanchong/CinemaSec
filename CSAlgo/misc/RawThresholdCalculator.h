#pragma once

class RawThresholdCalculator
{
public:
	RawThresholdCalculator();
	~RawThresholdCalculator();

	void LoadRawData(std::vector <float> arr1, std::vector <float> arr2);
	float GetThreshold();// prototype will be redone
private:

	float step;
	float threshold;
	float leftBound;
	float rightBound;

	std::vector <float> leftArr;
	std::vector <float> rightArr;



	int CountErr( double threshold );

};