#ifndef DPOS_PEIERLSNABARRO_H
#define DPOS_PEIERLSNABARRO_H

#include <qstring.h>
#include "pltwin.h"

bool CalcDPosPeierlsNabarro(PltWin *pw, QString &txtXY, QString &txtDPos);
bool findDPosPeierlsNabarro(PltWin *pw, DVector &dpos, int &nachain, IVector &achain, DVector &ubchain,
			    double &ubmin, double &ubmax);

#endif
