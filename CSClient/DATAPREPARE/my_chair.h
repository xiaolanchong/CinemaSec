
#ifndef  MY_CINEMA_CHAIR_H
#define  MY_CINEMA_CHAIR_H


#include "../../CSUtility/csutility.h"
#include "../CSChair/IChairContent.h"

//=================================================================================================
/** \brief ChairContents. */
//=================================================================================================


namespace MyChair
{

struct Chair
{
  int               subFrameNo;      //!< the index of control area (subframe that covers some area of a hall)
  int               id;              //!< unique identifier of this chair
  ChairContents     contents;        //!< the field describes contents of a chair
  Vec2fArr          curve;           //!< the curve that outlines the main zone inside a chair
  MyChair::Chair *           pLeftChair;      //!< pointer to the left neighbour chair or 0
  MyChair::Chair *           pRightChair;     //!< pointer to the right neighbour chair or 0
  MyChair::Chair *           pBackChair[3];   //!< pointers to the back neighbour chairs or 0s
  MyChair::Chair *           pFrontChair[3];  //!< pointers to the front neighbour chairs or 0s

  Vec2i             center;          //!< the central point of chair
  void clear_temporal_data()
  {
    center.set(0,0);

  }

  /** \brief Function clears this object. */
  void clear()
  {
    clear_temporal_data();
    subFrameNo = -1;
    id = -1;
    contents = CHAIR_UNDEFINED;
    alib::CompleteClear( curve );
    pLeftChair = (pRightChair = 0);
    pBackChair[0] = (pBackChair[1] = (pBackChair[2] = 0));
    pFrontChair[0] = (pFrontChair[1] = (pFrontChair[2] = 0));
  }

  /** \brief Constructor. */
  Chair() { clear(); }
};

typedef  std::list<MyChair::Chair>  ChairSet;
}



#endif // MY_CINEMA_CHAIR_H


