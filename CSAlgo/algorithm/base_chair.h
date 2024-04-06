/****************************************************************************
  base_chair.h
  ---------------------
  begin     : Aug 2004
  author(s) : Albert Akhriev
  email     : aaah@mail.ru
****************************************************************************/

#pragma once

//=================================================================================================
/** \struct BaseChair.
    \brief  Base structure of any chair type. */
//=================================================================================================
struct BaseChair
{
  __int32  cameraNo;         //!< the index of camera that covers control area occupied by this chair
  __int32  index;            //!< unique identifier of this chair
  __int32  leftChairNo;      //!< identifier of the left neighbour chair or -1
  __int32  rightChairNo;     //!< identifier of the right neighbour chair or -1
  __int32  backChairNo[3];   //!< identifier of the back neighbour chairs or -1s
  __int32  frontChairNo[3];  //!< identifier of the front neighbour chairs or -1s
  Vec2fArr curve;            //!< outline curve of the main zone of this chair

  /** \brief Function clears this object. */
  void clear()
  {
    cameraNo = -1;
    index = -1;
    leftChairNo = -1;
    rightChairNo = -1;
    backChairNo[0] = (backChairNo[1] = (backChairNo[2] = -1));
    frontChairNo[0] = (frontChairNo[1] = (frontChairNo[2] = -1));
    curve.clear();
  }

  /** \brief Constructor. */
  BaseChair()
  {
    clear();
  }

  /** \brief Copy operator. */
  BaseChair & operator=( const BaseChair & chair )
  {
    cameraNo     = chair.cameraNo;
    index        = chair.index;
    leftChairNo  = chair.leftChairNo;
    rightChairNo = chair.rightChairNo;

    backChairNo[0] = chair.backChairNo[0];
    backChairNo[1] = chair.backChairNo[1];
    backChairNo[2] = chair.backChairNo[2];

    frontChairNo[0] = chair.frontChairNo[0];
    frontChairNo[1] = chair.frontChairNo[1];
    frontChairNo[2] = chair.frontChairNo[2];

    curve.assign( chair.curve.begin(), chair.curve.end() );
    return (*this);
  }

  /** \brief Function serializes this object in a memory buffer. */
  __int8 * serialize( __int8 * p, csutility::SerializeAction action )
  {
    p = csutility::Serialize( p, cameraNo,        action );
    p = csutility::Serialize( p, index,           action );
    p = csutility::Serialize( p, leftChairNo,     action );
    p = csutility::Serialize( p, rightChairNo,    action );
    p = csutility::Serialize( p, backChairNo[0],  action );
    p = csutility::Serialize( p, backChairNo[1],  action );
    p = csutility::Serialize( p, backChairNo[2],  action );
    p = csutility::Serialize( p, frontChairNo[0], action );
    p = csutility::Serialize( p, frontChairNo[1], action );
    p = csutility::Serialize( p, frontChairNo[2], action );
    p = csutility::SerializeSimpleContainer( p, curve, action );
    return p;
  }
};

typedef  std::vector<BaseChair>  BaseChairArr;

