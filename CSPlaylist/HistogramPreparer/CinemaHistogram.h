/*****************************************************************/
/* CinemaHistogram                                             
/*
/* class for a very simple interval histogram                         
/* author: Alexander Boltnev                                     
/* e-mail: Alexander.Boltnev@biones.com                          
/*****************************************************************/

#pragma once

template <class T>
class CinemaHistogram
{
	struct Entry
	{
		T layer[3];
	};

	std::vector <Entry> histArr;
	T minValue;
	T maxValue;
	T interval;

	bool bReady;
public:
	enum LayerName
	{
		EMPTY = 0,
		HUMAN = 1,
		CLOTH = 2		
	};
	CinemaHistogram(){};
	void Initialize(T minValue, T maxValue, size_t nIntervals)
	{
		ASSERT( std::numeric_limits<T>::is_integer == false );
		assert (nIntervals != 0);
		histArr.resize(nIntervals);

		this->minValue = minValue;
		this->maxValue = maxValue;
		interval = (maxValue - minValue) / nIntervals;
	}
	CinemaHistogram (T minValue, T maxValue, size_t nIntervals)
	{
		Initialize(minValue, maxValue, nIntervals);
	}
	// adds value into the Histogram; returns error if cannot add the value
	bool push_value(T value, LayerName name)
	{	
		if (value < minValue || value > maxValue)
			return false;

		size_t intNum    = (size_t) ((value - minValue) / interval);
		histArr[intNum].layer[name] += (T)1;	
	
		return true;
	}
	size_t size() const
	{
		return histArr.size();
	}

	Entry& operator [] (size_t nIndex)
	{
		return histArr[nIndex];
	}

	const Entry& operator [] (size_t nIndex) const
	{
		return histArr[nIndex];
	}

	CinemaHistogram<T> & operator + (CinemaHistogram<T> & rh)
	{
		ASSERT(histArr.size() == rh.size());
        for (size_t i = 0; i < histArr.size(); i++)
		{
			histArr[i].layer[EMPTY] += rh[i].layer[EMPTY];
			histArr[i].layer[HUMAN] += rh[i].layer[HUMAN];
			histArr[i].layer[CLOTH] += rh[i].layer[CLOTH];
		}
		return (*this);
	}

	CinemaHistogram<T> & operator = (const CinemaHistogram<T> & rh)
	{
		ASSERT(histArr.size() == rh.size());
		for (size_t i = 0; i < histArr.size(); i++)
		{
			histArr[i].layer[EMPTY] = rh[i].layer[EMPTY];
			histArr[i].layer[HUMAN] = rh[i].layer[HUMAN];
			histArr[i].layer[CLOTH] = rh[i].layer[CLOTH];
		}
		return (*this);
	}

	void Clear()
	{
		size_t arrSize = histArr.size();		
		for (size_t i = 0; i < arrSize; i++) 
		{
			 histArr[i].layer[EMPTY] = 0;
			 histArr[i].layer[HUMAN] = 0;
			 histArr[i].layer[CLOTH] = 0;
		}
	}

	void Normilize()
	{
		T summ = 0;
		size_t arrSize = histArr.size();
		for (size_t i = 0; i < arrSize; i++) 
			summ += histArr[i].layer[EMPTY];

		if (summ != 0)
			for (size_t i = 0; i < arrSize; i++) 
				histArr[i].layer[EMPTY] /= summ;		
			
		summ = 0;
		for (size_t i = 0; i < arrSize; i++) 
			summ += histArr[i].layer[HUMAN];

		if (summ != 0)
			for (size_t i = 0; i < arrSize; i++) 
				histArr[i].layer[HUMAN] /= summ;		

		summ = 0;
		for (size_t i = 0; i < arrSize; i++) 
			summ += histArr[i].layer[CLOTH];

		if (summ != 0)
			for (size_t i = 0; i < arrSize; i++) 
				histArr[i].layer[CLOTH] /= summ;		

		bReady = true;
	}
	~CinemaHistogram (void){}


	bool LoadFromText( const std::vector<__int8> & data )
	{
		if (data.size() == 0)
			return false;
		std::stringstream strm;
		strm << reinterpret_cast<const char*>( &(data[0]) ); // &(*(data.begin()))

		for (size_t i = 0; i < histArr.size(); i++)
		{
			size_t index;
			strm >> index;
			strm >> histArr[i].layer[EMPTY];			
			strm >> histArr[i].layer[HUMAN];			
			strm >> histArr[i].layer[CLOTH];
		}
		return true;
	}

	bool SaveToText( std::vector<__int8> & data )
	{
		std::stringstream strm;
		for (size_t i = 0; i < histArr.size(); i++)
			strm << std::setw(3)  << (int) i                 << " "
				 << std::setw(10) << histArr[i].layer[EMPTY] << " "
			     << std::setw(10) << histArr[i].layer[HUMAN] << " "
				 << std::setw(10) << histArr[i].layer[CLOTH] << std::endl;
		
		std::string s = strm.str();
		
		data.resize( s.size() + 1 );
		std::copy(s.begin(), s.end(), data.begin());
		data.back() = '\0';		

		//CString str;
		//str = s.c_str();
		//AfxMessageBox(str);
		return true;
	}
};
