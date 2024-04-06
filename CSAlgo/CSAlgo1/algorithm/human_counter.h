/****************************************************************************
  human_counter.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//=================================================================================================
/** \class HumanCounter.
    \brief Class calculates the number of humans in a hall or in a control area. */
//=================================================================================================
class HumanCounter
{
private:
  double m_humanCount;  //!< human counter

public:
  /** \brief Function reinitializes this object. */
  void Reset()
  {
    m_humanCount = 0.0;
  }

  /** \brief Function processes a chair. */
  void Increment( const Chair & chair )
  {
#if 1
    m_humanCount += ((chair.humanity > 0.5) ? 1.0 : 0.0);
#else
    m_humanCount += chair.humanity;
#endif
  }

  /** \brief Function returns the number of currently encountered humans. */
  int HumanNumber() const
  {
    return (int)(m_humanCount+0.5);
  }

  /** \brief Constructor. */
  HumanCounter()
  {
    Reset();
  }
};

typedef  std::map<int,HumanCounter>  HumanCounterMap;  // camera indices and human counters organized in a map

