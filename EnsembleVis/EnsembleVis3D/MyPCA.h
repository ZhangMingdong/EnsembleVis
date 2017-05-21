// ============================================================
// PCA
// Written by Mingdong
// 2017/05/13
// ============================================================
#pragma once

#include "tnt_array2d.h"
class MyPCA
{
public:
	MyPCA();
	~MyPCA();
public:
	/*
		do the pca
		n:			number of vectors
		m:			length of the vectors
		arrInput:	input data: n*m
		arrOutput:	output data: n*(n-1)
		bNewData:	weather recalculate for new data
	*/
	void DoPCA(const double* arrInput, double* arrOutput, int n, int m, bool bNewData = false);

	/*
		recover nRow of vectors
	*/
	void Recover(double* arrInput, double* arrOutput,int nRow);
private:
	int _nRow;
	int _nCol;
	double* _arrDataMean;

	TNT::Array2D<double>* _pR;				// function to transform back
	TNT::Array2D<double>* _pCovar_matrix;
	TNT::Array2D<double>* _pEigenvector;
	TNT::Array2D<double>* _pEigenvalue;
	TNT::Array2D<double>* _pD;				// input data
private:
	// read the four matrices
	void readMatrices();
	// calculate the four matrices
	void calculateMatrices();
	// write the four matrices
	void writeMatrices();
};

