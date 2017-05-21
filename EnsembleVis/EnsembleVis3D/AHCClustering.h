// ============================================================
// agglomerative hierarchical clustering (AHC)
// Written by Mingdong
// 2017/05/12
// ============================================================
#pragma once
#include "Clustering.h"
class AHCClustering : public Clustering
{
public:
	AHCClustering();
	virtual ~AHCClustering();
protected:
	virtual void doCluster();
	// build the distance matrix
	void buildDistanceMatrix(double** disMatrix);
};

