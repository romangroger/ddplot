#ifndef RDF_H
#define RDF_H

#include "pltwin.h"

bool CalcRDF(PltWin *pw);
void RDF(PltWin *pw, int which, double rmin, double rmax, int nbins, QVector<double> &rdfrad, QVector<double> &rdfg, 
	 double &gmax);

#endif
