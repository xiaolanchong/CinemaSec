// ChairLink.h: interface for the ChairLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHAIRLINK_H__D383EFFA_3C56_4FA8_92E5_40443C28345B__INCLUDED_)
#define AFX_CHAIRLINK_H__D383EFFA_3C56_4FA8_92E5_40443C28345B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//! ������� ������ � ����������� �� ������� ������
enum CHAIR_POS
{
	//!	������ ������ ������
	pos_top,
	//!	������ ������ ������
	pos_right,
	//!	������ ������ �����
	pos_left,
	//!	������ ������ �����
	pos_bottom
};

//! ������������� ����� ������ ��� ���������� �� ����
struct ChairLinkDisk
{
	//! ��� �����
	CHAIR_POS			m_posFirst;
	//!	1�� ������ � ����� (������������)
	int					m_nFirstId, 
	//!	2�� ������ � �����
						m_nSecondId;
};

typedef std::vector< ChairLinkDisk >	ChairLinkDiskArray_t;

#endif // !defined(AFX_CHAIRLINK_H__D383EFFA_3C56_4FA8_92E5_40443C28345B__INCLUDED_)
