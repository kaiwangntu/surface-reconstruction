#pragma once
#ifndef CRENDERTEXT_H
#define  CRENDERTEXT_H
class CRenderText
{
public:
	CRenderText(void);
	~CRenderText(void);

	GLvoid BuildFont(HDC hDC);// Build Our Bitmap Font
	GLvoid KillFont(GLvoid);// Delete The Font List
	GLvoid SetTextPos(GLint* viewport);
	GLvoid glPrint(const char *fmt, ...);// Custom GL "Print" Routine

private:
	GLuint	base;
};
#endif
