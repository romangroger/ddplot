#ifndef DARRAY_H
#define DARRAY_H

#include <iostream>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>


/*------------------------------------------------------------------------------------------------
  class DArray - declarations
------------------------------------------------------------------------------------------------*/

template <class T, int rank>
class DArray {
public:
  int items=0;           // total number of items in the array
  int dim=0;              // dimension of the array (1=vector, 2=matrix, etc.
  T *data=NULL;         // stores the data
  int *len=NULL;             // number of items along the specific directions of the array
  int *estep=NULL;           // jumps for "inflating" of the array (auto_enlarge must be true)
  bool auto_enlarge=false;    // is the enlarging of the structure permitted ?

  DArray() { }                    // constructor without allocation
  DArray(int ilen, ...);          // constructor with allocation of space for data
  DArray(const DArray<T,rank> &t);      // copy constructor
  ~DArray();                      // destructor

  void Allocate(int ilen, ...);   // allocation of the space for data
  void Free();                    // freeing the space occupied by data

  T &operator()(int i);           // read/write into the array; these are defined explicitly for
  T &operator()(int i, int j);    // each rank of array for fast access
  T &operator()(int i, int j, int k);
  T &operator()(int i, int j, int k, int l);

  DArray<T,rank> &operator=(const T &value);
  const DArray<T,rank> operator+(const T &value);
  const DArray<T,rank> operator-(const T &value);
  const DArray<T,rank> operator*(const T &value);
  const DArray<T,rank> operator/(const T &value);
  DArray<T,rank> &operator+=(const T &value);
  DArray<T,rank> &operator-=(const T &value);
  DArray<T,rank> &operator*=(const T &value);
  DArray<T,rank> &operator/=(const T &value);
  
  DArray<T,rank> &operator=(const DArray<T,rank> &t);
  DArray<T,rank> &operator+(const DArray<T,rank> &t);
  DArray<T,rank> &operator-(const DArray<T,rank> &t);
  DArray<T,rank> &operator*(const DArray<T,rank> &A2);
  DArray<T,rank> &operator+=(const DArray<T,rank> &t);
  DArray<T,rank> &operator-=(const DArray<T,rank> &t);

  void EnlargeStep(int istep, ...);
  void Fill(T value);                         // assign all components of the array the given value
  int Length(int dir) { return len[dir-1]; }  // get the length of the array in the direction dir
  int Dim(void) { return dim; }               // get the dimension of the array
  double Max(DArray<T,rank> &arr);            // maximum item in an array
  void Resize(int i, ...);                    // resize the array by estep (if auto_enlarge=true)
};

template <class T> void swaparr(T &a, T &b);  // swap two values


/*------------------------------------------------------------------------------------------------
  class DArray - derived types
------------------------------------------------------------------------------------------------*/

// arrays with elements of type double
typedef DArray<double,1> DVector;
typedef DArray<double,2> DMatrix;
typedef DArray<double,3> DArray3;
typedef DArray<double,4> DArray4;

// arrays with elements of type int
typedef DArray<int,1> IVector;
typedef DArray<int,2> IMatrix;
typedef DArray<int,3> IArray3;
typedef DArray<int,4> IArray4;

// arrays with elements of type bool
typedef DArray<bool,1> BVector;
typedef DArray<bool,2> BMatrix;
typedef DArray<bool,3> BArray3;
typedef DArray<bool,4> BArray4;


/*------------------------------------------------------------------------------------------------
  class DArray - support functions (definitions in darray.cpp)
------------------------------------------------------------------------------------------------*/

void disp(DVector &A);
void disp(DMatrix &A);
void disp(DArray3 &A);
void disp(DArray4 &A);

void ErrMsg(const char *msg, const char *fun);   // handling of error messages
double vlength(DVector &vect);       // velikost vektoru
void sort(DMatrix &mold, int start, int end, int col, IVector &indexes);  // sort by the i-th column


/*------------------------------------------------------------------------------------------------
  class DArray - constructors and destructors
------------------------------------------------------------------------------------------------*/

