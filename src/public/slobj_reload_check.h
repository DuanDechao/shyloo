//对象状态类
/********************************************************************
	created:	2015/12/05
	created:	5:12:2015   15:42
	filename: 	e:\myproject\shyloo\sllib\slobj_reload_check.h
	file path:	e:\myproject\shyloo\sllib
	file base:	slobj_reload_check
	file ext:	h
	author:		ddc
	
	purpose:	对象状态类
*********************************************************************/

#ifndef _SL_OBJ_RELOAD_CHECK_H_
#define _SL_OBJ_RELOAD_CHECK_H_
#include "slsingleton.h"

namespace sl
{
	//对象状态类
	class CObjStat
	{
	public:
		CObjStat(): m_bResetShm(true), m_bCheck(false){}

		void SetNotReset() {m_bResetShm = false;}
		void SetReset() {m_bResetShm = true;}
		bool IsReset() {return m_bResetShm;}
		
		bool GetStat() {return m_bResetShm;}
		void SetStat(bool bResetShm) {m_bResetShm = bResetShm;}

		void ClearCheck() {m_bCheck = false;}
		void SetCheck() {m_bCheck = true;}
		bool IsCheckOK()  {return m_bCheck;}
    private:
		bool	m_bResetShm;
		bool	m_bCheck;
	};
}
#endif