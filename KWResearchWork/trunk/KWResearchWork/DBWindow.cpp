#include "stdafx.h"
#include "DBWindow.h"

//#ifdef _DEBUG

#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>

#define CONSOLE_TILE _T("Debug Info")

class ConsoleWindow 
{
public:
	ConsoleWindow();
	virtual ~ConsoleWindow();

	BOOL SetTile(LPCTSTR lpTile);
	BOOL WriteString(LPCTSTR lpString);
	BOOL WriteString(WORD Attrs,LPCTSTR lpString);
private:
	HANDLE m_hConsole;
	BOOL   m_bCreate;
	BOOL   m_bAttrs;
	WORD   m_OldColorAttrs;
};

ConsoleWindow::ConsoleWindow()
{
	m_hConsole=NULL;
	m_bCreate=FALSE;
	if(AllocConsole())
	{
		m_hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTitle(CONSOLE_TILE);
SetConsoleMode(m_hConsole,ENABLE_PROCESSED_OUTPUT);
m_bCreate=TRUE;
 }
 else{
	 m_hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
	 if(m_hConsole==INVALID_HANDLE_VALUE)
		 m_hConsole=NULL;
 }
 if(m_hConsole)
 {
	 CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	 if(GetConsoleScreenBufferInfo(m_hConsole, &csbiInfo))
	 {
		 m_bAttrs=TRUE;
		 m_OldColorAttrs = csbiInfo.wAttributes;      
	 }
	 else{
		 m_bAttrs=FALSE;
		 m_OldColorAttrs = 0;
	 }
 }
}

ConsoleWindow::~ConsoleWindow()
{
	if(m_bCreate)
		FreeConsole();
}

BOOL ConsoleWindow::SetTile(LPCTSTR lpTile)
{
	return SetConsoleTitle(lpTile);
}

BOOL ConsoleWindow::WriteString(LPCTSTR lpString)
{
	BOOL ret=FALSE;
	if(m_hConsole)
	{
		ret=WriteConsole(m_hConsole,lpString,_tcslen(lpString),NULL,NULL);
	}
	return ret;
}

BOOL ConsoleWindow::WriteString(WORD Attrs,LPCTSTR lpString)
{
	BOOL ret=FALSE;
	if(m_hConsole)
	{
		if(m_bAttrs)SetConsoleTextAttribute(m_hConsole,Attrs);
		ret=WriteConsole(m_hConsole,lpString,_tcslen(lpString),NULL,NULL);
		if(m_bAttrs)SetConsoleTextAttribute(m_hConsole,m_OldColorAttrs);
	}
	return ret; 
}

ConsoleWindow  ConWindow;

#define MAX_BUF_LEN 4096

BOOL DBWindowTile(LPCTSTR tile)
{
	return ConWindow.SetTile(tile);
}

BOOL DBWindowWrite(LPCTSTR fmt,...)
{
	TCHAR   message[MAX_BUF_LEN];
	va_list cur_arg;
	va_start(cur_arg,fmt);
	_vsntprintf(message,MAX_BUF_LEN,fmt,cur_arg);
	va_end(cur_arg);
	return ConWindow.WriteString(message);
}

BOOL DBWindowWrite(WORD Attrs,LPCTSTR fmt,...)
{
	TCHAR   message[MAX_BUF_LEN];
	va_list cur_arg;
	va_start(cur_arg,fmt);
	_vsntprintf(message,MAX_BUF_LEN,fmt,cur_arg);
	va_end(cur_arg);
	return ConWindow.WriteString(Attrs,message);
}

//#endif