#include "EnsembleLayer.h"
#include "MeteModel.h"

#include "ColorMap.h"
#include "def.h"

// for tess

/// ��ʼ���û��Զ���ĳ������

/** ���ƹ��� */
GLdouble TessProperty[5] =
{
	GLU_TESS_WINDING_ODD,        /**< ������Ϊ���� */
	GLU_TESS_WINDING_NONZERO,    /**< ������Ϊ��0 */
	GLU_TESS_WINDING_POSITIVE,   /**< ������Ϊ���� */
	GLU_TESS_WINDING_NEGATIVE,   /**< ������Ϊ���� */
	GLU_TESS_WINDING_ABS_GEQ_TWO /**< ����������ֵ���ڵ���2 */
};
GLuint nProperty = 0;              /**< ���ƹ������� */

/** gluTessCallbackע��Ļص����� */
void CALLBACK beginCallback(GLenum which)
{
	glBegin(which);
}

void CALLBACK errorCallback(GLenum errorCode)
{
	const GLubyte *string;
	///���������Ϣ
	string = gluErrorString(errorCode);
	// 	fprintf(stderr, "Tessellation Error: %s\n", string);
	// 	exit(0);
}

void CALLBACK endCallback(void)
{
	glEnd();
}

void CALLBACK vertexCallback(GLvoid *vertex)
{

	GLdouble* pt;
	GLubyte red, green, blue;
	pt = (GLdouble*)vertex;
	/** �漴������ɫֵ */
	red = (GLubyte)rand() & 0xff;
	green = (GLubyte)rand() & 0xff;
	blue = (GLubyte)rand() & 0xff;
	glColor3ub(red, green, blue);
	glVertex3dv(pt);

}

/** ���ڴ����������߽��㣬�������Ƿ�ϲ����㣬
�´����Ķ�����������4�����ж���������ϣ���Щ��������洢��data��
����weightΪȨ�أ�weight[i]���ܺ�Ϊ1.0
*/
void CALLBACK combineCallback(GLdouble coords[3],
	GLdouble *vertex_data[4],
	GLfloat weight[4],
	GLdouble **dataOut)
{
	GLdouble *vertex;
	int i;
	/** ����洢�¶�����ڴ� */
	vertex = (GLdouble *)malloc(6 * sizeof(GLdouble));

	/** �洢����ֵ */
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];

	/** ͨ����ֵ����RGB��ɫֵ */
	for (i = 3; i < 6; i++)
		vertex[i] = weight[0] * vertex_data[0][i] +
		weight[1] * vertex_data[1][i] +
		weight[2] * vertex_data[2][i] +
		weight[3] * vertex_data[3][i];
	*dataOut = vertex;
}
// ~for tess
EnsembleLayer::EnsembleLayer() :_dataTexture(NULL)
{
}

EnsembleLayer::~EnsembleLayer()
{
	if (_dataTexture)
	{
		delete[]_dataTexture;
	}
	if (_colorbarTexture)
	{
		delete[]_colorbarTexture;
	}
	
}

