#ifndef  VISLIB_GRAY_EDGE_DETECTOR_H
#define  VISLIB_GRAY_EDGE_DETECTOR_H

namespace vislib
{

//=================================================================================================
/** \class GrayEdgeDetector.
    \brief Very simple edge detector that works with gray-scaled images. */
//=================================================================================================
class ALIB_API GrayEdgeDetector  
{
public:
  enum EdgePointType { NO_EDGE = 0, MAYBE_EDGE = 1, YES_EDGE = 2 };

private:
  Vec2fImg m_dirs;           //!< normalized gradient directions at each point
  Arr2f    m_grads;          //!< gradient modules at each point
  Arr2ub   m_edgeMap;        //!< map of edge points marked with one of 'EdgePointType' flag
  float    m_gradNoise;      //!< deviation of gradient noise
  float    m_hiThr;          //!< high threshold on gradient value
  float    m_loThr;          //!< low threshold on gradient value
  bool     m_ok;             //!< processing status

public:                      // P A R A M E T E R S:
  bool     m_bBinarize;      //!< binarize edge map at the end of processing (0 or 255)
  float    m_hiThrMult;      //!< high threshold = m_hiThrMult * m_gradNoise
  float    m_loThrMult;      //!< low  threshold = m_loThrMult * m_gradNoise
 
protected:
  virtual void EdgeTracking();
  virtual void FollowEdge( Vec2i start );

public:
  GrayEdgeDetector();
  virtual ~GrayEdgeDetector();

  // static void  ComputeGradientsByDirectionalFilter( const Arr2f & image, Vec2fImg & dirs, Arr2f & grads, const ARect * pRect = 0 );
  static void  ComputeGradients( const Arr2f & image, Vec2fImg & dirs, Arr2f & grads, const ARect * pRect = 0 );
  static float EstimateGradNoiseAndFindRawEdgePoints( const Vec2fImg & dirs, const Arr2f & grads, Arr2ub & edgeMap, const ARect * pRect = 0 );

  virtual void SetDefaultParameters();
  virtual void ValidateParameters();
  virtual bool Clear();
  virtual bool Run( const Arr2f & image, const ARect * pRect = 0 );

  bool IsOk() const { return m_ok; }

  const Vec2fImg & GetGradDirections() const { ASSERT( m_ok );  return m_dirs;      }
  const Arr2f    & GetGradients()      const { ASSERT( m_ok );  return m_grads;     }
  const Arr2ub   & GetEdgeMap()        const { ASSERT( m_ok );  return m_edgeMap;   }
  float            GetGradNoise()      const { ASSERT( m_ok );  return m_gradNoise; }
};

} // namespace vislib

#endif // VISLIB_GRAY_EDGE_DETECTOR_H


