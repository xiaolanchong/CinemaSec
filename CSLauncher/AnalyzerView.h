#pragma once

class CLauncherDoc;

// CAnalyzerView view

class CAnalyzerView : public CScrollView
{
	DECLARE_DYNCREATE(CAnalyzerView)

	//bool	bHall;
protected:
	CAnalyzerView();           // protected constructor used by dynamic creation
	virtual ~CAnalyzerView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	int			m_nCameraNo;

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct

	DECLARE_MESSAGE_MAP()
};


