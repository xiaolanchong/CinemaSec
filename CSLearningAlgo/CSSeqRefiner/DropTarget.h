
#pragma once


// CDropTarget command target

class CDropTarget : public COleDropTarget
{
	DECLARE_DYNAMIC(CDropTarget)

public:
	CDropTarget();
	virtual ~CDropTarget();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnDragLeave(CWnd* pWnd);
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,	DROPEFFECT dropEffect, CPoint point);
};


