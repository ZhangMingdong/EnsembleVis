
#pragma once
#include <QGLWidget>
#include "ContourGenerator.h"
#include "ContourStripGenerator.h"
#include "BasicDataStructure.h"

class DataField;

/*
	model of the meteorology data
	Mingdong
	2017/05/05
*/
class MeteModel
{
public:
	MeteModel();
	virtual ~MeteModel();
public:
	// initialize the model
	virtual void InitModel(int nEnsembleLen, int nWidth, int nHeight, int nFocusX, int nFocusY, int nFocusW, int nFocusH
		, QString strFile, bool bBinary = false
		, int nWest = -179, int nEast = 180, int nSouth = -90, int nNorth = 90
		, int nFocusWest = -179, int nFocusEast = 180, int nFocusSouth = -90, int nFocusNorth = 90, bool bFiltered = false);
	// generate color mapping texture
	virtual GLubyte* generateTexture();
	virtual GLubyte* generateTextureMean();
	virtual GLubyte* generateTextureRange();
	virtual GLubyte* generateTextureGridCluster();
	// range of which index
	virtual GLubyte* generateTextureRange(int nIndex);
	virtual GLubyte* generateTextureSDF();
	virtual int GetW(){ return _nWidth; }
	virtual int GetH(){ return _nHeight; }
	virtual int GetFocusW(){ return _nFocusW; }
	virtual int GetFocusH(){ return _nFocusH; }
	virtual int GetFocusX(){ return _nFocusX; }
	virtual int GetFocusY(){ return _nFocusY; }
	virtual int GetWest() { return _nWest; }
	virtual int GetEast() { return _nEast; }
	virtual int GetSouth() { return _nSouth; }
	virtual int GetNorth() { return _nNorth; }
	virtual int GetFocusWest() { return _nFocusWest; }
	virtual int GetFocusEast() { return _nFocusEast; }
	virtual int GetFocusSouth() { return _nFocusSouth; }
	virtual int GetFocusNorth() { return _nFocusNorth; }
	virtual bool GetFilter() {
		return _bFilter; 
	}
	virtual QList<ContourLine> GetContourMin(){ return _listContourMinE; }
	virtual QList<ContourLine> GetContourMax(){ return _listContourMaxE; }
	virtual QList<ContourLine> GetContourMean(){ return _listContourMeanE; }
	virtual QList<QList<ContourLine>> GetContour();
	virtual QList<QList<ContourLine>> GetContourBrushed();
	virtual QList<QList<ContourLine>> GetContourNotBrushed();
	virtual QList<UnCertaintyArea*> GetUncertaintyArea(){ return _listUnionAreaE; }
	virtual int GetClusters() { return _nClusters; }
	virtual QList<ContourLine> GetContourMin(int nLabel) { return _listContourMinEC[nLabel]; }
	virtual QList<ContourLine> GetContourMax(int nLabel) { return _listContourMaxEC[nLabel]; }
	virtual QList<ContourLine> GetContourMean(int nLabel) { return _listContourMeanEC[nLabel]; }
	virtual QList<ContourLine> GetContourMeanPCA(int nLabel) { return _listContourMeanPCA[nLabel]; }
	virtual QList<QList<ContourLine>> GetContour(int nLabel) { return _listContourC[nLabel]; }
	virtual QList<UnCertaintyArea*> GetUncertaintyArea(int nLabel) { 
		return _listUnionAreaEC[nLabel]; 
	}
	virtual const QList<QList<UnCertaintyArea*> > GetUncertaintyAreaG() {
		return _listUnionAreaEG;
	}
protected:
	virtual void initializeModel();				// specialized model initialization
protected:
	// read ensemble data from text file
	virtual void readDataFromText();

	// pca and clustering
	void doPCA();

	/*
		calculate the signed distance function
		arrData:	the input data
		arrSDF:		the calculated sdf
		isoValue:	the iso value
		contour:	the generated contour
	*/
	void calculateSDF(const double* arrData, double* arrSDF, int nW, int nH, double isoValue, QList<ContourLine> contour);
	
	// space segmentation
	void generateContourImp(const QList<ContourLine>& contourMin, const QList<ContourLine>& contourMax, QList<UnCertaintyArea*>& areas);

	// read data from binary file
	void readData();
public:
	enum enumBackgroundFunction
	{
		bg_mean,			// mean
		bg_vari,			// variance
		bg_cluster,			// spatial cluster
		bg_sdf				// signed distance function
	};
protected:
	// using which background function
	enumBackgroundFunction _bgFunction;
public:
	void SetBgFunctionMean(enumBackgroundFunction f) { _bgFunction = f; }
	// brushing
	virtual void Brush(int nLeft, int nRight, int nTop, int nBottom);
protected:
	// do the spatial clustering for the grid point above threshold value
	void doSpatialClustering();
public:
	const std::vector<Point> GetPoints() { return _points; }

protected:	
	// 1.raw data
	DataField* _pData;						// the data		
	int _nEnsembleLen;						// number of ensemble members
	int _nWidth;
	int _nHeight;	
	int _nLen;								// _nWidth*_nHeight
	int _nFocusX;
	int _nFocusY;
	int _nFocusW;
	int _nFocusH;
	int _nFocusLen;							 //_nFocusW*_nFocusH
	int _nWest;
	int _nEast;
	int _nSouth;
	int _nNorth;
	int _nFocusWest;
	int _nFocusEast;
	int _nFocusSouth;
	int _nFocusNorth;

	// 2.basic contours and areas
	QList<UnCertaintyArea*> _listUnionAreaE;			// list of the uncertainty area of union of E
	QList<ContourLine> _listContourMinE;				// list of contours of minimum of E
	QList<ContourLine> _listContourMaxE;				// list of contours of maximum of E
	QList<ContourLine> _listContourMeanE;				// list of contours of mean of E
	QList<QList<ContourLine>> _listContour;				// list of contours of ensemble members
	QList<QList<ContourLine>> _listContourBrushed;		// list of brushed contours of ensemble members
	QList<QList<ContourLine>> _listContourNotBrushed;	// list of not brushed contours of ensemble members

protected:
	// 3.clustering related
	QList<QList<UnCertaintyArea*>> _listUnionAreaEC;	// list of the uncertainty area of union of E	(clustered)
	QList<QList<ContourLine>> _listContourMinEC;
	QList<QList<ContourLine>> _listContourMaxEC;
	QList<QList<ContourLine>> _listContourMeanEC;
	QList<QList<QList<ContourLine>>> _listContourC;
	
	QList<QList<UnCertaintyArea*>> _listUnionAreaEG;	// list of the uncertainty area of union of E	(for gradient)
	QList<QList<ContourLine>> _listContourMinEG;
	QList<QList<ContourLine>> _listContourMaxEG;

	DataField* _pSDF;						// signed distance function	
	QList<DataField*> _listClusterData;		// data of each cluster	
	double* _pClusterCenter;				// cluster centers transfered back

	
	QList<int> _listClusterLen;		// length of each cluster	
	int* _arrLabels;				// array of labels	
	int* _arrGridLabels;			// labels of each grid point	
	int _nClusters;					// number of clusters

	
	QList<QList<ContourLine>> _listContourMeanPCA;	// cluster mean of pca

	std::vector<Point> _points;	// Point list of the result of clustering

	// 0.io related	
	QString _strFile;				// file name of the data	
	bool _bBinaryFile;				// whether read binary file	
	bool _bFilter;					// filtered the data between grids	
};

