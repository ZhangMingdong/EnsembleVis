// ============================================================
// Interface for clustering
// Written by Mingdong
// 2017/05/12
// ============================================================
#pragma once
class Clustering
{
public:
	Clustering();
	virtual ~Clustering();
protected:
	int _n;
	int _m;
	int _k;
	const double *_arrData;
	int* _arrLabels;
protected:
	/*
		implementation of clustering
	*/
	virtual void doCluster() = 0;
public:
	/*
		set input data
		n:			number of vectors
		m:			length of vectors
		r:			number of clusters
		arrData:	data buffer
		arrLabels:	return the labels. [length:n][0~r-1]
	*/
	void DoCluster(int n, int m, int k, const double *arrData, int* arrLabels);
};

