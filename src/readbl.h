#ifndef READBL_H
#define READBL_H

#include <qstring.h>

#include "pltwin.h"

bool LoadBOP(QString fname, PltWin *pw);
bool LoadDD(QString fname, PltWin *pw);
bool LoadGB(QString fname, long int *blstart, PltWin *initConf, PltWin *pw);
bool LoadPLT(QString fname, PltWin *pw);
bool LoadXYZ(QString fname, PltWin *pw);

#endif
