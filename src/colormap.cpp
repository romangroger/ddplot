#include <math.h>

#include <qvector.h>
#include <qcolor.h>


void colormap(int ncol, QVector<QRgb> &cmap)
{
  float k, r, g, b;
  int i;

  cmap.clear();
  for (i=0; i<ncol; i++) {
    if (ncol > 1)
      k = i/double(ncol-1);
    else
      k = 0;
    r = pow(k,2);
    b = pow(1-k,2);
    g = 1-r-b;
    cmap.append(qRgb(round(255*r), round(255*g), round(255*b)));
  }
  //  for (i=0; i<ncol; i++) {
  //  printf("%d %d %d\n",qRed(cmap[i]),qGreen(cmap[i]),qBlue(cmap[i]));
  //}
}
