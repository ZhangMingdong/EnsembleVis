#include "MeteModel.h"

#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>

#include "ContourGenerator.h"
#include "ColorMap.h"

#include "AHCClustering.h"
#include "MyPCA.h"
#include "DataField.h"

#include <fstream>
#include <iomanip>
#include <assert.h>
#include <sstream>

#include "def.h"

using namespace std;



/*
double PointToSegDist(double x, double y, double x1, double y1, double x2, double y2)
{
	double cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
	if (cross <= 0) return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));

	double d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	if (cross >= d2) return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));

	double r = cross / d2;
	double px = x1 + (x2 - x1) * r;
	double py = y1 + (y2 - y1) * r;
	return sqrt((x - px) * (x - px) + (py - y) * (py - y));
}
*/
double PointToSegDist(double x, double y, double x1, double y1, double x2, double y2)
{
	double cross = (x2 - x1) * (x - x1) + (y2 - y1) * (y - y1);
	if (cross <= 0)
		return sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));

	double d2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	if (cross >= d2)
		return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));

	double r = cross / d2;
	double px = x1 + (x2 - x1) * r;
	double py = y1 + (y2 - y1) * r;
	return sqrt((x - px) * (x - px) + (y - py) * (y - py));
}

MeteModel::MeteModel() :_pData(0)
,_pSDF(0)
, _arrLabels(0)
/*
, _gridSDF(0)
, _gridVar(0)
, _gridMean(0)
, _gridUnionMax(0)
, _gridUnionMin(0)
*/
, _pClusterCenter(0)
{
}

MeteModel::~MeteModel()
{
	if (_pData)
	{
		delete _pData;
	}
	if (_pSDF) {
		delete _pSDF;
	}
	/*
	if (_gridSDF)
	{
		delete[] _gridSDF;
	}
	
	if (_gridVar)
	{
		delete[] _gridVar;
	}

	if (_gridMean)
	{
		delete[] _gridMean;
	}
	if (_gridUnionMax)
	{
		delete[] _gridUnionMax;
	}
	if (_gridUnionMin)
	{
		delete[] _gridUnionMin;
	}
	*/

	for each (UnCertaintyArea* pArea in _listUnionAreaE)
		delete pArea;

	for each (DataField* pData in _listClusterData)
	{
		delete pData;
	}
	/*
	for (size_t i = 0; i < _gridMeanC.length(); i++)
	{
		if (_gridMeanC[i])
		{
			delete[] _gridMeanC[i];
		}
		if (_gridVarC[i])
		{
			delete[] _gridVarC[i];
		}
		if (_gridUnionMaxC[i])
		{
			delete[] _gridUnionMaxC[i];
		}
		if (_gridUnionMinC[i])
		{
			delete[] _gridUnionMinC[i];
		}

	}
	*/
	for (size_t i = 0; i < _listUnionAreaEC.length(); i++)
	{
		for each (UnCertaintyArea* pArea in _listUnionAreaEC[i])
			delete pArea;

	}
	if (_arrLabels)
	{
		delete[] _arrLabels;
	}
	if (_pClusterCenter)
	{
		delete[] _pClusterCenter;
	}
}

