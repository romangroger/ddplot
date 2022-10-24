#include <iostream>
#include <stdlib.h>

#include "darray.h"


char const *sformat="%10.4lg";


void ErrMsg(const char *msg, const char *fun)
{
  printf("[%s] :: %s\n",fun,msg);
  exit(EXIT_FAILURE);
}

//---------------------------------------------------------------------------------------------------

/* output of the array to stdout */
template <class T, int rank>
std::ostream &operator<<(std::ostream &outp, DArray<T,rank> &A)
{
  int i,imax;

  imax = A.Length(1);

  printf("[ ");
  for (i=1; i<=imax; i++) {
    printf(sformat,A(i));          // %6.4le
    if (i<imax)
      printf("  ");
  }
  printf(" ]\n");

  return outp;
}

//---------------------------------------------------------------------------------------------------

// length of a vector
double vlength(DVector &vect)
{
  double vlength;
  int len,i;

  len = vect.Length(1);
  vlength = 0;
  for (i=1; i<=len; i++) {
    vlength += pow(vect(i),2);
  }
  vlength = sqrt(vlength);

  return vlength;
}


// implementation of the quicksort algorithm
void quicksort(DMatrix &list, int lo, int hi)
{
  int i=lo, j=hi;
  double x=list((lo+hi)/2,1);

  while (i<=j)
  {
    while (list(i,1)<x) i++;
    while (list(j,1)>x) j--;

    if (i<=j) {
      swaparr(list(i,1),list(j,1));
      swaparr(list(i,2),list(j,2));
      i++;
      j--;
    }
  }

  // rekurze
  if (lo<j) quicksort(list,lo,j);
  if (i<hi) quicksort(list,i,hi);
}


// trideni podle i-teho sloupce
void sort(DMatrix &mold, int start, int end, int col, IVector &indexes)
{
  DMatrix list;
  int row,nrows;

  nrows = end-start+1;
  list.Allocate(nrows,2);
  for (row=1; row<=nrows; row++) {
    list(start+row-1,1) = mold(row,col);          // value
    list(start+row-1,2) = row;                    // number of row
  }

  quicksort(list,start,end);
  for (row=1; row<=nrows; row++)
    indexes(row) = (int) list(row,2);
}
