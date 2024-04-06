// DropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "CSSeqRefiner.h"
#include "DropTarget.h"
#include ".\droptarget.h"


// CDropTarget

IMPLEMENT_DYNAMIC(CDropTarget, COleDropTarget)
CDropTarget::CDropTarget()
{
}

CDropTarget::~CDropTarget()
{
}


BEGIN_MESSAGE_MAP(CDropTarget, COleDropTarget)
END_MESSAGE_MAP()



// CDropTarget message handlers

void CDropTarget::OnDragLeave(CWnd* pWnd)
{
	// TODO: Add your specialized code here and/or call the base class

	COleDropTarget::OnDragLeave(pWnd);
}

DROPEFFECT CDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// TODO: Add your specialized code here and/or call the base class

	return COleDropTarget::OnDragEnter(pWnd, pDataObject, dwKeyState, point);
}

DROPEFFECT CDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// TODO: Add your specialized code here and/or call the base class

	return COleDropTarget::OnDragOver(pWnd, pDataObject, dwKeyState, point);
}

BOOL CDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,	DROPEFFECT dropEffect, CPoint point)
{
	// TODO: Add your specialized code here and/or call the base class

	return COleDropTarget::OnDrop(pWnd, pDataObject, dropEffect, point);
}
