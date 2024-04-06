#pragma	once

/// class DayNight
/// author: Alexander Boltnev
/// e-mail: Alexander.Boltnev@biones.com
/// 24 Aug 2005

/// algorithm not by the author

class DayNight
{
public:
	
	/// constructor
	DayNight( float iThreshold = 115.0f, float pThreshold = 65.0f );
	/// destructor
	~DayNight();

	/// returns true for day and false for night
	bool Determine( const Arr2f & picture ) const;

	/// call this function with typical pictures of day and night and obtain parameters for
	/// the method determine()
	bool Learn( const Arr2f & dayPicture, const Arr2f & nightPicture, float & iThreshold, float & pThreshold );

	/// this function runs learning process with video fragments
	/// call it with video fragments of day and night. At least one "day" fragment and one "night" fragment
    /// should be presented. Point out true for day and false for night
	/// video in .avd format 
	/// After learning call GetParameters(...) method to obtain the parameters
	/// the method determine()
	bool Learn( std::wstring videoFile, bool bDay );
	/// the same as previous, but frame by frame
	bool Learn( Arr2ub & frame, bool bDay );
    // function returns false if not enough video fragmnts were presented
	bool GetParameters( float & iThreshold, float & pThreshold );

private:
	float iThr; // intesity threshold
	float pThr; // percent threshold

	csutility::MultiHistogram < float, 2 > videoHisto;
	bool dayInit;
	bool nightInit;

	void GetParametersFromHisto( csutility::MultiHistogram < float, 2 >& histo, float & iThreshold, float & pThreshold );
};