void MeteModel::InitModel(int nEnsembleLen, int nWidth, int nHeight, int nFocusX, int nFocusY, int nFocusW, int nFocusH
	, QString strFile, bool bBinary, int nWest, int nEast, int nSouth, int nNorth
	, int nFocusWest, int nFocusEast, int nFocusSouth, int nFocusNorth,bool bFilter) {
	// 0.record states variables
	_nClusters = 3;

	_nEnsembleLen = nEnsembleLen;
	_nWidth = nWidth;
	_nHeight = nHeight;
	_nLen = nHeight*nWidth;

	_nFocusX = nFocusX;
	_nFocusY = nFocusY;
	_nFocusW = nFocusW;
	_nFocusH = nFocusH;

	_nFocusLen = _nFocusW*_nFocusH;

	_nWest = nWest;
	_nEast = nEast;
	_nSouth = nSouth;
	_nNorth = nNorth;
	_nFocusWest = nFocusWest;
	_nFocusEast = nFocusEast;
	_nFocusSouth = nFocusSouth;
	_nFocusNorth = nFocusNorth;

	_strFile = strFile;
	_bBinaryFile = bBinary;
	_bFilter = bFilter;

	// 2.allocate resource

	_pData = new DataField(_nWidth, _nHeight, _nEnsembleLen);// new double[_nLen*_nEnsembleLen];
	_pSDF = new DataField(_nWidth, _nHeight, _nEnsembleLen);// new double[_nLen*_nEnsembleLen];
//	_gridSDF = new double[_nFocusLen*_nEnsembleLen];
	_arrLabels = new int[_nEnsembleLen];

	// 3.read data
	if (_bBinaryFile)
	{
		readData();
	}
	else {
		readDataFromText();
	}

	// 4.statistic
	_pData->DoStatistic();

	// 5.generate features
	ContourGenerator generator;
	generator.Generate(_pData->GetMean(), _listContourMeanE, g_fThreshold, _nWidth, _nHeight, _nFocusX, _nFocusY, _nFocusW, _nFocusH);
	generator.Generate(_pData->GetUMin(), _listContourMinE, g_fThreshold, _nWidth, _nHeight, _nFocusX, _nFocusY, _nFocusW, _nFocusH);
	generator.Generate(_pData->GetUMax(), _listContourMaxE, g_fThreshold, _nWidth, _nHeight, _nFocusX, _nFocusY, _nFocusW, _nFocusH);
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		QList<ContourLine> contour;
		generator.Generate(_pData->GetLayer(i), contour, g_fThreshold, _nWidth, _nHeight, _nFocusX, _nFocusY, _nFocusW, _nFocusH);
		_listContour.push_back(contour);
	}
	if (g_bClustering)
	{
		// 6.PCA
		doPCA();

		bool bUsingGlobalData = false;	// not just focus the iso value
		double dbIsoValue;
		// 7.generate clustered data
		if (bUsingGlobalData)
		{
			_pData->GenerateClusteredData(_listClusterLen, _arrLabels, _listClusterData);
			dbIsoValue = g_fThreshold;
		}
		else {
			dbIsoValue = 0;
			_pSDF->GenerateClusteredData(_listClusterLen, _arrLabels, _listClusterData);
		}
		// 8.generate features for clusters
		for (size_t i = 0; i < _nClusters; i++)
		{
			QList<ContourLine> listContourMin;
			QList<ContourLine> listContourMax;
			QList<ContourLine> listContourMean;
			QList<QList<ContourLine>> listContour;
			QList<UnCertaintyArea*> listUnionArea;
			generator.Generate(_listClusterData[i]->GetMean(), listContourMean, dbIsoValue, _nWidth, _nHeight, _nFocusX, _nFocusY, _nFocusW, _nFocusH);
			generator.Generate(_listClusterData[i]->GetUMin(), listContourMin, dbIsoValue, _nWidth, _nHeight, _nFocusX, _nFocusY, _nFocusW, _nFocusH);
			generator.Generate(_listClusterData[i]->GetUMax(), listContourMax, dbIsoValue, _nWidth, _nHeight, _nFocusX, _nFocusY, _nFocusW, _nFocusH);
			_listContourMinEC.push_back(listContourMin);
			_listContourMaxEC.push_back(listContourMax);
			_listContourMeanEC.push_back(listContourMean);
			_listContourC.push_back(listContour);
			_listUnionAreaEC.push_back(listUnionArea);

			// pca
			QList<ContourLine> listContourMeanPCA;
			generator.Generate(_pClusterCenter + i*_nLen, listContourMeanPCA, 0, _nWidth, _nHeight, _nFocusX, _nFocusY, _nFocusW, _nFocusH);
			_listContourMeanPCA.push_back(listContourMeanPCA);
		}
		// put the spaghetti into different list according to their clusters
		for (size_t i = 0; i < _nEnsembleLen; i++)
		{
			_listContourC[_arrLabels[i]].push_back(_listContour[i]);
		}
		// 9.generate uncertainty areas
		for (size_t i = 0; i < _nClusters; i++)
		{
			if (_listClusterLen[i]>1)
			{
				generateContourImp(_listContourMinEC[i], _listContourMaxEC[i], _listUnionAreaEC[i]);
			}
		}
	}

	generateContourImp(_listContourMinE, _listContourMaxE, _listUnionAreaE);
}

