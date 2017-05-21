#include "AHCClustering.h"
#include <math.h>

#include <QDebug>


AHCClustering::AHCClustering()
{
}


AHCClustering::~AHCClustering()
{
}

void AHCClustering::doCluster() {
	// 0.initialize the distance matrix
	double** disMatrix = new double*[_n];
	buildDistanceMatrix(disMatrix);

	// 2.clustering
	for (int i = 0; i < _n; i++)
	{
		_arrLabels[i] = i;
	}
	// start from every grid point as a cluster
	int nClusters = _n;
	// state of each grid point
	int* arrState = new int[_n];
	for (int i = 0; i<_n; i++) arrState[i] = 1;	// 0 means this grid has been merged to other cluster
	while (nClusters >_k)
	{
		// 2.1.Find the nearest points pair nI,nJ
		int nI, nJ;
		double fMinDis = 10000.0;
		for (int i = 0; i < _n; i++)
		{
			if (arrState[i] == 0) continue;
			// 			int latI = i / g_focus_w;			// latitude of index i
			// 			int lonI = i%g_focus_w;

			for (int j = 0; j < i; j++)
			{
				if (arrState[j] == 0) continue;
				// 				int latJ = j / g_focus_w;			// latitude of index j
				// 				int lonJ = j%g_focus_w;
				// 				double fBiasLat = latJ - latI;
				// 				double fBiasLon = lonJ - lonI;
				if (_arrLabels[i] == _arrLabels[j]) continue;
				if (disMatrix[i][j] < fMinDis)
				{
					nI = i;
					nJ = j;
					fMinDis = disMatrix[i][j];
				}
			}
		}

		int nSourceLabel = _arrLabels[nI];
		int nTargetLabel = _arrLabels[nJ];


		// 2.2.search all the point with source label, reset distance and state
		for (int i = 0; i < _n; i++)
		{
			if (_arrLabels[i] == nSourceLabel)
			{
				// 2.2.1.reset the label
				_arrLabels[i] = nTargetLabel;
				if (arrState[i] == 1)
				{
					// 2.2.2.reset distance
					for (int j = 0; j < _n; j++)
					{
						if (j == i || j == nJ) continue;
						// as we merge i to nJ
						// for each j, if dis(i,j)>dis(nJ,j), we use dis(i,j) to replace dis(nJ,j)
						double fDisIJ;
						if (i < j) fDisIJ = disMatrix[j][i];
						else fDisIJ = disMatrix[i][j];
						if (j < nJ) {
							disMatrix[nJ][j] = fmax(disMatrix[nJ][j], fDisIJ);
						}
						else {
							disMatrix[j][nJ] = fmax(disMatrix[j][nJ], fDisIJ);

						}
					}
					arrState[i] = 0;
				}
			}
		}
		nClusters--;
	}


	// 3.release the resource
	for (int i = 0; i < _n; i++)
	{
		delete[]disMatrix[i];
	}
	delete[] disMatrix;
	delete[] arrState;

//	for (int i = 0; i < _n; i++) qDebug() << _arrLabels[i];
	int nBias = 0;
	for (int i = 0; i < _n; i++)
	{
		// if i is a valid label
		bool bValidLabel = false;
		for (int j = 0; j < _n; j++)
		{
			if (_arrLabels[j] == i) {
				_arrLabels[j] -= nBias;
				bValidLabel = true;
			}
		}
		if (!bValidLabel) nBias++;
	}
	// 4.save the labels
	{
		/*
		// print them
		QFile caFile("Label.txt");
		caFile.open(QIODevice::WriteOnly | QIODevice::Text);

		if (!caFile.isOpen()) {
			qDebug() << "- Error, unable to open" << "outputFilename" << "for output";
		}
		QTextStream outStream(&caFile);
		for (int i = 0; i < g_focus_l; i++)
		{
			outStream << _gridLabels[i] << endl;
		}
		caFile.close();
		*/
	}

}


void AHCClustering::buildDistanceMatrix(double** disMatrix) {
	// 0.allocate resource
	for (int i = 0; i < _n; i++)
	{
		disMatrix[i] = new double[_n];
	}
	// 1.calculate the distance
	for (int i = 0; i < _n; i++)
	{
		for (int j = 0; j < i; j++)
		{
			double dis = 0;
			for (int k = 0; k < _m; k++)
			{
				double bias = _arrData[i*_m + k] - _arrData[j*_m + k];
				dis += bias*bias;
			}
			disMatrix[i][j] = sqrt(dis);
		}
	}
}