#include "StdAfx.h"
#include "RenderText.h"

CRenderText::CRenderText(void)
{
}

CRenderText::~CRenderText(void)
{
}

GLvoid CRenderText::BuildFont(HDC hDC)// Build Our Bitmap Font
{
	HFONT	font;// Windows Font ID
	HFONT	oldfont;// Used For Good House Keeping

	this->base = glGenLists(96);								// Storage For 96 Characters

	font = CreateFont(	-24,							// Height Of Font
		0,								// Width Of Font
		0,								// Angle Of Escapement
		0,								// Orientation Angle
		FW_BOLD,						// Font Weight
		FALSE,							// Italic
		FALSE,							// Underline
		FALSE,							// Strikeout
		ANSI_CHARSET,					// Character Set Identifier
		OUT_TT_PRECIS,					// Output Precision
		CLIP_DEFAULT_PRECIS,			// Clipping Precision
		ANTIALIASED_QUALITY,			// Output Quality
		FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
		"Courier New");					// Font Name

	oldfont = (HFONT)SelectObject(hDC, font);           // Selects The Font We Want
	wglUseFontBitmaps(hDC, 32, 96, base);				// Builds 96 Characters Starting At Character 32
	SelectObject(hDC, oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font
}

GLvoid CRenderText::KillFont(GLvoid)// Delete The Font List
{
	glDeleteLists(base, 96);// Delete All 96 Characters
}

GLvoid CRenderText::glPrint(const char *fmt, ...)// Custom GL "Print" Routine
{
	char		text[256];// Holds Our String
	va_list		ap;	// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base - 32);								// Sets The Base Character to 32
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits

}

GLvoid CRenderText::SetTextPos(GLint* viewport)
{
//	glColor3f(1,1,0);
	
	glDisable(GL_LIGHTING);

	glColor3f(1,0,0);

	glPushAttrib(GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);

	float x=0;
//	float y=0;
	float y=viewport[3]-20;
	float z=0;
	float w=1;
	/* Setup projection parameters. */
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDepthRange(z, z);
	glViewport((int) x - 1, (int) y - 1, 2, 2);
	/* Set the raster (window) position. */
	float fx = x - (int) x;
	float fy = y - (int) y;
	glRasterPos4f(fx, fy, 0.0, w);
	/* Restore matrices, viewport and matrix mode. */
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();

	glEnable(GL_LIGHTING);

}
