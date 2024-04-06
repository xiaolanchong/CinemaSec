#ifndef ELVEES_NODELIST_INCLUDED__
#define ELVEES_NODELIST_INCLUDED__

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

namespace Elvees
{

///////////////////////////////////////////////////////////////////////////////
// CNodeList
///////////////////////////////////////////////////////////////////////////////

class CNodeList
{
public:
	class Node
	{
	public:
		Node *Next() const;
		Node *Prev() const;

		void RemoveFromList();
		
	protected:
		Node();
		~Node();

		void Next(Node *pNext);
		void Prev(Node *pPrev);

		void AddToList(CNodeList *pList);
		void Unlink();

		friend class CNodeList;

	private:
		Node *m_pNext;
		Node *m_pPrev;
		
		CNodeList *m_pList;
	};
	
	CNodeList();
	
	void PushNode(Node *pNode);
	Node *PopNode();

	void PushLastNode(Node *pNode);
	Node *PopLastNode();

	Node *Head() const;
	Node *End() const;
	
	unsigned Count() const;
	bool Empty() const;
	
private:
	friend void Node::RemoveFromList();
	void RemoveNode(Node *pNode);

	Node *m_pHead; 
	Node *m_pEnd;
	unsigned m_numNodes;
};

///////////////////////////////////////////////////////////////////////////////
// TNodeList
///////////////////////////////////////////////////////////////////////////////

template <class T> class TNodeList : public CNodeList
{
public:
	void PushNode(T *pNode);
	T *PopNode();

	void PushLastNode(T *pNode);
	T *PopLastNode();
	
	T *Head() const;
	T *End() const;
	
	static T *Next(const T *pNode);
	static T *Prev(const T *pNode);
};

template <class T>
void TNodeList<T>::PushNode(T *pNode)
{
	CNodeList::PushNode(pNode);
}

template <class T>
T *TNodeList<T>::PopNode()
{
	return static_cast<T*>(CNodeList::PopNode());
}

template <class T>
void TNodeList<T>::PushLastNode(T *pNode)
{
	CNodeList::PushLastNode(pNode);
}

template <class T>
T *TNodeList<T>::PopLastNode()
{
	return static_cast<T*>(CNodeList::PopLastNode());
}

template <class T>
T *TNodeList<T>::Head() const
{
	return static_cast<T*>(CNodeList::Head());
}

template <class T>
T *TNodeList<T>::End() const
{
	return static_cast<T*>(CNodeList::End());
}

template <class T>
T *TNodeList<T>::Next(const T *pNode)
{
	return static_cast<T*>(pNode->Next());
}

template <class T>
T *TNodeList<T>::Prev(const T *pNode)
{
	return static_cast<T*>(pNode->Prev());
}

///////////////////////////////////////////////////////////////////////////////
// Namespace: Elvees
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Elvees

#endif // ELVEES_NODELIST_INCLUDED__
