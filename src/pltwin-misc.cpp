#include <math.h>
#include <stdlib.h>

#include "pltwin.h"



/*
  This function determines the position of a point in the world space that is given by its screen
  coordinates, i.e. pixels (xscreen,yscreen). The world position is returned as (xworld,yworld). 
*/

void PltWin::xyScreenToWorld(int xs, int ys, double &xw, double &yw)
{

}


/*
  This function determines the position of a point on the screen that is given by its world
  coordinates (xworld,yworld). The screen position is returned as (xscreen,yscreen). 
*/

void PltWin::xyWorldToScreen(double xw, double yw, int &xs, int &ys)  
{
  //xs = ZFact*xw - xPan - (ZFact-1)*xyzCen(1);
  //ys = ZFact*yw - yPan - (ZFact-1)*xyzCen(2);
  xs = xBorder + xOffset + (int) floor(factor*(xw-xyzMin(1))); 
  ys = height() - (yBorder + yOffset + (int) floor(factor*(yw-xyzMin(2))));
}

