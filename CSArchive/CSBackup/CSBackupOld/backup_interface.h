/****************************************************************************
  backup_interface.h
  ---------------------
  begin     : Nov 2004
  author(s) : S.Murga
  email     : murgas@newmail.ru
****************************************************************************/

#ifndef __CS_BACKUP_INTERFACE_H__
#define __CS_BACKUP_INTERFACE_H__

//=================================================================================================
/** \struct ICSBackUp.
    \brief  Interface class for automatic cyclic backup. */
//=================================================================================================
struct ICSBackUp
{
  virtual HRESULT Release() = 0;
};

#endif // __CS_BACKUP_INTERFACE_H__