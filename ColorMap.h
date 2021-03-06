// ============================================================
// used to generate color map
// Written by Mingdong
// 2017/05/07
// ============================================================
#pragma once
#include <QGLWidget>
// struct of the color
struct MYGLColor 
{
	GLubyte _rgb[3];	// 0~255
public:
	MYGLColor();
	MYGLColor(GLubyte r, GLubyte g, GLubyte b);
};
class ColorMap
{
public:
	static ColorMap* GetInstance();
private:
	static ColorMap* _pInstance;
private:
	ColorMap();
public:
	// get a color given an value
	MYGLColor GetColor(double fValue);
	~ColorMap();
	int GetLength(){ return _nLen; }
	int GetStep(){ return _nStep; }
private:
	MYGLColor* _pColors;		// list of color
	double* _pValues;		// list of value
	int _nLen;				// length of color mapping
	int _nStep;
private:
	MYGLColor interpolateColor(MYGLColor color0, MYGLColor color1, double fBias0, double fBias1);
};

