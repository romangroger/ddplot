#include <math.h>
#include <stdlib.h>
#include <qstring.h>

#include "darray.h"


// returns the atomic mass for a given element
double AtomicMass(QString elem)
{
  if (elem=="MO")
    return 95.94;
  else
    return 1.0;
}


// Calculation of an index into stacklen. Item is numbered from zero.
// If item is greater or equal than stacklen (exceeds the number of items in the stack),
// the idx is calculated from the beginning of the stack. In other words, idx can
// never exceed stacklen-1.
int CalcIndex(int item, int stacklen)
{
  div_t dstruct;

  if (item<stacklen)
    return item;
  else {
    dstruct = div(item,stacklen);
    return dstruct.rem;
  }
}


// Returns the file name with extension .dd.
QString CreateFName(QString fname, QString ext)
{
  QString fnew;
  int pos;

  fnew = fname;

  // if saving from .plt file...
  pos = fnew.indexOf(".plt");
  if (pos>=0)
    fnew.truncate(pos);

  // if saving .dd, don't append <ext> to make it .dd<ext>
  pos = fnew.indexOf(".dd");
  if (pos>=0)
    fnew.truncate(pos);

  fnew.append(ext); 

  return fnew;
}


// Compares two numbers and returns true if they are the same 
// within the specified tolerance
bool isequal(double num1, double num2, double tol=1e-4)
{
  return (num1<=num2+tol) && (num1>=num2-tol);
}


// Calculates the transformation matrix between two coordinate systems
void mtransf(DMatrix csys1, DMatrix csys2, DMatrix &mat)
{
  DMatrix cs1(3,3), cs2(3,3);
  double size1, size2;
  int i, j, ito, ifrom, idot;

  // normalization
  for (i=1; i<=3; i++) {
    size1 = 0;
    size2 = 0;
    for (j=1; j<=3; j++) {
      size1 += pow(csys1(i,j),2);
      size2 += pow(csys2(i,j),2);
    }
    size1 = sqrt(size1);
    size2 = sqrt(size2);

    for (j=1; j<=3; j++) {
      cs1(i,j) = csys1(i,j) / size1;
      cs2(i,j) = csys2(i,j) / size2;
    }
  }

  // transformation
  for (ito=1; ito<=3; ito++) {
    for (ifrom=1; ifrom<=3; ifrom++) {
      mat(ito,ifrom) = 0;

      for (idot=1; idot<=3; idot++)
	mat(ito,ifrom) += cs2(ito,idot) * cs1(ifrom,idot);

      if (fabs(mat(ito,ifrom))<=1e-16)
	mat(ito,ifrom) = 0;
    }
  }
}