// constructor for an array of any rank
template <class T, int rank>
DArray<T,rank>::DArray(int ilen, ...)
{
  va_list args;

  dim = rank;
  len = (int *) malloc(dim*sizeof(int));
  estep = (int *) malloc(dim*sizeof(int));

  va_start(args, ilen);
  len[0] = items = ilen;
  for (int i=1; i<rank; i++) {
    len[i] = va_arg(args, int);
    items *= len[i];
  }
  va_end(args);

  auto_enlarge = false;
  std::fill_n(estep, dim, 0);

  data = (T *) malloc(items*sizeof(T));
  if (data==NULL && items>0)  {
    const char *fun="DArray<T,rank> (constructor)";
    ErrMsg("Allocation of the space for data unsuccessful.", fun);
  }
  Fill(0);
}


// copy constructor
template <class T, int rank>
DArray<T,rank>::DArray(const DArray<T,rank> &t)
{
  dim = t.dim;
  items = t.items;
  auto_enlarge = t.auto_enlarge;

  if (len==NULL) len = (int *) malloc(dim*sizeof(int));
  if (estep==NULL) estep = (int *) malloc(dim*sizeof(int));
  memcpy(len, t.len, t.dim*sizeof(int));
  memcpy(estep, t.estep, t.dim*sizeof(int));

  if (data!=NULL) free(data);
  data = (T *) malloc(t.items*sizeof(T));
  if (data==NULL && t.items>0) {
    const char *fun="DArray<T,rank> (copy constructor)";
    ErrMsg("Allocation of the space for data unsuccessful.", fun);
  }
  memcpy(data, t.data, t.items*sizeof(T));
}


// destructor
template <class T, int rank>
DArray<T,rank>::~DArray()
{
  Free();
}


// dynamic allocation during the run of the program
// - declare:   DArray<T,rank> A;  (don't know the extent yet)
// - allocate:  A.Allocate(...);
template <class T, int rank>
void DArray<T,rank>::Allocate(int ilen, ...)
{
  va_list args;

  // if the array is already allocated, free it first (this is safe !)
  Free();

  dim = rank;
  len = (int *) malloc(dim*sizeof(int));
  estep = (int *) malloc(dim*sizeof(int));
  va_start(args, ilen);
  len[0] = items = ilen;
  for (int i=1; i<rank; i++) {
    len[i] = va_arg(args, int);
    items *= len[i];
  }
  va_end(args);

  auto_enlarge = false;
  std::fill_n(estep, dim, 0);

  data = (T *) malloc(items*sizeof(T));
  if (data==NULL && items>0) {
    const char *fun="DArray<T,rank>::Allocate";
    ErrMsg("Allocation of the space for data unsuccessful.", fun);
  }
}


// deallocation during the run of the program
template <class T, int rank>
void DArray<T,rank>::Free()
{
  if (data!=NULL) {
    free(data);
    data = NULL;
  }

  if (len!=NULL) {
    free(len);
    len = NULL;
  }

  if (estep!=NULL) {
    free(estep);
    estep = NULL;
  }

  dim = items = 0;
  auto_enlarge = false;
}


/*------------------------------------------------------------------------------------------------
  class DArray - operators
------------------------------------------------------------------------------------------------*/

// reading/writing into a vector
template <class T, int rank>
T &DArray<T,rank>::operator()(int i)
{
  if (auto_enlarge) {
    if (i>len[0]) Resize(i);
  } else {
    if (i<1 || i>len[0]) {
      const char *fun="DArray<T,rank>::operator(i)";
      ErrMsg("Attempt to read/write outside an array.", fun);
    }
  }
  long pos = i-1;
  return data[pos];
}


// reading/writing into a matrix
template <class T, int rank>
  T &DArray<T,rank>::operator()(int i, int j)
{
  if (auto_enlarge) {
    if (i>len[0] || j>len[1]) Resize(i, j);
  } else {
    if (i<1 || i>len[0] || j<1 || j>len[1]) {
      const char *fun="DArray<T,rank>::operator(i,j)";
      ErrMsg("Attempt to read/write outside an array.", fun);
    }
  }
  long pos = j-1 + len[1]*(i-1);
  return data[pos];
}