void MeteModel::readDataFromText() {
	int nTimeStep = 4;

	QFile file(_strFile);

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(0, "error", file.errorString());
	}

	QTextStream in(&file);
	while (!in.atEnd()) {
		for (size_t t = 0; t <= nTimeStep; t++)
		{
			for (int i = 0; i < _nEnsembleLen; i++)
			{
				QString line = in.readLine();
				for (int j = 0; j < _nLen; j++)
				{
					QString line = in.readLine();
					if (t == nTimeStep)
						_pData->SetData(i, j, line.toFloat());
				}
			}
		}
			break;

	}

	file.close();
}

void MeteModel::doPCA() {
	int nPCALen = _nEnsembleLen - 1;
	// 0.calculate the sdf
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		calculateSDF(_pData->GetLayer(i), _pSDF->GetEditableLayer(i), _nWidth, _nHeight, g_fThreshold, _listContour[i]);
	}
	_pSDF->DoStatistic();
	// 1.pca
	MyPCA pca;
	double* arrData = new double[_nEnsembleLen*nPCALen];
	pca.DoPCA(_pSDF->GetData(), arrData, _nEnsembleLen, _nLen, true);
//	pca.DoPCA(_pSDF->GetData(), arrData, _nEnsembleLen, _nLen);

	// try to recover the data, used to test DoPCA and Recover
//	pca.Recover(arrData, _gridSDF, _nEnsembleLen);
//	pca.Recover(arrData, _gridSDF, 1);

	// 2.clustering
	AHCClustering clustering;
	clustering.DoCluster(_nEnsembleLen, nPCALen, _nClusters, arrData, _arrLabels);

	// 3.counting length of each cluster
	for (size_t i = 0; i < _nClusters; i++)
	{
		_listClusterLen.push_back(0);
	}
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		_listClusterLen[_arrLabels[i]]++;
	}

	// 4.calculate mean of clusters
	// mean of each cluster
	double* _arrMeanC = new double[nPCALen*_nClusters];
	for (size_t i = 0; i < nPCALen*_nClusters; i++)
	{
		_arrMeanC[i] = 0;
	}
	for (size_t i = 0; i < _nEnsembleLen; i++)
	{
		int nCluster = _arrLabels[i];
		for (size_t j = 0; j < nPCALen; j++)
		{
			_arrMeanC[nCluster*nPCALen + j] += arrData[i*nPCALen + j];
		}
	}
	for (size_t i = 0; i < _nClusters; i++)
	{
		for (size_t j = 0; j < nPCALen; j++)
		{
			_arrMeanC[i*nPCALen + j] /= _listClusterLen[i];
		}
	}

	_pClusterCenter = new double[_nClusters*_nLen];
	// 5.project back of the cluster means
	// transfer back
	pca.Recover(_arrMeanC, _pClusterCenter, _nClusters);

	delete[] _arrMeanC;


	// 4.output labels
	if (false)
	{
		cout << "Labels:" << endl;
		for (size_t i = 0; i < _nEnsembleLen; i++)
		{
			cout << _arrLabels[i] << endl;
		}
		cout << "~Labels:" << endl;

	}
	
	delete[] arrData;
}

void MeteModel::calculateSDF(const double* arrData, double* arrSDF, int nW, int nH, double isoValue, QList<ContourLine> contour) {
	for (size_t i = 0; i < nH; i++)
	{
		for (size_t j = 0; j < nW; j++)
		{
			double dbMinDis = nW*nH;
			for (size_t k = 0,lenK=contour.length(); k < lenK; k++)
			{
				for (size_t l = 0,lenL=contour[k]._listPt.length()-1; l < lenL; l++)
				{
					double dbDis = PointToSegDist(j,i
						, contour[k]._listPt[l].x(), contour[k]._listPt[l].y()
						, contour[k]._listPt[l+1].x(), contour[k]._listPt[l+1].y());
					if (dbDis < dbMinDis)
					{
						dbMinDis = dbDis;
					}

				}
			}
			if (arrData[i*_nWidth + j] < isoValue) dbMinDis = -dbMinDis;	// sign
			arrSDF[i*_nWidth + j] = dbMinDis;
		}
	}
}

