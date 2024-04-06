/****************************************************************************
  chair_state_data.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//=================================================================================================
/** \struct ChairStateData.
    \brief  Structure reflects the current state of a chair. */
//=================================================================================================
struct ChairStateData
{
  bool              bOccupied;       //!< non-zero if a chair is occupied by an object
  bool              bHardOccupied;   //!< non-zero if a chair is persistently occupied by an object
  bool              bMovement;       //!< non-zero if there is some activity inside a chair
  bool              bHardMovement;   //!< non-zero if there is some persistent activity inside a chair
  bool              bHuman;          //!< non-zero if a human has been identified within a chair
  float             humanity;        //!< the degree of how reliable is to qualify a chair as occupied by a human
  float             statNCC;         //!< the current NCC between the current frame and the static background
  float             dynaNCC;         //!< the current NCC between the current frame and the dynamic background
  StaticIntervalLst staticIntervals; //!< intervals of chair's occupation and activity

  /** \brief Function swaps contents of two objects. */
  void swap_state_data( ChairStateData & x )
  {
    std::swap( bOccupied,     x.bOccupied     );
    std::swap( bHardOccupied, x.bHardOccupied );
    std::swap( bMovement,     x.bMovement     );
    std::swap( bHardMovement, x.bHardMovement );
    std::swap( bHuman,        x.bHuman        );
    std::swap( humanity,      x.humanity      );
    std::swap( statNCC,       x.statNCC       );
    std::swap( dynaNCC,       x.dynaNCC       );
    staticIntervals.swap( x.staticIntervals );
  }

  /** \brief Function clears temporal, state data. */
  void clear_state_data()
  {
    bOccupied = false;
    bHardOccupied = false;
    bMovement = false;
    bHardMovement = false;
    bHuman = false;
    humanity = 0.0f;
    statNCC = 0.0f;
    dynaNCC = 0.0f;
    staticIntervals.clear();
  }

  /** \brief Constructor. */
  ChairStateData()
  {
    clear_state_data();
  }
};

typedef  std::vector<ChairStateData>  ChairStateDataArr;

