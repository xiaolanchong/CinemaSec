// ChairLink.h: interface for the ChairLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHAIRLINK_H__D383EFFA_3C56_4FA8_92E5_40443C28345B__INCLUDED_)
#define AFX_CHAIRLINK_H__D383EFFA_3C56_4FA8_92E5_40443C28345B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//! позиция кресле в зависимости от другого кресла
enum CHAIR_POS
{
	//!	первое кресло вверху
	pos_top,
	//!	первое кресло справа
	pos_right,
	//!	первое кресло слева
	pos_left,
	//!	первое кресло внизу
	pos_bottom
};

//! представление связи кресел для сохранения на диск
struct ChairLinkDisk
{
	//! вид связи
	CHAIR_POS			m_posFirst;
	//!	1ое кресло в связи (определяющее)
	int					m_nFirstId, 
	//!	2ое кресло в связи
						m_nSecondId;
};

typedef std::vector< ChairLinkDisk >	ChairLinkDiskArray_t;

#endif // !defined(AFX_CHAIRLINK_H__D383EFFA_3C56_4FA8_92E5_40443C28345B__INCLUDED_)
