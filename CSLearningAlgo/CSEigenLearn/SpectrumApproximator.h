//////////////////////////////////////////////////////////////////////////
/*
author: Alexander Boltnev
e-mail: Alexander.Boltnev@biones.com
start date: 7 June 2005

This class finds parameters chatacterizing the distribution
Also gives some helpful functions
*/
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#pragma once

//T shoul be a real type
template <class T>
class SpectrumApproximator
{
public:
	SpectrumApproximator( const std::vector <T> & spectrum );
	T Delta( T a, T b );
	T f( T a, T b, T x );
    void GetApproximationParams( T& a, T& b );
	void GetMoments( T& average, T& deviation );
	~SpectrumApproximator(void);
private:
	T GetAfromB(T b);
	std::vector <T> spectrum;
};

template <class T>
SpectrumApproximator<T>::SpectrumApproximator(const std::vector <T>& spectrum)
{
	this->spectrum = spectrum;
}
template <class T>
SpectrumApproximator<T>::~SpectrumApproximator(void)
{

}

template <class T>
void SpectrumApproximator<T>::GetApproximationParams(T& a, T& b)
{
	T c;
	T d;

	T dmin = (T)(1.0);
	T cmin = GetAfromB(dmin);

	T deltamin = Delta(dmin, cmin);

	for (d = (T)1.0; d < (T)(spectrum.size()); d += (T)1.0)
	{
		c = GetAfromB(d);
		T delta = Delta(c, d);
		if (delta < deltamin)
		{
			deltamin = delta;
			cmin = c;
			dmin = d;
		}
	}
	a = cmin;
	b = dmin;
}

template <class T>
T SpectrumApproximator<T>::Delta(T a, T b)
{

	T summ = 0;
	for (int i = 0; i < (int)spectrum.size(); i++)
	{
		summ += (spectrum[i] - f(a, b, (T)i)) * (spectrum[i] - f(a, b, (T)i));
	}
	return summ;
}

template <class T>
T SpectrumApproximator<T>::f(T a, T b, T x)
{
	return a / (b * b + x * x);
}

template <class T>
T SpectrumApproximator<T>::GetAfromB(T b)
{
	T summ1 = 0;
	T summ2 = 0;

	for (int i = 0; i < (int)spectrum.size(); i++)
	{
		T denom = b * b + (T)(i * i);
		summ1 += spectrum[i] / denom;
		summ2 += (T)(1.0) / denom / denom;
	}
	return summ1 / summ2;
}
template <class T>
void SpectrumApproximator<T>::GetMoments( T& m1, T& m2 )
{
	T mom1 = 0;
	T mom2 = 0;
	for (int i = 0; i < (int)spectrum.size(); i++)
	{	
		mom1 += spectrum[i]; 
		mom2 += (T)i * spectrum[i];
	}

	m1 = mom1 / (T)spectrum.size();
    m2 = mom2 / (T)spectrum.size();
}