GLubyte* MeteModel::generateTexture() {
	GLubyte* _dataTexture = new GLubyte[4 * _nLen];
	// color map
	if (true)
	{
		ColorMap* colormap = ColorMap::GetInstance();

		for (int i = 0; i < _nLen; i++)
		{
			MYGLColor color = colormap->GetColor(_pData->GetMean()[i]);
			// using transparency and the blue tunnel
			_dataTexture[4 * i + 0] = color._rgb[0];
			_dataTexture[4 * i + 1] = color._rgb[1];
			_dataTexture[4 * i + 2] = color._rgb[2];
			_dataTexture[4 * i + 3] = (GLubyte)255;
		}
	}

	// color map of variance
	if (false)
	{
		ColorMap* colormap = ColorMap::GetInstance();

		for (int i = 0; i < _nLen; i++)
		{
			MYGLColor color = colormap->GetColor(_pData->GetVari()[i]);
			// using transparency and the blue tunnel
			_dataTexture[4 * i + 0] = color._rgb[0];
			_dataTexture[4 * i + 1] = color._rgb[1];
			_dataTexture[4 * i + 2] = color._rgb[2];
			_dataTexture[4 * i + 3] = (GLubyte)255;
		}
	}

	return _dataTexture;
}

GLubyte* MeteModel::generateTextureMean() {
	// old version, whole area
	if (false)
	{
		GLubyte* dataTexture = new GLubyte[4 * _nLen];

		// color map
		ColorMap* colormap = ColorMap::GetInstance();

		for (int i = 0; i < _nLen; i++)
		{
			MYGLColor color = colormap->GetColor(_pData->GetMean()[i]);
			// using transparency and the blue tunnel
			dataTexture[4 * i + 0] = color._rgb[0];
			dataTexture[4 * i + 1] = color._rgb[1];
			dataTexture[4 * i + 2] = color._rgb[2];
			dataTexture[4 * i + 3] = (GLubyte)255;
		}

		return dataTexture;
	}
	// new version, focused area
	if (true)
	{
		GLubyte* dataTexture = new GLubyte[4 * _nFocusLen];

		// color map
		ColorMap* colormap = ColorMap::GetInstance();
		for (int i = _nFocusY, iLen = _nFocusY + _nFocusH; i < iLen; i++) {
			for (int j = _nFocusX, jLen = _nFocusX + _nFocusW; j < jLen; j++) {

				int nIndex = i*_nWidth + j;
				int nIndexFocus = (i - _nFocusY)*_nFocusW + j - _nFocusX;
				MYGLColor color = colormap->GetColor(_pData->GetMean()[nIndex]);
				// using transparency and the blue tunnel
				dataTexture[4 * nIndexFocus + 0] = color._rgb[0];
				dataTexture[4 * nIndexFocus + 1] = color._rgb[1];
				dataTexture[4 * nIndexFocus + 2] = color._rgb[2];
				dataTexture[4 * nIndexFocus + 3] = (GLubyte)255;

			}
		}
		return dataTexture;
	}
}

GLubyte* MeteModel::generateTextureRange() {
	GLubyte* dataTexture = new GLubyte[4 * _nFocusLen];

	double* pMeanBias = new double[_nLen];
	for (size_t i = 0; i < _nLen; i++)
	{
		pMeanBias[i] = _pData->GetMean()[i];// -_gridMean[2][i];
	}

	double *pMean = pMeanBias;

	double fMin = 100000;
	double fMax = -100000;

	for (int i = 0; i < _nLen; i++)
	{
		double f = _pData->GetData(0, i);
		if (pMean[i] > fMax) fMax = pMean[i];
		if (pMean[i] < fMin) fMin = pMean[i];
	}
	double fRange = fMax - fMin;

	for (int i = _nFocusY, iLen = _nFocusY + _nFocusH; i < iLen; i++) {
		for (int j = _nFocusX, jLen = _nFocusX + _nFocusW; j < jLen; j++) {

			int nIndex = i*_nWidth + j;
			int nIndexFocus = (i - _nFocusY)*_nFocusW + j - _nFocusX;
			// using transparency and the blue tunnel
			dataTexture[4 * nIndexFocus + 0] = (GLubyte)0;
			dataTexture[4 * nIndexFocus + 1] = (GLubyte)0;
			dataTexture[4 * nIndexFocus + 2] = (GLubyte)((pMean[nIndex] - fMin) * 254 / fRange);
			dataTexture[4 * nIndexFocus + 3] = (GLubyte)((pMean[nIndex] - fMin) * 254 / fRange);
		}
	}

	delete[] pMeanBias;
	return dataTexture;
}

