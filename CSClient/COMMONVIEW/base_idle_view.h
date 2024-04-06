#pragma once

//=================================================================================================
/** \class BaseIdleView.
    \brief The basic class that provides OnIdle() method. */
//=================================================================================================
class BaseIdleView : public CScrollView
{
protected:
  DECLARE_DYNCREATE(BaseIdleView)
  BaseIdleView();
  virtual ~BaseIdleView();
  virtual bool OnIdle();
  virtual void OnDraw( CDC * );
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump( CDumpContext & dc ) const;
#endif
  DECLARE_MESSAGE_MAP()
};


