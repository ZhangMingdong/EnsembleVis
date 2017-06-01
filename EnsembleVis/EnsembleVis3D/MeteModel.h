/*
	model of the meteorology data
	Mingdong
	2017/05/05
*/
#pragma once
#include <QGLWidget>
#include "ContourGenerator.h"
#include "ContourStripGenerator.h"
#include "BasicDataStructure.h"

class DataField;


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
	virtual QList<QList<ContourLine>> GetContour() { return _listContour; }
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
	// the data
	DataField* _pData;

	// signed distance function
	DataField* _pSDF;

	// data of each cluster
	QList<DataField*> _listClusterData;

	// cluster centers transfered back
	double* _pClusterCenter;

	// number of ensemble members
	int _nEnsembleLen;
	int _nWidth;
	int _nHeight;
	// width*height
	int _nLen;

	int _nFocusX;
	int _nFocusY;
	int _nFocusW;
	int _nFocusH;
	int _nFocusLen;

	int _nWest;
	int _nEast;
	int _nSouth;
	int _nNorth;
	int _nFocusWest;
	int _nFocusEast;
	int _nFocusSouth;
	int _nFocusNorth;



	// list of the uncertainty area of union of E
	QList<UnCertaintyArea*> _listUnionAreaE;
	QList<ContourLine> _listContourMinE;
	QList<ContourLine> _listContourMaxE;
	QList<ContourLine> _listContourMeanE;
	QList<QList<ContourLine>> _listContour;

	// list of the uncertainty area of union of E	(clustered)
	QList<QList<UnCertaintyArea*>> _listUnionAreaEC;
	QList<QList<ContourLine>> _listContourMinEC;
	QList<QList<ContourLine>> _listContourMaxEC;
	QList<QList<ContourLine>> _listContourMeanEC;
	QList<QList<QList<ContourLine>>> _listContourC;

	// list of the uncertainty area of union of E	(for gradient)
	QList<QList<UnCertaintyArea*>> _listUnionAreaEG;
	QList<QList<ContourLine>> _listContourMinEG;
	QList<QList<ContourLine>> _listContourMaxEG;

	// length of each cluster
	QList<int> _listClusterLen;
	// array of labels
	int* _arrLabels;
	// labels of each grid point
	int* _arrGridLabels;
	// number of clusters
	int _nClusters;


	// cluster mean of pca
	QList<QList<ContourLine>> _listContourMeanPCA;

	// file name of the data
	QString _strFile;

	// whether read binary file
	bool _bBinaryFile;

	// filtered the data between grids
	bool _bFilter;
private:
	// read ensemble data from text file
	void readDataFromText();

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
private:
	// do the spatial clustering for the grid point above threshold value
	void doSpatialClustering();
private:
	// Point list
	std::vector<Point> _points;
public:
	const std::vector<Point> GetPoints() { return _points; }
	

};

