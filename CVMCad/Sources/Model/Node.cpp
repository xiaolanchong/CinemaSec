 /*
*	CVMCad 
*	Copyright(C) 2005 Elvees
*	All rights reserved.
*
*	$Filename: Node.cpp
*	$Author:   Yanakov Leonid
*	$Date:     2005-05-18
*	$Version:  1.0
*	$Access:   Public
*
*	$Description: This is sample description
*
*
*/
#include "Common.hpp"
#include "Configuration.hpp"
#include "Types.hpp"
#include "Node.hpp"


void CNode::SetName( const wstring& sName )
{
	m_sName = sName;
}

void CNode::GetName( wstring& sName )
{
	sName = m_sName;
}

void CNode::GetType( wstring& sType )
{
	sType = m_sType;
}