#include "ColorMap.h"


MYGLColor::MYGLColor(){
	_rgb[0] = _rgb[1] = _rgb[2] = 0;
}

MYGLColor::MYGLColor(GLubyte r, GLubyte g, GLubyte b){
	_rgb[0] = r;
	_rgb[1] = g;
	_rgb[2] = b;
}


ColorMap* ColorMap::GetInstance(){
	if (!_pInstance)
	{
		_pInstance = new ColorMap();
	}
	return _pInstance;
}
ColorMap* ColorMap::_pInstance = NULL;

ColorMap::ColorMap()
{
	_nLen = 9;
	_nStep = 2;					// for mean
// 	_nStep = 7;					// for variance
	_pValues = new double[_nLen];
	_pColors = new MYGLColor[_nLen];

// 	_pValues[0] = 0;
// 	_pValues[1] = 2;
// 	_pValues[2] = 5;
// 	_pValues[3] = 10;
// 	_pColors[0] = MYGLColor(255,255,255);
// 	_pColors[1] = MYGLColor(0, 0, 255);
// 	_pColors[2] = MYGLColor(0, 255, 0);
// 	_pColors[3] = MYGLColor(255, 0, 0);

	for (int i = 0; i < _nLen; i++)
	{
		_pValues[i] = i * _nStep;
	}
	_pColors[0] = MYGLColor(165, 206, 227);
	_pColors[1] = MYGLColor(81, 154, 165);
	_pColors[2] = MYGLColor(86, 177, 70);
	_pColors[3] = MYGLColor(249, 141, 141);
	_pColors[4] = MYGLColor(239, 105, 68);
	_pColors[5] = MYGLColor(255, 135, 16);
	_pColors[6] = MYGLColor(178, 149, 200);
	_pColors[7] = MYGLColor(198, 180, 153);
	_pColors[8] = MYGLColor(176, 88, 40);


}


ColorMap::~ColorMap()
{
	delete _pColors;
	delete _pValues;
	_pInstance = NULL;
}

MYGLColor ColorMap::GetColor(double fValue){
	int i = 1;
	while (fValue > _pValues[i] && i < _nLen) i++;


	if (i == _nLen) return _pColors[_nLen - 1];
	else{
		MYGLColor color0 = _pColors[i - 1];
		MYGLColor color1 = _pColors[i];
		return interpolateColor(color0, color1, fValue - _pValues[i - 1], _pValues[i] - fValue);
	}
}
MYGLColor ColorMap::interpolateColor(MYGLColor color0, MYGLColor color1, double fBias0, double fBias1){
	MYGLColor result;
	for (int i = 0; i < 3;i++)
	{
		result._rgb[i] = (color0._rgb[i] * fBias1 + color1._rgb[i] * fBias0) / (fBias0 + fBias1);
	}
	return result;
}