#ifndef NEIGHLIST_H
#define NEIGHLIST_H

#include "pltwin.h"
#include "darray.h"


bool InitNeighborList( PltWin *pw, IMatrix &nnlist, IVector &nnum );
bool RelNeighborList( PltWin *pw, int rmax, IMatrix &nnlist, IVector &nnum );

void NeighInit( PltWin *pw );
void NeighList( PltWin *pw );
void GetCell( PltWin *pw, int i, IVector &icell );


#endif