GLubyte* MeteModel::generateTextureRange(int nIndex) {
	GLubyte* dataTexture = new GLubyte[4 * _nFocusLen];

	const double *pMean = _pData->GetLayer(nIndex);

	double fMin = 100000;
	double fMax = -100000;

	for (int i = 0; i < _nLen; i++)
	{
		double f = _pData->GetData(0, i);
		if (pMean[i] > fMax) fMax = pMean[i];
		if (pMean[i] < fMin) fMin = pMean[i];
	}
	double fRange = fMax - fMin;

	for (int i = _nFocusY, iLen = _nFocusY + _nFocusH; i < iLen; i++) {
		for (int j = _nFocusX, jLen = _nFocusX + _nFocusW; j < jLen; j++) {

			int nIndex = i*_nWidth + j;
			int nIndexFocus = (i - _nFocusY)*_nFocusW + j - _nFocusX;
			// using transparency and the blue tunnel
			dataTexture[4 * nIndexFocus + 0] = (GLubyte)0;
			dataTexture[4 * nIndexFocus + 1] = (GLubyte)0;
			dataTexture[4 * nIndexFocus + 2] = (GLubyte)((pMean[nIndex] - fMin) * 254 / fRange);
			dataTexture[4 * nIndexFocus + 3] = (GLubyte)((pMean[nIndex] - fMin) * 254 / fRange);
		}
	}

	return dataTexture;
}

GLubyte* MeteModel::generateTextureSDF() {
	GLubyte* dataTexture = new GLubyte[4 * _nFocusLen];

	double fMin = 100000;
	double fMax = -100000;
	const double* pData = _pSDF->GetLayer(0);
//	double* pData = _pClusterCenter + 2 * _nLen;	// using the third cluster

	for (int i = 0; i < _nLen; i++)
	{
		if (pData[i] > fMax) fMax = pData[i];
		if (pData[i] < fMin) fMin = pData[i];
	}
	double fRange = fMax - fMin;

	for (int i = 0; i < _nFocusH; i++) {
		for (int j = 0; j < _nFocusW; j++) {

			int nIndex = (i + _nFocusY)*_nWidth + j + _nFocusX;
			int nIndexFocus = i*_nFocusW + j;
			// using transparency and the blue tunnel
			dataTexture[4 * nIndexFocus + 0] = (GLubyte)0;
			dataTexture[4 * nIndexFocus + 1] = (GLubyte)0;
			dataTexture[4 * nIndexFocus + 2] = (GLubyte)((pData[nIndex] - fMin) * 254 / fRange);
			dataTexture[4 * nIndexFocus + 3] = (GLubyte)((pData[nIndex] - fMin) * 254 / fRange);
		}
	}

	return dataTexture;
}

void MeteModel::generateContourImp(const QList<ContourLine>& contourMin, const QList<ContourLine>& contourMax, QList<UnCertaintyArea*>& areas) {
	ContourStripGenerator generator;
	generator.Generate(areas, contourMin, contourMax, _nWidth, _nHeight, _nFocusX, _nFocusY, _nFocusW, _nFocusH);
}

void MeteModel::readData() {
	QFile file(_strFile);
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	char* temp = new char[file.size()];
	file.read(temp, file.size());
	float* f = (float*)temp;
	int x = file.size();

	for (int l = 0; l < _nEnsembleLen; l++)
	{
		// ��һ������֮ǰ��һ�����ݣ�֮��ļ���֮ǰ����������
		if (l == 0) f++;
		else f += 2;

		for (int i = 0; i < _nHeight; i++)
		{
			for (int j = 0; j < _nWidth; j++)
			{
				double fT = *f;
				_pData->SetData(l, i, j, *f++);
				// ֻȡ���ȣ�����0.5��
				if (_bFilter&&j < _nWidth - 1) f++;
			}
			// ֻȡ���ȣ�����0.5��
			if (_bFilter&&i < _nHeight - 1) f += (_nWidth * 2 - 1);
		}
	}
}