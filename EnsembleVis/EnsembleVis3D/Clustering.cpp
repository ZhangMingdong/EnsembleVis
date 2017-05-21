#include "Clustering.h"



Clustering::Clustering()
{
}


Clustering::~Clustering()
{
}

void Clustering::DoCluster(int n, int m, int k, const double *arrData, int* arrLabels) {
	_n = n;
	_m = m;
	_k = k;
	_arrData = arrData;
	_arrLabels = arrLabels;
	doCluster();
}