#ifndef _DBWINDOW_
#define _DBWINDOW_

//#include <windows.h>

//����̨���ʱ���ı���ɫ
#define WDS_T_RED    FOREGROUND_RED
#define WDS_T_GREEN  FOREGROUND_GREEN
#define WDS_T_BLUE  FOREGROUND_BLUE
//����̨���ʱ���ı�������ɫ
#define WDS_BG_RED  BACKGROUND_RED
#define WDS_BG_GREEN BACKGROUND_GREEN
#define WDS_BG_BLUE  BACKGROUND_BLUE

//#ifdef _DEBUG

//���ÿ���̨������ڱ���
BOOL DBWindowTile(LPCTSTR tile);
//��ʽ���ı����
BOOL DBWindowWrite(LPCTSTR fmt,...);
//����ɫ��ʽ���ı����
BOOL DBWindowWrite(WORD Attrs,LPCTSTR fmt,...);

//#else

//#define DBWindowTile
//#define DBWindowWrite

//#endif

#endif