// reading/writing into a rank-3 array
template <class T, int rank>
  T &DArray<T,rank>::operator()(int i, int j, int k)
{
  if (auto_enlarge) {
    if (i>len[0] || j>len[1] || k>len[2]) Resize(i, j, k);
  } else {
    if (i<1 || i>len[0] || j<1 || j>len[1] || k<1 || k>len[2]) {
      const char *fun="DArray<T,rank>::operator(i,j,k)";
      ErrMsg("Attempt to read/write outside an array.", fun);
    }
  }
  long pos = k-1 + len[2]*(j-1) + len[1]*len[2]*(i-1);
  return data[pos];
}


// reading/writing into a rank-4 array
template <class T, int rank>
  T &DArray<T,rank>::operator()(int i, int j, int k, int l)
{
  if (auto_enlarge) {
    if (i>len[0] || j>len[1] || k>len[2] || l>len[3]) Resize(i, j, k, l);
  } else {
    if (i<1 || i>len[0] || j<1 || j>len[1] || k<1 || k>len[2] || l<1 || l>len[3]) {
      const char *fun="DArray<T,rank>::operator(i,j,k,l)";
      ErrMsg("Attempt to read/write outside an array.", fun);
    }
  }
  long pos = l-1 + len[3]*(k-1) + len[2]*len[3]*(j-1) + len[1]*len[2]*len[3]*(i-1);
  return data[pos];
}


// fills the array with a constant value
template <class T, int rank>
DArray<T,rank> &DArray<T,rank>::operator=(const T &value)
{ 
  Fill(value); 
  return *this; 
}


// overload of the addition operator
template <class T, int rank>
const DArray<T,rank> DArray<T,rank>::operator+(const T &value)
{
  DArray<T,rank> res = *this;
  res += value;
  return res;
}


// overload of the subtraction operator
template <class T, int rank>
const DArray<T,rank> DArray<T,rank>::operator-(const T &value)
{
  DArray<T,rank> res = *this;
  res -= value;
  return res;
}


// overload of the multiplication operator
template <class T, int rank>
const DArray<T,rank> DArray<T,rank>::operator*(const T &value)
{
  DArray<T,rank> res = *this;
  res *= value;
  return res;
}


// overload of the division operator
template <class T, int rank>
const DArray<T,rank> DArray<T,rank>::operator/(const T &value)
{
  DArray<T,rank> res = *this;
  res /= value;
  return res;
}


// overload of the += operator
template <class T, int rank>
DArray<T,rank> &DArray<T,rank>::operator+=(const T &value)
{
  for (int i=0; i<items; i++)
    data[i] += value;
  return *this;
}


// overload of the -= operator
template <class T, int rank>
DArray<T,rank> &DArray<T,rank>::operator-=(const T &value)
{
  for (int i=0; i<items; i++)
    data[i] -= value;
  return *this;
}


// overload of the *= operator
template <class T, int rank>
DArray<T,rank> &DArray<T,rank>::operator*=(const T &value)
{
  for (int i=0; i<items; i++)
    data[i] *= value;
  return *this;
}


// overload of the /= operator
template <class T, int rank>
DArray<T,rank> &DArray<T,rank>::operator/=(const T &value)
{
  for (int i=0; i<items; i++)
    data[i] /= value;
  return *this;
}

//----------------------------------------------------------------------------------------------------

// addition of arrays if they are of the same dimension and extent
template <class T, int rank>
 DArray<T,rank> &DArray<T,rank>::operator+(const DArray<T,rank> &t)
{
  const char *fun="DArray<T,rank>::operator+";
  static DArray<T,rank> result = t;

  if (this->dim!=t.dim || this->items!=t.items)
    ErrMsg("Addition of dimensionally incompatible arrays.",fun);

  // can I make this faster (assembler code) ?
  for (int i=0; i<items; i++)
    result.data[i] = data[i]+t.data[i];

  return result;
}


// operator of addition to the target
template <class T, int rank>
DArray<T,rank> &DArray<T,rank>::operator+=(const DArray<T,rank> &t)
{
  *this = *this+t;
  return *this;
}


// operation of subtraction
template <class T, int rank>
DArray<T,rank> &DArray<T,rank>::operator-(const DArray<T,rank> &t)
{
  static DArray<T,rank> result = t;

  if (this->dim!=t.dim || this->items!=t.items) {
    const char *fun="DArray<T,rank>::operator-";
    ErrMsg("Subtraction of dimensionally incompatible arrays.", fun);
  }

  // can I make this faster (assembler code) ?
  for (int i=0; i<items; i++)
    result.data[i] = data[i]-t.data[i];

  return result;
}


// operator of subtraction from the target 
template <class T, int rank>
DArray<T,rank> &DArray<T,rank>::operator-=(const DArray<T,rank> &t)
{
  *this = *this-t;
  return *this;
}

//----------------------------------------------------------------------------------------------------

// copy operator (copies t to *this)
template <class T, int rank>
DArray<T,rank> &DArray<T,rank>::operator=(const DArray<T,rank> &t)
{
  size_t size;
  
  Free();

  dim = t.dim;
  items = t.items;
  auto_enlarge = t.auto_enlarge;

  size = dim*sizeof(int);
  len = (int *) malloc(size);
  memcpy(len, t.len, size);

  size = dim*sizeof(int);
  estep = (int *) malloc(size);
  memcpy(estep, t.estep, size);

  size = items*sizeof(T);
  data = (T *) malloc(size);
  if (data==NULL && items>0) {
    const char *fun="DArray<T,rank>::operator=(DArray)";
    ErrMsg("Allocation of the space for data unsuccessful", fun);
  }
  memcpy(data, t.data, size);

  return *this;
}


// Operator of multiplication of two arrays (dimensionally compatible !)
// This is supposed to be used for matrices only - not too general yet.
template <class T, int rank>
DArray<T,rank> &DArray<T,rank>::operator*(const DArray<T,rank> &A2)
{
  const char *fun="DArray<T,rank>::operator*";
  static DArray<T,rank> A1 = *this;
  static DArray<T,rank> target;    // the allocation is below
  double sum;
  int new_len[4];
  int new_dim;
  int rmax1,cmax1,cmax2,d,dmax;
  int row,col,idx;

  // is the multiplication possible ?
  if (A1.Length(A1.dim)!=A2.Length(1))
    ErrMsg("Multiplication of dimensionally incompatible arrays.",fun);

  // the dimension of the resulting array
  new_dim = 0;
  dmax = A1.Dim()-1;
  for (d=1; d<=dmax; d++) {
    new_len[new_dim] = A1.Length(d);
    new_dim++;
  }

  dmax = A2.Dim();
  for (d=2; d<=dmax; d++) {
    new_len[new_dim] = A2.Length(d);
    new_dim++;
  }

  // alokace vysledne matice
  switch (new_dim) {
    case 1:
      target.Allocate(new_len[0]);
      break;

    case 2:
      target.Allocate(new_len[0],new_len[1]);
      break;

    case 3:
      target.Allocate(new_len[0],new_len[1],new_len[2]);
      break;

    case 4:
      target.Allocate(new_len[0],new_len[1],new_len[2],new_len[3]);
  }

  // nasobeni
  if (A1.Dim()==2) {
    rmax1 = A1.Length(1);
    cmax1 = A1.Length(2);
    cmax2 = A2.Length(2);

    for (row=1; row<=rmax1; row++) {
      for (col=1; col<=cmax2; col++) {
        sum = 0;
        for (idx=1; idx<=cmax1; idx++)
          sum += A1(row,idx)*A2(idx,col);
        target(row,col) = sum;
      }
    }
  } else
    ErrMsg("Multiplication of these dimension has not been provided yet.",fun);

  return target;
}


/*------------------------------------------------------------------------------------------------
  class DArray - main functions
------------------------------------------------------------------------------------------------*/

// set the steps for enlarging the array - automatically toggles the auto_enlarge flag to true
template <class T, int rank>
void DArray<T,rank>::EnlargeStep(int istep, ...)
{
  va_list args;

  auto_enlarge = true;
  estep[0] = istep;
  va_start(args, istep);
  for (int i=1; i<rank; i++)
    estep[i] = va_arg(args, int);
  va_end(args);
}


// fill the array by a number
template <class T, int rank>
void DArray<T,rank>::Fill(T value)
{
  std::fill_n(data, items, value);
}


// find the maximum value in an array
template <class T, int rank>
T Max(DArray<T,rank> &arr)
{
  T max_item;
  long i;

  max_item = arr.data[0];
  for (i=1; i<arr.items; i++) 
    if (arr.data[i]>max_item) 
      max_item = arr.data[i];
  
  return max_item;
}


