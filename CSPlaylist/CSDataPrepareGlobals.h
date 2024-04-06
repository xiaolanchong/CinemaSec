#pragma once

class CSDataPrepareGlobals // from old times module DataPrepare
{
public:
	// construction / destruction
	CSDataPrepareGlobals(int nPictureSide = 64, int nSmallSquareSide = 4);
	~CSDataPrepareGlobals();
	void GetSquareImageFromCurve(const Arr2ub& bigImage, const Vec2fArr& curve, Arr2f& outImage) const;
	void GetBigSquareImageFromCurve(const Arr2ub& bigImage, const Vec2fArr& curve, Arr2f& outImage, const double inflation = 0.1) const;
	void GetWalshCoefs(const Arr2f& image, std::vector<float>& vWalshCoefs) const;
	void GetAverageVector(const Arr2f &image, std::vector <float>& vAverage) const;
	void GetLowResolutionImage(const Arr2f &image, Arr2f &loResImage ) const;

	void GetHist(const Arr2f& image, std::vector <long>& hist) const;
	// Calculates brightness of the image
	float GetBrightness(const Arr2f& image) const;
	// Calculates deviation of the image
	float GetDeviation(const Arr2f& image) const;
	//brief normalizes the vector in the Lp norm (only for p = 1 or p = 2)
	void NormalizeVector(std::vector <float>& vVector, int p) const;
	//normalizes the image
	void NormalizeImage(Arr2f& image) const;
	void NormalizeImageBrightness(Arr2f& image) const;
	Arr2f GetLevelsNoParams(const Arr2f image) const;
	int GetPictureSide() const
	{
		return nPictureSide;
	}

	void GetInscribedQuadrFromCurve(const Vec2fArr& myCurve, Vec2f inscribedQuadr[4]) const;
	void RectToQuadrangleTransformCoefs(const double quadranglePoints[8], const int nWidth, const int nHeight, double Coefs[8]) const;
	void QuadrangleToRectTransformCoefs(const double quadranglePoints[8], const int nWidth, const int nHeight, double Coefs[8]) const;



	void ProjectionMapping(const double Coefs[8], const double x, const double y, double& fx, double& fy) const
	{
		double denominator = Coefs[6]*x + Coefs[7]*y + 1.0;
		fx = (Coefs[0]*x + Coefs[1]*y + Coefs[2]) / denominator;
		fy = (Coefs[3]*x + Coefs[4]*y + Coefs[5]) / denominator;
	}	
	void GetSquareImage(const Arr2ub& bigImage, const Vec2f quadrangle[4], Arr2f& outImage) const; 
	// calculates thresholds by the Albert's algorithm
	void GetThresholds(const Arr2f& image, float & t1, float & t2)  const;

private:
	int nPictureSide;
	int nSmallSquareSide;
};