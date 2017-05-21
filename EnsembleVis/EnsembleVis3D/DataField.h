// represent a data field
#pragma once
#include <QList>
class DataField
{
public:
	DataField(int w,int h,int l);
	~DataField();
private:
	// store the data
	double* _pBuf;

	// variance of each grid among different ensemble results
	double* _gridVari;
	// mean of each grid among different ensemble results
	double* _gridMean;
	double* _gridUMax;
	double* _gridUMin;

	int _nW;
	int _nH;
	int _nL;
public:
	// get the l'th layer
	const double* GetLayer(int l);
	const double* GetVari();
	const double* GetMean();
	const double* GetUMax();
	const double* GetUMin();
	double* GetEditableLayer(int l);
	// set the data value at a given position
	void SetData(int l, int bias, double dbValue);
	void SetData(int l, int y,int x, double dbValue);
	// get the data value at a given position
	double GetData(int l, int bias);
	const double* GetData() { return _pBuf; }
	void DoStatistic();
	// generate clustered data using the labels
	void GenerateClusteredData(const QList<int> listClusterLens, const int* arrLabels, QList<DataField*>& arrData);
};