void EnsembleLayer::draw(DisplayStates states){
	// border
	glBegin(GL_LINE_LOOP);
	glVertex2f(_fLeft, _fBottom);
	glVertex2f(_fRight, _fBottom);
	glVertex2f(_fRight, _fTop);
	glVertex2f(_fLeft, _fTop);
	glEnd();


	//*
	double g_arrColors[9][3] = {
		{ 1, 0, 0 },			// R
		{ 0, 1, 0 },			// G
		{ 0, 0, 1 },			// B
		{ 1, 1, 0 },			// R
		{ 0, 1, 1 },			// G
		{ 1, 0, 1 },			// B
		{ 1, .5, 0 },			// R
		{ 0, 1, .5 },			// G
		{ .5, 0, 1 },			// B
	};
	//*/
	/*
	double g_arrColors[9][3] = {
		{ 165	/255.0, 206	/255.0, 227	 /255.0},
		{ 81	/255.0, 154	/255.0, 165	 /255.0},
		{ 86	/255.0, 177	/255.0, 70	 /255.0},
		{ 249	/255.0, 141	/255.0, 141	 /255.0},
		{ 239	/255.0, 105	/255.0, 68	 /255.0},
		{ 255	/255.0, 135	/255.0, 16	 /255.0},
		{ 178	/255.0, 149	/255.0, 200	 /255.0},
		{ 198	/255.0, 180	/255.0, 153	 /255.0},
		{ 176	/255.0, 88	/255.0, 40	 /255.0}
	};
	//*/
	

	/*
	double fFocusL = _fLeft + (_fRight - _fLeft)*_pModel->GetFocusX() / _pModel->GetW();
	double fFocusR = _fLeft + (_fRight - _fLeft)*(_pModel->GetFocusX()+_pModel->GetFocusW()) / _pModel->GetW();
	double fFocusB = _fBottom + (_fTop - _fBottom)*_pModel->GetFocusY() / _pModel->GetH();
	double fFocusT = _fBottom + (_fTop - _fBottom)*(_pModel->GetFocusY()+_pModel->GetFocusH()) / _pModel->GetH();
	*/

	/*
	double fFocusL = _fLeft + (_fRight - _fLeft)*(_pModel->GetWest() + 180) / 360;
	double fFocusR = _fLeft + (_fRight - _fLeft)*(_pModel->GetEast() + 180) / 360;
	double fFocusB = _fBottom + (_fTop - _fBottom)*(_pModel->GetSouth() + 90) / 180;
	double fFocusT = _fBottom + (_fTop - _fBottom)*(_pModel->GetNorth() + 90) / 180;
	*/

	int nClusterIndex = -1;	// -1 means show all clusters

	double biasX = (_pModel->GetWest() + _pModel->GetEast()) / 2.0 *_fScaleW;
	double biasY = (_pModel->GetSouth() + _pModel->GetNorth()) / 2.0*_fScaleH;

	double biasFocusX = (_pModel->GetFocusWest() + _pModel->GetFocusEast()) / 2.0 *_fScaleW;
	double biasFocusY = (_pModel->GetFocusSouth() + _pModel->GetFocusNorth()) / 2.0*_fScaleH;

	double scaleFocusX = (_pModel->GetFocusEast() - _pModel->GetFocusWest()) / 360.0;
	double scaleFocusY = (_pModel->GetFocusNorth() - _pModel->GetFocusSouth()) / 180.0;

	glPushMatrix();
	// draw background
	if (states._bShowBackground){
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glBindTexture(GL_TEXTURE_2D, texID[0]);


		glTranslatef(biasFocusX, biasFocusY, 0);			// λ��ƫ��
		glScalef(scaleFocusX, scaleFocusY, 0);			// �ı�ߴ�

		glBegin(GL_QUADS);
		/*
		glTexCoord2f(0.0f, 0.0f); glVertex2f(fFocusL, fFocusB);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(fFocusR, fFocusB);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(fFocusR, fFocusT);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(fFocusL, fFocusT);
		*/

		glTexCoord2f(0.0f, 0.0f); glVertex2f(_fLeft, _fBottom);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(_fRight, _fBottom);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(_fRight, _fTop);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(_fLeft, _fTop);

		glEnd();

		glPopMatrix();

		// color bar
		glBindTexture(GL_TEXTURE_2D, texID[1]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(_fRight+.03, _fBottom);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(_fRight+.06, _fBottom);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(_fRight+.06, _fTop);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(_fRight+.03, _fTop);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		// border

		glBegin(GL_LINE_LOOP);
		glVertex2f(_fRight + .03, _fBottom);
		glVertex2f(_fRight + .06, _fBottom);
		glVertex2f(_fRight + .06, _fTop);
		glVertex2f(_fRight + .03, _fTop);
		glEnd();
		ColorMap* colormap = ColorMap::GetInstance();
		int nLen = colormap->GetLength();
		int nStep = colormap->GetStep();
		// scale
		for (int i = 1; i < nLen; i++)
		{
			glBegin(GL_LINES);
			glVertex2f(_fRight + .06, _fBottom + (_fTop - _fBottom)*i / (nLen-1));
			glVertex2f(_fRight + .05, _fBottom + (_fTop - _fBottom)*i / (nLen-1));
			glEnd();
			// draw text

			char buf[10];
			sprintf_s(buf, "%d", i*nStep);
// 			font.PrintText(buf, _fRight + .06, _fBottom + (_fTop - _fBottom)*i / 8);
			_pCB->DrawText(buf, _fRight + .063, _fBottom + (_fTop - _fBottom)*i / (nLen-1));
		}
	}
	glPushMatrix();

	glTranslatef(biasX, biasY, 0);				// �ƶ���ָ��λ��

//	if (!_pModel->GetFilter())
//		glScalef(.5, .5, 1);						// �任��С

	glTranslatef(-(_pModel->GetW()-1)*_fScaleW/2, -(_pModel->GetH() - 1)*_fScaleH / 2, 0);		// �ƶ����м�
	// draw contour line
	if (states._bShowContourLineMin)
	{
		if (g_bClustering)
		{
			if (nClusterIndex>-1)	// show single cluster
			{
				glColor4f(.5, .5, 0.0, 1.0);
				drawContourLine(_pModel->GetContourMin(nClusterIndex));

			}
			else {

				double fOpacity = 1.0;
				for (size_t i = 0, length = _pModel->GetClusters(); i < length; i++)
				{
					glColor4f(g_arrColors[i][0], g_arrColors[i][1], g_arrColors[i][2], fOpacity);
					drawContourLine(_pModel->GetContourMin(i));
				}
			}
		}
		else {
			glColor4f(1, 1, 0.0, 1.0);
			drawContourLine(_pModel->GetContourMin());
		}
	}
	if (states._bShowContourLineMax){
		if (g_bClustering)
		{
			if (nClusterIndex>-1)	// show single cluster
			{
				int i = 0;
				glColor4f(0.0, 1.0, 1.0, 1.0);
				drawContourLine(_pModel->GetContourMax(nClusterIndex));
			}
			else {
				double fOpacity = 1.0;
				for (size_t i = 0, length = _pModel->GetClusters(); i < length; i++)
				{
					glColor4f(g_arrColors[i][0], g_arrColors[i][1], g_arrColors[i][2], fOpacity);
					drawContourLine(_pModel->GetContourMax(i));
				}

			}
		}
		else {
			glColor4f(0.0, 1.0, 1.0, 1.0);
			drawContourLine(_pModel->GetContourMax());
		}
	}
	if (states._bShowContourLineMean)
	{
		if (g_bClustering)
		{
			double fOpacity = 1.0;
			for (size_t i = 0,length=_pModel->GetClusters(); i < length; i++)
			{
				if (false) { // show two kinds of means
					glColor4f(g_arrColors[i][0], g_arrColors[i][1], g_arrColors[i][2], fOpacity);
					drawContourLine(_pModel->GetContourMeanPCA(i));
					glColor4f(g_arrColors[i][0], g_arrColors[i][1], g_arrColors[i][2], fOpacity / 2);
					drawContourLine(_pModel->GetContourMean(i));
				}
				else {
					glColor4f(g_arrColors[i][0], g_arrColors[i][1], g_arrColors[i][2], fOpacity);
					drawContourLine(_pModel->GetContourMean(i));
				}
			}			
		}
		else {
			glColor4f(1.0, 0.0, 1.0, .5);
			drawContourLine(_pModel->GetContourMean());
		}
	}
	// show contour line
	if (states._bShowContourLine)
	{
		if (g_bClustering)
		{
			if (false)		// show first one
			{
				QList<QList<ContourLine>> contours = _pModel->GetContour();
				glColor4f(1.0, 0.0, 0.0, 1.0);
				drawContourLine(contours[0]);
			}
			else {
				float fOpacity = .5;
				for (size_t i = 0, length = _pModel->GetClusters(); i < length; i++)
				{
					glColor4f(g_arrColors[i][0], g_arrColors[i][1], g_arrColors[i][2], fOpacity);
					QList<QList<ContourLine>> contours = _pModel->GetContour(i);
					for (int j = 0; j < contours.size(); j++)
					{
						drawContourLine(contours[j]);
					}
				}
			}
			/*
			QList<QList<ContourLine>>* contours = _pModel->GetContour();
			glColor4f(1.0, 0.0, 0.0, .1);
			for (int i = 0; i < contours[1].size(); i++)
			{
				drawContourLine(contours[1][i]);
			}
			glColor4f(0.0, 1.0, 0.0, .1);
			for (int i = 0; i < contours[2].size(); i++)
			{
				drawContourLine(contours[2][i]);
			}
			*/
		}
		else {
			QList<QList<ContourLine>> contours = _pModel->GetContour();
			glColor4f(1.0, 0.0, 0.0, 1.0);
			for (int i = 0; i < contours.size(); i++)
			{
				drawContourLine(contours[i]);
			}
		}
	}
	// union
	if (states._bShowUnionE)
	{

		double fTransparency = .2;
		if (g_bClustering) {
			if (nClusterIndex>-1)
			{
				// show single cluster
				for (size_t j = 0; j < 3; j++)
				{
					glColor4f(g_arrColors[j][0], g_arrColors[j][1], g_arrColors[j][2], fTransparency);
					glCallList(_gllist + (nClusterIndex + 1) * 3 + j);
				}
			}
			else {
				for (size_t i = 0, length = _pModel->GetClusters(); i < length; i++)
				{
					glColor4f(g_arrColors[i][0], g_arrColors[i][1], g_arrColors[i][2], fTransparency);
					glCallList(_gllist + (i + 1) * 3 + 1);
				}

			}
		}
		else {
			// render each area
			if (g_bShowUncertaintyOnly) {

				glColor4f(1, 1, 1, .5);
				glCallList(_gllist + 1);
			}
			else
			for (size_t j = 0; j < 3; j++)
			{
				glColor4f(g_arrColors[j][0], g_arrColors[j][1], g_arrColors[j][2], fTransparency);
				glCallList(_gllist + j);
			}
		}
	}

	glPopMatrix();

}

void EnsembleLayer::ReloadTexture() {
	// Enable texturing
	// 	generateBackground();
	// 	glEnable(GL_TEXTURE_2D);
//	_dataTexture = _pModel->generateTextureMean();
	_dataTexture = _pModel->generateTextureGridCluster();
	//	_dataTexture = _pModel->generateTextureRange(0);
	//	_dataTexture = _pModel->generateTextureSDF();
	glGenTextures(1, &texID[0]);
	glBindTexture(GL_TEXTURE_2D, texID[0]);
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _pModel->GetFocusW(), _pModel->GetFocusH(), 0, GL_RGBA, GL_UNSIGNED_BYTE, _dataTexture);

}
void EnsembleLayer::init(){
	ReloadTexture();
	// color bar
	ColorMap* colormap=ColorMap::GetInstance();
	int nLen = colormap->GetLength();
	int nStep = colormap->GetStep();
	int nColorBarW = 1;
	int nColorBarH = (nLen-1)*10+1;
	_colorbarTexture = new GLubyte[nColorBarH * 4];
	for (int i = 0; i < nColorBarH; i++)
	{
		MYGLColor color = colormap->GetColor(i/10.0*nStep);
		// using transparency and the blue tunnel
		_colorbarTexture[4 * i + 0] = color._rgb[0];
		_colorbarTexture[4 * i + 1] = color._rgb[1];
		_colorbarTexture[4 * i + 2] = color._rgb[2];
		_colorbarTexture[4 * i + 3] = (GLubyte)255;
	}
	glGenTextures(1, &texID[1]);
	glBindTexture(GL_TEXTURE_2D, texID[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nColorBarW, nColorBarH, 0, GL_RGBA, GL_UNSIGNED_BYTE, _colorbarTexture);
	// ~build texture


	// initialize tess
	_gllist = glGenLists((_pModel->GetClusters()+1)*3);	// generate the display lists

	_tobj = gluNewTess();
	gluTessCallback(_tobj, GLU_TESS_VERTEX,
		(void(__stdcall*)())glVertex3dv);
	gluTessCallback(_tobj, GLU_TESS_BEGIN,
		(void(__stdcall*)())beginCallback);
	gluTessCallback(_tobj, GLU_TESS_END,
		(void(__stdcall*)())endCallback);
	gluTessCallback(_tobj, GLU_TESS_ERROR,
		(void(__stdcall*)())errorCallback);



	tessSegmentation(_gllist, _pModel->GetUncertaintyArea());
	if (g_bClustering)
	{
		for (size_t i = 0, length = _pModel->GetClusters(); i < length; i++)
		{
			tessSegmentation(_gllist + (i + 1) * 3, _pModel->GetUncertaintyArea(i));
		}
	}
}

void EnsembleLayer::drawContourLine(const QList<ContourLine>& contours){

	for each (ContourLine contour in contours)
	{
		glBegin(GL_LINE_STRIP);
		for each (QPointF pt in contour._listPt)
		{
			double x = pt.x() * _fScaleW;
			double y = pt.y() * _fScaleH;
			glVertex2f(x, y);
		}
		glEnd();
	}
}

void EnsembleLayer::tessSegmentation(GLuint gllist, QList<UnCertaintyArea*> areas){
	// create buffer for the uncertainty areas	
	int nAreaLen = areas.size();										// how many area
	GLdouble**** contourBuffer = new GLdouble***[areas.size()];
	// for each area
	for (int i = 0; i < nAreaLen; i++)
	{
		int nContourLen = areas[i]->_listEmbeddedArea.size() + 1;		// how many contour
		contourBuffer[i] = new GLdouble**[nContourLen];
		// first contour
		int nLen = areas[i]->_contour._listPt.size();
		contourBuffer[i][0] = new GLdouble*[nLen];
		for (int j = 0; j < nLen; j++)
		{
			contourBuffer[i][0][j] = new GLdouble[3];
			contourBuffer[i][0][j][0] = areas[i]->_contour._listPt[j].x()*_fScaleW;
			contourBuffer[i][0][j][1] = areas[i]->_contour._listPt[j].y()*_fScaleH;
			contourBuffer[i][0][j][2] = 0;
		}
		// other contours
		for (int k = 0; k < nContourLen - 1; k++)
		{
			int nLen = areas[i]->_listEmbeddedArea[k]->_contour._listPt.size();
			contourBuffer[i][k + 1] = new GLdouble*[nLen];
			for (int j = 0; j < nLen; j++)
			{
				contourBuffer[i][k + 1][j] = new GLdouble[3];
				contourBuffer[i][k + 1][j][0] = areas[i]->_listEmbeddedArea[k]->_contour._listPt[j].x()*_fScaleW;
				contourBuffer[i][k + 1][j][1] = areas[i]->_listEmbeddedArea[k]->_contour._listPt[j].y()*_fScaleH;
				contourBuffer[i][k + 1][j][2] = 0;
			}
		}
	}
	for (int state = -1; state < 2; state++)
	{
		glNewList(gllist + 1 + state, GL_COMPILE);
		gluTessProperty(_tobj, GLU_TESS_WINDING_RULE, TessProperty[nProperty]);
		gluTessBeginPolygon(_tobj, NULL);
		for (int i = 0; i < nAreaLen; i++)
		{
			// 		if (i != 1) continue;
			if (areas[i]->_nState == state)
			{
				int nContourLen = areas[i]->_listEmbeddedArea.size() + 1;
				for (int k = 0; k < nContourLen; k++)
				{
					int nLen;
					if (k == 0) nLen = areas[i]->_contour._listPt.size();
					else nLen = areas[i]->_listEmbeddedArea[k - 1]->_contour._listPt.size();
					gluTessBeginContour(_tobj);
					for (int j = 0; j < nLen; j++)
					{
						gluTessVertex(_tobj, contourBuffer[i][k][j], contourBuffer[i][k][j]);
					}
					gluTessEndContour(_tobj);
				}
			}
		}
		gluTessEndPolygon(_tobj);
		glEndList();
	}

	for (int i = 0; i < nAreaLen; i++)
	{
		int nContourLen = areas[i]->_listEmbeddedArea.size() + 1;		// how many contour
		// first contour
		int nLen = areas[i]->_contour._listPt.size();
		for (int j = 0; j < nLen; j++)
		{
			delete contourBuffer[i][0][j];
		}
		for (int k = 0; k < nContourLen - 1; k++)
		{
			int nLen = areas[i]->_listEmbeddedArea[k]->_contour._listPt.size();
			for (int j = 0; j < nLen; j++)
			{
				delete[]contourBuffer[i][k + 1][j];
			}
			delete[]contourBuffer[i][k + 1];
		}
		delete[]contourBuffer[i];
	}
	delete[]contourBuffer;
}