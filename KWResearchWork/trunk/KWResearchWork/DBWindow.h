#ifndef _DBWINDOW_
#define _DBWINDOW_

//#include <windows.h>

//控制台输出时的文本颜色
#define WDS_T_RED    FOREGROUND_RED
#define WDS_T_GREEN  FOREGROUND_GREEN
#define WDS_T_BLUE  FOREGROUND_BLUE
//控制台输出时的文本背景颜色
#define WDS_BG_RED  BACKGROUND_RED
#define WDS_BG_GREEN BACKGROUND_GREEN
#define WDS_BG_BLUE  BACKGROUND_BLUE

//#ifdef _DEBUG

//设置控制台输出窗口标题
BOOL DBWindowTile(LPCTSTR tile);
//格式化文本输出
BOOL DBWindowWrite(LPCTSTR fmt,...);
//带颜色格式化文本输出
BOOL DBWindowWrite(WORD Attrs,LPCTSTR fmt,...);

//#else

//#define DBWindowTile
//#define DBWindowWrite

//#endif

#endif