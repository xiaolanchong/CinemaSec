#pragma once

class EigAlgoImageHolder
{
public:
	EigAlgoImageHolder(int imgSide = 8);
    ~EigAlgoImageHolder(void);
	void AddImage(const Arr2f& image);
	void GetEigenValues(std::vector <float>& eigValues) const;
	Arr2f GetFirstImage() const;
	void SetFirstImage(Arr2f image);

	void MinusAvCorrMatrix();
	void Clear();

	EigAlgoImageHolder & operator = (const EigAlgoImageHolder & rh)
	{
		rh;
		this->Clear();
		return (*this);
	}
	// for histo learn
	void UpdateHisto();
	void GetHisto(csutility::MultiHistogram <float, 2> & histo, int & histoCounter) const;
	void GetAverages(MeanVarAccum < float >& accum);
	void SpliceMomentsListToThisList( std::list < float > & lst);

private:
	Arr2f avImage;
	Arr2f avCorrMatrix;

	Arr2f firstImage;
	Arr2d corrMatrix;
	
	int imgSide;
	long cnt; // frame counter

	//for 
	float HistoTransform(float x);
	csutility::MultiHistogram <float, 2> myHisto;
    int histoCnt;

	MeanVarAccum < float > accum;

	std::list < float > momentsList;
};
/*/
Usage:
	EigAlgoImageHolderArr arr;
	Do in cycle
	for all items in arr
	see if arr[i].first has changed 
	then
		arr[i].second->GetNeedInfo();
		arr[i].second->Clear();
	else
		arr[i].second->UpdateInfo();
	end
	
/*/
typedef std::vector < std::pair< ChairContents, EigAlgoImageHolder > > EigAlgoImageHolderArr;