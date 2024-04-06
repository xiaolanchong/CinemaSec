#include "NodeList.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

namespace Elvees
{

CNodeList::CNodeList() : m_pHead(0), m_pEnd(0), m_numNodes(0) 
{
}

void CNodeList::PushNode(Node *pNode)
{
	pNode->AddToList(this);
	pNode->Next(m_pHead);
	
	m_pHead = pNode;

	if(!m_pEnd)
		m_pEnd = pNode;
	
	++m_numNodes;
}

CNodeList::Node *CNodeList::PopNode()
{
	Node *pNode = m_pHead;
	
	if(pNode)
	{
		RemoveNode(pNode);
	}
	
	return pNode;
}

void CNodeList::PushLastNode(Node *pNode)
{
	pNode->AddToList(this);
	pNode->Prev(m_pEnd);
	
	m_pEnd = pNode;

	if(!m_pHead)
		m_pHead = pNode;
	
	++m_numNodes;
}

CNodeList::Node *CNodeList::PopLastNode()
{
	Node *pNode = m_pEnd;
	
	if(pNode)
	{
		RemoveNode(pNode);
	}
	
	return pNode;
}

CNodeList::Node *CNodeList::Head() const
{
	return m_pHead;
}

CNodeList::Node *CNodeList::End() const
{
	return m_pEnd;
}

unsigned CNodeList::Count() const
{
	return m_numNodes;
}

bool CNodeList::Empty() const
{
	return (0 == m_numNodes);
}

void CNodeList::RemoveNode(Node *pNode)
{
	if(pNode == m_pHead)
	{
		m_pHead = pNode->Next();
	}

	if(pNode == m_pEnd)
	{
		m_pEnd = pNode->Prev();
	}

	pNode->Unlink();
	
	--m_numNodes;
}

///////////////////////////////////////////////////////////////////////////////
// CNodeList::Node
///////////////////////////////////////////////////////////////////////////////

CNodeList::Node::Node() : m_pNext(0), m_pPrev(0), m_pList(0) 
{
}

CNodeList::Node::~Node() 
{
	try
	{
		RemoveFromList();
	}
	catch(...)
	{
	}
	
	m_pNext = 0;
	m_pPrev = 0;
	m_pList = 0;
}

CNodeList::Node *CNodeList::Node::Next() const
{
	return m_pNext;
}

CNodeList::Node *CNodeList::Node::Prev() const
{
	return m_pPrev;
}

void CNodeList::Node::Next(Node *pNext)
{
	m_pNext = pNext;
	
	if(pNext)
	{
		pNext->m_pPrev = this;
	}
}

void CNodeList::Node::Prev(Node *pPrev)
{
	m_pPrev = pPrev;
	
	if(pPrev)
	{
		pPrev->m_pNext = this;
	}
}

void CNodeList::Node::AddToList(CNodeList *pList)
{
	m_pList = pList;
}

void CNodeList::Node::RemoveFromList()
{
	if(m_pList)
	{
		m_pList->RemoveNode(this);
	}
}

void CNodeList::Node::Unlink()
{
	if(m_pPrev)
	{
		m_pPrev->m_pNext = m_pNext;
	}
	
	if(m_pNext)
	{
		m_pNext->m_pPrev = m_pPrev;
	}
	
	m_pNext = 0;
	m_pPrev = 0;
	
	m_pList = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees
