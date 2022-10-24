#include "QColor"
#include "QImage"

#include "ddplot.h"
#include "math.h"
#include "pltwin.h"


/*
  Take the indexed picture in sphere.png and assign the individual colors to a new
  colormap.
*/

void atomSphere(QColor col, QImage &img)
{
  double f;
  int colr, colg, colb, i;
  
  int cr = col.red();
  int cg = col.green();
  int cb = col.blue();

  img = GRAY_SPHERE;
  QVector<QRgb> ctable;
  ctable.append(qRgba(0,0,0,0));
  for (i=1; i<=255; i++) {
    f = pow(i/255.0,0.5);
    colr = round(cr*f);
    colg = round(cg*f);
    colb = round(cb*f);
    ctable.append(qRgb(colr,colg,colb));
  }
  img.setColorTable(ctable);
}


/*
  Prepare as many colored spheres to render atoms as we have the number of Z layers.
*/

void PltWin::PrepareZColoredSpheres()
{
  sphere.resize(NumZLayers+1);
  atomSphere(Qt::lightGray, sphere[0]);                 // inert atom
  for (int layer=1; layer<=NumZLayers; layer++) {       // active atoms
    atomSphere(zColorLayer(layer,2), sphere[layer]);
  }
}
