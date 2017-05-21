/*
	ensemble result display layer
	Mingdong
	2017/05/05
*/
#pragma once
#include "MeteLayer.h"
#include "ContourGenerator.h"
#include <QGLWidget>
#include <gl/GLU.h>
#include "GLFont.h"
class UnCertaintyArea;
class EnsembleLayer :
	public MeteLayer
{
public:
	EnsembleLayer();
	virtual ~EnsembleLayer();
protected:
	virtual void draw(DisplayStates states);
	virtual void init();
private:
	// draw a contour line
	void drawContourLine(const QList<ContourLine>& contours);

	// tessellation the area segmentation, generate three display list start from _gllist
	void tessSegmentation(GLuint gllist, QList<UnCertaintyArea*> areas);
private:
	// truth texture, generated from truth data
	GLubyte* _dataTexture;
	// texture of the color bar, 160*2
	GLubyte* _colorbarTexture;
	// texture id,1-data,2-colorbar
	GLuint texID[2];

	GLUtesselator *_tobj;                    /**< 网格化对象指针 */

// 	GLFont font;                /**< 定义一个字体实例 */


};

