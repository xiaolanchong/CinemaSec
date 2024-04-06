//////////////////////////////////////////////////////////////////////////
//DynamicChairBehaviour.h
//author : Alexander Boltnev
//e-mail : Alexander.Boltnev@biones.com
//date   : 30 june 2005  
//revised: Albert Akhriev, 19 Jul 2005, Albert.Akhriev@biones.com
//////////////////////////////////////////////////////////////////////////

#pragma once

class DynamicChairBehaviour
{
private:
  int                                           m_frameCnt;
  std::vector<double>                           m_timeMeans;
  std::vector<double>                           m_timeSquares;
  bool                                          m_bMeanNorm;
  csalgocommon::ImageNormalizer<Arr2f,Vec2iArr> m_normalizer;

public:
	DynamicChairBehaviour(void);
	~DynamicChairBehaviour(void);

	void  Reset( bool bMeanNormalized );
	void  Update( const Arr2f & image, const BaseChairEx & chair );
	float GetDynamicCharacteristic() const;
  int   GetFrameCounter() const { return m_frameCnt; }
};