// enlarging the extent (number of values in each direction) of an array
template <class T, int rank>
void DArray<T,rank>::Resize(int inew, ...)
{
  const char *fun="DArray<T,rank>::Resize";
  va_list args;
  T *new_data;
  unsigned int idx_target, idx_source;
  int ilen, jlen, klen, llen, newlen[dim];
  int i, j, k;

  if (!auto_enlarge) {
    ErrMsg("Cannot resize a fixed-size array (use EnlargeStep)",fun);
    return;
  }

  // read the arguments
  newlen[0] = inew;
  va_start(args, inew);
  for (i=1; i<rank; i++)
    newlen[i] = va_arg(args, int);
  va_end(args);

  // calculate of the new size of the array
  items = 1;
  ilen = jlen = klen = llen = 0;

  if (newlen[0]>len[0] && estep[0]>0) {
    ilen = len[0] + ceil((newlen[0]-len[0])/double(estep[0]))*estep[0];
    items *= ilen;
  } else {
    items *= len[0];
    ilen = len[0];
  }

  if (dim>1) {
    if (newlen[1]>len[1] && estep[1]>0) {
      jlen = len[1] + ceil((newlen[1]-len[1])/double(estep[1]))*estep[1];
      items *= jlen;
    } else {
      items *= len[1];
      jlen = len[1];
    }
  }

  if (dim>2) {
    if (newlen[2]>len[2] && estep[2]>0) {
      klen = len[2]+ceil((newlen[2]-len[2])/double(estep[2]))*estep[2];
      items *= klen;
    } else {
      items *= len[2];
      klen = len[2];
    }
  }

  if (dim>3) {
    if (newlen[3]>len[3] && estep[3]>0) {
      llen = len[3] + ceil((newlen[3]-len[3])/double(estep[3]))*estep[3];
      items *= llen;
    } else {
      items *= len[3];
      llen = len[3];
    }
  }

  new_data = (T *) malloc(items*sizeof(T));
  if (new_data==NULL && items>0) {
    ErrMsg("Allocation of the space for data unsuccessful.", fun);
    return;
  }

  std::fill_n(new_data, items, 0);

  switch (dim) {
    case 1:
      memcpy(new_data, data, len[0]*sizeof(T));
      len[0] = ilen;
      break;

    case 2:
      idx_source = idx_target = 0;
      for (i=0; i<len[0]; i++) {
	memcpy(new_data+idx_target, data+idx_source, len[1]*sizeof(T));
	idx_target += jlen;
	idx_source += len[1];
      }
      len[0] = ilen;
      len[1] = jlen;
      break;

    case 3:
      idx_source = idx_target = 0;
      for (i=0; i<len[0]; i++) {
        for (j=0; j<len[1]; j++) {
	  memcpy(new_data+idx_target, data+idx_source, len[2]*sizeof(T));
	  idx_target += klen;
	  idx_source += len[2];
	}
	idx_target += (jlen-len[1])*klen;  // the empty space left by enlarging
      }
      len[0] = ilen;
      len[1] = jlen;
      len[2] = klen;
      break;

    case 4:
      idx_source = idx_target = 0;
      for (i=0; i<len[0]; i++) {
        for (j=0; j<len[1]; j++) {
          for (k=0; k<len[2]; k++) {
	    memcpy(new_data+idx_target, data+idx_source, len[3]*sizeof(T));
	    idx_target += llen;
	    idx_source += len[3];
          }
	  idx_target += (klen-len[2])*llen;  // the empty space left by enlarging
        }
	idx_target += (jlen-len[1])*klen*llen;  // the empty space left by enlarging
      }
      len[0] = ilen;
      len[1] = jlen;
      len[2] = klen;
      len[3] = llen;      
      break;
  }

  free(data);
  data = (T *) malloc(items*sizeof(T));
  if (data==NULL && items>0) {
    ErrMsg("Allocation of the space for data unsuccessful.", fun);
    return;
  }
  memcpy(data, new_data, items*sizeof(T));
  free(new_data);
}


// swap two numbers
template <class T>
void swaparr(T &a, T &b)
{
  T dum;
  dum = a;
  a = b;
  b = dum;
}

#endif
